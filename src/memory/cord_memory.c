#include <memory/cord_memory.h>
#include <cord_error.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>

// Huge page allocation with fallback to calloc
void *cord_alloc_hugepage(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    // Align size to huge page boundary
    size_t aligned_size = CORD_ALIGN_TO_HUGE_PAGE(size);

    // Try to allocate using huge pages
    void *ptr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_POPULATE, -1, 0);

    if (ptr != MAP_FAILED)
    {
        // Success with huge pages
        return ptr;
    }

    // Huge page allocation failed, fall back to calloc
    ptr = calloc(1, size);
    if (!ptr)
    {
        CORD_ERROR("[cord_alloc_hugepage] calloc fallback failed");
        return NULL;
    }

    return ptr;
}

void cord_free_hugepage(void *ptr, size_t size)
{
    if (!ptr || size == 0)
    {
        return;
    }

    // Try to determine if this was allocated with mmap or calloc
    // We'll attempt munmap first - if it fails, we'll use free
    size_t aligned_size = CORD_ALIGN_TO_HUGE_PAGE(size);

    if (munmap(ptr, aligned_size) == -1)
    {
        // munmap failed, this was likely allocated with calloc
        free(ptr);
    }
    // If munmap succeeded, we're done (it was huge page allocation)
}

#ifdef ENABLE_DPDK_DATAPLANE

