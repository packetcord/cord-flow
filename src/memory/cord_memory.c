#include <memory/cord_memory.h>
#include <cord_error.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void* cord_huge_pages_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    // Calculate real allocation size with metadata page
    size_t real_size = CORD_ALIGN_TO_HUGE_PAGE(size + CORD_HUGE_PAGE_SIZE);
    
    // Attempt huge page allocation first
    void* ptr = mmap(NULL, real_size, 
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB,
                     -1, 0);
    
    if (ptr == MAP_FAILED) {
        // Huge page allocation failed, fallback to regular malloc
        ptr = malloc(real_size);
        if (ptr == NULL) {
            return NULL;
        }
        // Mark as malloc allocation by setting size to 0
        *((size_t*)ptr) = 0;
    } else {
        // Mark as mmap allocation by storing actual size
        *((size_t*)ptr) = real_size;
    }
    
    // Return pointer after metadata page
    return (uint8_t*)ptr + CORD_HUGE_PAGE_SIZE;
}

void cord_huge_pages_free(void* ptr)
{
    if (ptr == NULL) {
        return;
    }
    
    // Get pointer to metadata page
    void* real_ptr = (uint8_t*)ptr - CORD_HUGE_PAGE_SIZE;
    
    // Read allocation metadata
    size_t real_size = *((size_t*)real_ptr);
    
    if (real_size == 0) {
        // Was allocated with malloc, use free
        free(real_ptr);
    } else {
        // Was allocated with mmap, use munmap
        assert(real_size % CORD_HUGE_PAGE_SIZE == 0);
        int result = munmap(real_ptr, real_size);
        if (result != 0) {
            CORD_ERROR("[cord_huge_pages_free] munmap");
        }
    }
}

size_t cord_huge_pages_get_size(const void* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    
    // Get pointer to metadata page
    const void* real_ptr = (const uint8_t*)ptr - CORD_HUGE_PAGE_SIZE;
    
    // Read stored size
    size_t real_size = *((const size_t*)real_ptr);
    
    if (real_size == 0) {
        // malloc allocation - size not tracked
        return 0;
    }
    
    // Return usable size (minus metadata page)
    return real_size - CORD_HUGE_PAGE_SIZE;
}

bool cord_huge_pages_available(void)
{
    // Try to allocate a minimal huge page to test availability
    void* test_ptr = mmap(NULL, CORD_HUGE_PAGE_SIZE,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                          -1, 0);
    
    if (test_ptr == MAP_FAILED) {
        return false;
    }
    
    // Clean up test allocation
    munmap(test_ptr, CORD_HUGE_PAGE_SIZE);
    return true;
}

void* cord_cache_aligned_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    void* ptr;
    int result = posix_memalign(&ptr, CORD_CACHE_LINE_SIZE, size);
    
    if (result != 0) {
        return NULL;
    }
    
    return ptr;
}

void cord_cache_aligned_free(void* ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

#ifdef ENABLE_DPDK_DATAPLANE

struct rte_mempool* cord_pktmbuf_mpool_alloc(const char *name, unsigned int n, unsigned int cache_size)
{
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(name, n, cache_size, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

    return mbuf_pool;
}

void cord_pktmbuf_mpool_free(struct rte_mempool **mbuf_pool)
{
    if (*mbuf_pool != NULL)
    {
        rte_mempool_free(*mbuf_pool);
        *mbuf_pool = NULL;
    }

    return;
}

#endif // ENABLE_DPDK_DATAPLANE

struct cord_tpacketv3_ring* cord_tpacketv3_ring_alloc(uint32_t block_size, uint32_t frame_size, uint32_t block_num)
{
    struct cord_tpacketv3_ring *ring = calloc(1, sizeof(struct cord_tpacketv3_ring));
    if (ring == NULL)
    {
        CORD_ERROR("[cord_tpacketv3_ring_alloc] calloc");
        return NULL;
    }

    memset(&ring->req, 0, sizeof(ring->req));
    ring->req.tp_block_size = block_size;
    ring->req.tp_frame_size = frame_size;
    ring->req.tp_block_nr = block_num;
    ring->req.tp_frame_nr = (block_size * block_num) / frame_size;
    ring->req.tp_retire_blk_tov = 1;
    ring->req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;

    return ring;
}

void cord_tpacketv3_ring_init(struct cord_tpacketv3_ring **ring)
{
    (*ring)->map_size = (*ring)->req.tp_block_size * (*ring)->req.tp_block_nr;

    (*ring)->map = mmap(NULL, (*ring)->map_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_LOCKED | MAP_POPULATE, (*ring)->fd, 0);
    
    if ((*ring)->map == MAP_FAILED)
    {
        CORD_ERROR("[cord_tpacketv3_ring_init] mmap");
        return;
    }

    (*ring)->iov_ring = malloc((*ring)->req.tp_block_nr * sizeof(struct iovec));
    if (!(*ring)->iov_ring)
    {
        CORD_ERROR("[cord_tpacketv3_ring_init] malloc");
        munmap((*ring)->map, (*ring)->map_size);
        return;
    }

    for (uint32_t i = 0; i < (*ring)->req.tp_block_nr; i++)
    {
        (*ring)->iov_ring[i].iov_base = (*ring)->map + (i * (*ring)->req.tp_block_size);
        (*ring)->iov_ring[i].iov_len = (*ring)->req.tp_block_size;
    }

    (*ring)->block_idx = 0;
    
    return;
}

void cord_tpacketv3_ring_free(struct cord_tpacketv3_ring **ring)
{
    if (*ring == NULL) 
        return;

    if ((*ring)->map && (*ring)->map != MAP_FAILED)
    {
        munmap((*ring)->map, (*ring)->map_size);
    }

    if ((*ring)->iov_ring)
    {
        free((*ring)->iov_ring);
    }

    if ((*ring)->fd >= 0)
    {
        close((*ring)->fd);
    }

    free(*ring);
    *ring = NULL;

    return;
}
