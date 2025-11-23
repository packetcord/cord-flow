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

#ifdef ENABLE_XDP_DATAPLANE

#include <xdp/xsk.h>
#include <linux/if_link.h>
#include <net/if.h>
#include <fcntl.h>

struct cord_xdp_socket_info* cord_xdp_socket_alloc(const char *ifname,
                                                    uint16_t queue_id,
                                                    uint32_t num_frames,
                                                    uint32_t frame_size,
                                                    uint16_t rx_ring_size,
                                                    uint16_t tx_ring_size,
                                                    uint16_t fill_ring_size,
                                                    uint16_t comp_ring_size)
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

void cord_xdp_socket_init(struct cord_xdp_socket_info **xsk_info)
{
    cord_xdp_socket_init_shared(xsk_info, NULL);
}

void cord_xdp_socket_init_shared(struct cord_xdp_socket_info **xsk_info, struct cord_xdp_socket_info **shared_umem_socket)
{
    struct xsk_umem_config umem_cfg = {
        .fill_size = (*xsk_info)->fill_ring_size,
        .comp_size = (*xsk_info)->comp_ring_size,
        .frame_size = (*xsk_info)->frame_size,
        .frame_headroom = XSK_UMEM__DEFAULT_FRAME_HEADROOM,
        .flags = 0
    };

    struct xsk_socket_config xsk_cfg = {
        .rx_size = (*xsk_info)->rx_ring_size,
        .tx_size = (*xsk_info)->tx_ring_size,
        .libbpf_flags = 0,
        .xdp_flags = XDP_FLAGS_UPDATE_IF_NOEXIST,
        .bind_flags = XDP_USE_NEED_WAKEUP | XDP_ZEROCOPY
    };

    int ret;
    uint32_t idx;

    if (shared_umem_socket && *shared_umem_socket)
    {
        (*xsk_info)->umem_area = (*shared_umem_socket)->umem_area;
        (*xsk_info)->umem_size = (*shared_umem_socket)->umem_size;
        (*xsk_info)->umem = (*shared_umem_socket)->umem;
        (*xsk_info)->umem_frames_rx = (*shared_umem_socket)->umem_frames_rx;
        (*xsk_info)->umem_frames_tx = (*shared_umem_socket)->umem_frames_tx;
        (*xsk_info)->free_frames_rx = (*shared_umem_socket)->free_frames_rx;
        (*xsk_info)->free_frames_tx = (*shared_umem_socket)->free_frames_tx;
        (*xsk_info)->umem_owner = *shared_umem_socket;

        memset(&(*xsk_info)->fq, 0, sizeof((*xsk_info)->fq));
        memset(&(*xsk_info)->cq, 0, sizeof((*xsk_info)->cq));
    }
    else
    {
        ret = posix_memalign(&(*xsk_info)->umem_area, getpagesize(), (*xsk_info)->umem_size);
        if (ret)
        {
            CORD_ERROR("[cord_xdp_socket_init] posix_memalign");
            return;
        }

        ret = xsk_umem__create(&(*xsk_info)->umem, (*xsk_info)->umem_area, (*xsk_info)->umem_size,
                              &(*xsk_info)->fq, &(*xsk_info)->cq, &umem_cfg);
        if (ret)
        {
            CORD_ERROR("[cord_xdp_socket_init] xsk_umem__create");
            free((*xsk_info)->umem_area);
            return;
        }

        (*xsk_info)->umem_owner = NULL;
    }

    if (shared_umem_socket && *shared_umem_socket)
    {
        xsk_cfg.bind_flags |= XDP_SHARED_UMEM;
        ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
        if (ret)
        {
            xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_COPY | XDP_SHARED_UMEM;
            ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                    (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
            if (ret)
            {
                xsk_cfg.xdp_flags = XDP_FLAGS_UPDATE_IF_NOEXIST | XDP_FLAGS_SKB_MODE;
                xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_COPY | XDP_SHARED_UMEM;
                ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                        (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
                if (ret)
                {
                    CORD_ERROR("[cord_xdp_socket_init] xsk_socket__create");
                    return;
                }
            }
        }
    }
    else
    {
        ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
        if (ret)
        {
            xsk_cfg.bind_flags = XDP_USE_NEED_WAKEUP | XDP_COPY;
            ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                    (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
            if (ret)
            {
                xsk_cfg.xdp_flags = XDP_FLAGS_UPDATE_IF_NOEXIST | XDP_FLAGS_SKB_MODE;
                ret = xsk_socket__create(&(*xsk_info)->xsk, (*xsk_info)->ifname, (*xsk_info)->queue_id,
                                        (*xsk_info)->umem, &(*xsk_info)->rx, &(*xsk_info)->tx, &xsk_cfg);
                if (ret)
                {
                    CORD_ERROR("[cord_xdp_socket_init] xsk_socket__create");
                    xsk_umem__delete((*xsk_info)->umem);
                    free((*xsk_info)->umem_area);
                    return;
                }
            }
        }
    }

    int sockfd = xsk_socket__fd((*xsk_info)->xsk);
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags != -1)
    {
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }

    if (!shared_umem_socket || !*shared_umem_socket)
    {
        uint32_t rx_frames = (*xsk_info)->num_frames / 2;
        uint32_t tx_frames = (*xsk_info)->num_frames - rx_frames;

        (*xsk_info)->umem_frames_rx = malloc(rx_frames * sizeof(uint64_t));
        (*xsk_info)->umem_frames_tx = malloc(tx_frames * sizeof(uint64_t));

        if (!(*xsk_info)->umem_frames_rx || !(*xsk_info)->umem_frames_tx)
        {
            CORD_ERROR("[cord_xdp_socket_init] malloc");
            free((*xsk_info)->umem_frames_rx);
            free((*xsk_info)->umem_frames_tx);
            xsk_socket__delete((*xsk_info)->xsk);
            xsk_umem__delete((*xsk_info)->umem);
            free((*xsk_info)->umem_area);
            return;
        }

        for (uint32_t i = 0; i < rx_frames; i++)
            (*xsk_info)->umem_frames_rx[i] = i * (*xsk_info)->frame_size;

        for (uint32_t i = 0; i < tx_frames; i++)
            (*xsk_info)->umem_frames_tx[i] = (rx_frames + i) * (*xsk_info)->frame_size;

        (*xsk_info)->free_frames_rx = rx_frames;
        (*xsk_info)->free_frames_tx = tx_frames;

        ret = xsk_ring_prod__reserve(&(*xsk_info)->fq, (*xsk_info)->fill_ring_size, &idx);
        if (ret != (*xsk_info)->fill_ring_size)
        {
            CORD_ERROR("[cord_xdp_socket_init] xsk_ring_prod__reserve(fq)");
            free((*xsk_info)->umem_frames_rx);
            free((*xsk_info)->umem_frames_tx);
            xsk_socket__delete((*xsk_info)->xsk);
            xsk_umem__delete((*xsk_info)->umem);
            free((*xsk_info)->umem_area);
            return;
        }

        for (uint16_t i = 0; i < (*xsk_info)->fill_ring_size; i++)
            *xsk_ring_prod__fill_addr(&(*xsk_info)->fq, idx++) = cord_xdp_alloc_frame_rx(*xsk_info);

        xsk_ring_prod__submit(&(*xsk_info)->fq, (*xsk_info)->fill_ring_size);
    }

    return;
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
        free((*xsk_info)->umem_area);

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