struct rte_mempool *cord_pktmbuf_mpool_alloc(const char *name, unsigned int n, unsigned int cache_size)
{
    struct rte_mempool *mbuf_pool =
        rte_pktmbuf_pool_create(name, n, cache_size, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

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

struct cord_tpacketv3_ring *cord_tpacketv3_ring_alloc(uint32_t block_size, uint32_t frame_size, uint32_t block_num)
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

    (*ring)->map =
        mmap(NULL, (*ring)->map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_POPULATE, (*ring)->fd, 0);

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

#ifdef ENABLE_XDP_DATAPLANE

#include <xdp/xsk.h>
#include <linux/if_link.h>
#include <net/if.h>
#include <fcntl.h>

struct cord_xdp_socket_info *cord_xdp_socket_alloc(const char *ifname, uint16_t queue_id, uint32_t num_frames,
                                                   uint32_t frame_size, uint16_t rx_ring_size, uint16_t tx_ring_size,
                                                   uint16_t fill_ring_size, uint16_t comp_ring_size)
{
    struct cord_xdp_socket_info *xsk_info = calloc(1, sizeof(struct cord_xdp_socket_info));
    if (xsk_info == NULL)
    {
        CORD_ERROR("[cord_xdp_socket_alloc] calloc");
        return NULL;
    }

    xsk_info->ifname = ifname;
    xsk_info->queue_id = queue_id;
    xsk_info->num_frames = num_frames;
    xsk_info->frame_size = frame_size;
    xsk_info->rx_ring_size = rx_ring_size;
    xsk_info->tx_ring_size = tx_ring_size;
    xsk_info->fill_ring_size = fill_ring_size;
    xsk_info->comp_ring_size = comp_ring_size;
    xsk_info->umem_size = num_frames * frame_size;
    xsk_info->ifindex = if_nametoindex(ifname);

    if (xsk_info->ifindex == 0)
    {
        CORD_ERROR("[cord_xdp_socket_alloc] if_nametoindex");
        free(xsk_info);
        return NULL;
    }

    return xsk_info;
}

cord_retval_t cord_xdp_socket_init(struct cord_xdp_socket_info **xsk_info, 
                                   bool load_default_prog)
{
    return cord_xdp_socket_init_shared(xsk_info, NULL, load_default_prog);
}

cord_retval_t cord_xdp_socket_init_shared(struct cord_xdp_socket_info **xsk_info,
                                          struct cord_xdp_socket_info **shared_umem_socket,
                                          bool load_default_prog)
{
    if (!xsk_info || !*xsk_info)
    {
        CORD_ERROR("[cord_xdp_socket_init] Invalid xsk_info pointer");
        return CORD_ERR;
    }

    struct cord_xdp_socket_info *info = *xsk_info;
    struct cord_xdp_socket_info *shared_info = (shared_umem_socket && *shared_umem_socket) ? *shared_umem_socket : NULL;

    struct xsk_umem_config umem_cfg = {
        .fill_size = info->fill_ring_size,
        .comp_size = info->comp_ring_size,
        .frame_size = info->frame_size,
        .frame_headroom = XSK_UMEM__DEFAULT_FRAME_HEADROOM,
        .flags = 0
    };

    struct xsk_socket_config xsk_cfg = {
        .rx_size = info->rx_ring_size,
        .tx_size = info->tx_ring_size,
        .libxdp_flags = load_default_prog ? 0 : XSK_LIBXDP_FLAGS__INHIBIT_PROG_LOAD,
        .xdp_flags = load_default_prog ? XDP_FLAGS_UPDATE_IF_NOEXIST : 0,
        .bind_flags = XDP_USE_NEED_WAKEUP
    };

    int ret;
    uint32_t idx;

    // UMEM Handling
    if (shared_info)
    {
        // Secondary Socket: Copy primary socket's references
        info->umem_area = shared_info->umem_area;
        info->umem_size = shared_info->umem_size;
        info->umem = shared_info->umem;
        info->umem_owner = shared_info;
        info->is_ebpf_owner = false;

        info->fq = shared_info->fq;
        info->cq = shared_info->cq;
    }
    else
    {
        // Primary Socket: Allocate memory region and create UMEM
        info->umem_area = mmap(NULL, info->umem_size,
                               PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS,
                               -1, 0);

        if (info->umem_area == MAP_FAILED)
        {
            CORD_ERROR("[cord_xdp_socket_init] mmap failed");
            return CORD_ERR;
        }

        ret = xsk_umem__create(&info->umem, info->umem_area, info->umem_size,
                               &info->fq, &info->cq, &umem_cfg);
        if (ret)
        {
            CORD_ERROR("[cord_xdp_socket_init] xsk_umem__create failed");
            munmap(info->umem_area, info->umem_size);
            return CORD_ERR;
        }

        info->umem_owner = NULL;
        info->is_ebpf_owner = true;
    }

    // Socket Binding & Creation
    if (shared_info)
    {
        CORD_LOG("[cord_xdp_socket_init] attempting zero-copy mode with shared UMEM...\n");

        // Pass the primary socket's FD via bind flags or share context
        xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_ZEROCOPY | XDP_SHARED_UMEM;

        ret = xsk_socket__create_shared(&info->xsk, info->ifname, info->queue_id,
                                        info->umem, &info->rx, &info->tx,
                                        &info->fq, &info->cq, &xsk_cfg);
        if (ret)
        {
            CORD_LOG("[cord_xdp_socket_init] fallback to standard share initialization...\n");
            ret = xsk_socket__create(&info->xsk, info->ifname, info->queue_id,
                                     info->umem, &info->rx, &info->tx, &xsk_cfg);
            if (ret)
            {
                CORD_ERROR("[cord_xdp_socket_init] xsk_socket__create (shared) failed");
                return CORD_ERR;
            }
        }
    }
    else
    {
        CORD_LOG("[cord_xdp_socket_init] attempting zero-copy mode with dedicated UMEM...\n");
        xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_ZEROCOPY;

        ret = xsk_socket__create(&info->xsk, info->ifname, info->queue_id,
                                 info->umem, &info->rx, &info->tx, &xsk_cfg);
        if (ret)
        {
            CORD_LOG("[cord_xdp_socket_init] fallback to copy mode\n");
            xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_COPY;

            ret = xsk_socket__create(&info->xsk, info->ifname, info->queue_id,
                                     info->umem, &info->rx, &info->tx, &xsk_cfg);
            if (ret)
            {
                CORD_ERROR("[cord_xdp_socket_init] xsk_socket__create failed");
                xsk_umem__delete(info->umem);
                munmap(info->umem_area, info->umem_size);
                return CORD_ERR;
            }
        }
    }

    // Guard against NULL socket FD before using fcntl
    if (!info->xsk)
    {
        CORD_ERROR("[cord_xdp_socket_init] XSK socket structure is NULL");
        return CORD_ERR;
    }

    int sockfd = xsk_socket__fd(info->xsk);
    if (sockfd >= 0)
    {
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags != -1)
        {
            fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    // Frame Allocation & Partitioning
    uint32_t frames_per_socket = info->num_frames / 2;
    uint32_t rx_frames = frames_per_socket / 2;
    uint32_t tx_frames = frames_per_socket - rx_frames;

    info->umem_frames_rx = malloc(rx_frames * sizeof(uint64_t));
    info->umem_frames_tx = malloc(tx_frames * sizeof(uint64_t));

    if (!info->umem_frames_rx || !info->umem_frames_tx)
    {
        CORD_ERROR("[cord_xdp_socket_init] Memory allocation failed for frame array");
        free(info->umem_frames_rx);
        free(info->umem_frames_tx);
        xsk_socket__delete(info->xsk);
        if (!shared_info)
        {
            xsk_umem__delete(info->umem);
            munmap(info->umem_area, info->umem_size);
        }
        return CORD_ERR;
    }

    uint32_t base_frame_idx = shared_info ? frames_per_socket : 0;

    for (uint32_t i = 0; i < rx_frames; i++)
        info->umem_frames_rx[i] = (base_frame_idx + i) * info->frame_size;

    for (uint32_t i = 0; i < tx_frames; i++)
        info->umem_frames_tx[i] = (base_frame_idx + rx_frames + i) * info->frame_size;

    info->free_frames_rx = rx_frames;
    info->free_frames_tx = tx_frames;

    // Populate Fill Queue for both Primary and Secondary sockets
    ret = xsk_ring_prod__reserve(&info->fq, rx_frames < info->fill_ring_size ? rx_frames : info->fill_ring_size, &idx);
    if (ret > 0)
    {
        for (int i = 0; i < ret; i++)
            *xsk_ring_prod__fill_addr(&info->fq, idx++) = cord_xdp_alloc_frame_rx(info);

        xsk_ring_prod__submit(&info->fq, ret);
    }

    return CORD_OK;
}

void cord_xdp_socket_free(struct cord_xdp_socket_info **xsk_info)
{
    if (*xsk_info == NULL)
        return;

    if ((*xsk_info)->xsk)
        xsk_socket__delete((*xsk_info)->xsk);

    if ((*xsk_info)->umem)
        xsk_umem__delete((*xsk_info)->umem);

    if ((*xsk_info)->umem_frames_rx)
        free((*xsk_info)->umem_frames_rx);

    if ((*xsk_info)->umem_frames_tx)
        free((*xsk_info)->umem_frames_tx);

    if ((*xsk_info)->umem_area)
        munmap((*xsk_info)->umem_area, (*xsk_info)->umem_size);

    free(*xsk_info);
    *xsk_info = NULL;

    return;
}

uint64_t cord_xdp_alloc_frame_rx(struct cord_xdp_socket_info *xsk_info)
{
    if (xsk_info->free_frames_rx == 0)
        return UINT64_MAX;

    uint64_t frame = xsk_info->umem_frames_rx[--xsk_info->free_frames_rx];
    xsk_info->umem_frames_rx[xsk_info->free_frames_rx] = UINT64_MAX;
    return frame;
}

void cord_xdp_free_frame_rx(struct cord_xdp_socket_info *xsk_info, uint64_t frame)
{
    xsk_info->umem_frames_rx[xsk_info->free_frames_rx++] = frame;
}

uint64_t cord_xdp_alloc_frame_tx(struct cord_xdp_socket_info *xsk_info)
{
    if (xsk_info->free_frames_tx == 0)
        return UINT64_MAX;

    uint64_t frame = xsk_info->umem_frames_tx[--xsk_info->free_frames_tx];
    xsk_info->umem_frames_tx[xsk_info->free_frames_tx] = UINT64_MAX;
    return frame;
}

void cord_xdp_free_frame_tx(struct cord_xdp_socket_info *xsk_info, uint64_t frame)
{
    xsk_info->umem_frames_tx[xsk_info->free_frames_tx++] = frame;
}

#endif // ENABLE_XDP_DATAPLANE
