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

struct cord_tpacketv3_ring_t
{
    int fd;
    struct iovec *ring;
    uint8_t *map;
    size_t map_size;
    struct tpacket_req3 req;
    unsigned int block_idx;
    char iface[IFNAMSIZ];

    struct tpacket_block_desc *current_block;
    struct tpacket3_hdr *current_packet;
    unsigned int packets_remaining;
};

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
            CORD_ERROR("Failed to munmap huge page memory");
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

cord_tpacketv3_ring_t* cord_tpacketv3_ring_alloc(const char *iface_name, uint32_t block_size, uint32_t frame_size, uint32_t block_num, bool qdisc_bypass)
{
    cord_tpacketv3_ring_t *ring = cord_cache_aligned_malloc(sizeof(cord_tpacketv3_ring_t));
    if (!ring)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: cord_cache_aligned_malloc(ring)");
        return NULL;
    }

    memset(ring, 0, sizeof(cord_tpacketv3_ring_t));
    strncpy(ring->iface, iface_name, IFNAMSIZ - 1);
    ring->block_idx = 0;

    ring->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ring->fd < 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: socket()");
        cord_cache_aligned_free(ring);
        return NULL;
    }

    int version = TPACKET_V3;
    if (setsockopt(ring->fd, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) < 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: setsockopt(PACKET_VERSION)");
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    if (qdisc_bypass)
    {
        int val = 1;
        if (setsockopt(ring->fd, SOL_PACKET, PACKET_QDISC_BYPASS, &val, sizeof(val)) < 0)
        {
            CORD_ERROR("cord_tpacketv3_ring_alloc: setsockopt(PACKET_QDISC_BYPASS)");
        }
    }

    memset(&ring->req, 0, sizeof(ring->req));
    ring->req.tp_block_size = block_size;
    ring->req.tp_frame_size = frame_size;
    ring->req.tp_block_nr = block_num;
    ring->req.tp_frame_nr = (block_size * block_num) / frame_size;
    ring->req.tp_retire_blk_tov = 1;
    ring->req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;

    if (setsockopt(ring->fd, SOL_PACKET, PACKET_RX_RING, &ring->req, sizeof(ring->req)) < 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: setsockopt(PACKET_RX_RING)");
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    ring->map_size = ring->req.tp_block_size * ring->req.tp_block_nr;
    ring->map = mmap(NULL, ring->map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_POPULATE, ring->fd, 0);
    if (ring->map == MAP_FAILED)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: mmap()");
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    ring->ring = cord_cache_aligned_malloc(ring->req.tp_block_nr * sizeof(struct iovec));
    if (!ring->ring)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: cord_cache_aligned_malloc(iovec)");
        munmap(ring->map, ring->map_size);
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    for (unsigned int i = 0; i < ring->req.tp_block_nr; i++)
    {
        ring->ring[i].iov_base = ring->map + (i * ring->req.tp_block_size);
        ring->ring[i].iov_len = ring->req.tp_block_size;
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = if_nametoindex(iface_name);

    if (sll.sll_ifindex == 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: if_nametoindex()");
        cord_cache_aligned_free(ring->ring);
        munmap(ring->map, ring->map_size);
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    if (bind(ring->fd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: bind()");
        cord_cache_aligned_free(ring->ring);
        munmap(ring->map, ring->map_size);
        close(ring->fd);
        cord_cache_aligned_free(ring);
        return NULL;
    }

    int enable = 1;
    if (setsockopt(ring->fd, SOL_PACKET, PACKET_IGNORE_OUTGOING, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("cord_tpacketv3_ring_alloc: setsockopt(PACKET_IGNORE_OUTGOING)");
    }

    ring->current_block = NULL;
    ring->current_packet = NULL;
    ring->packets_remaining = 0;

    return ring;
}

void cord_tpacketv3_ring_free(cord_tpacketv3_ring_t *ring)
{
    if (!ring)
        return;

    if (ring->ring)
    {
        cord_cache_aligned_free(ring->ring);
        ring->ring = NULL;
    }

    if (ring->map && ring->map != MAP_FAILED)
    {
        munmap(ring->map, ring->map_size);
        ring->map = NULL;
    }

    if (ring->fd >= 0)
    {
        close(ring->fd);
        ring->fd = -1;
    }

    cord_cache_aligned_free(ring);
}