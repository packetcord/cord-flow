#ifndef CORD_MEMORY_H
#define CORD_MEMORY_H

#include <cord_type.h>
#include <stddef.h>

#ifdef ENABLE_DPDK_DATAPLANE
#include <rte_mbuf.h>

struct rte_mempool* cord_pktmbuf_mpool_alloc(const char *name, unsigned int n, unsigned int cache_size);
void cord_pktmbuf_mpool_free(struct rte_mempool **mbuf_pool);

#endif // ENABLE_DPDK_DATAPLANE

// Memory alignment and huge page constants
#define CORD_HUGE_PAGE_2MB       (2 * 1024 * 1024)
#define CORD_HUGE_PAGE_1GB       (1024 * 1024 * 1024)
#define CORD_HUGE_PAGE_SIZE      CORD_HUGE_PAGE_2MB

#define CORD_CACHE_LINE_SIZE     64
#define CORD_PAGE_SIZE           4096

// Memory alignment macros
#define CORD_ALIGN_TO_HUGE_PAGE(x) \
    (((x) + CORD_HUGE_PAGE_SIZE - 1) / CORD_HUGE_PAGE_SIZE * CORD_HUGE_PAGE_SIZE)

#define CORD_ALIGN_TO_CACHE_LINE(x) \
    (((x) + CORD_CACHE_LINE_SIZE - 1) / CORD_CACHE_LINE_SIZE * CORD_CACHE_LINE_SIZE)

#define CORD_ALIGN_TO_PAGE(x) \
    (((x) + CORD_PAGE_SIZE - 1) / CORD_PAGE_SIZE * CORD_PAGE_SIZE)

// Huge page allocation functions (with fallback to calloc/free)
void *cord_alloc_hugepage(size_t size);
void cord_free_hugepage(void *ptr, size_t size);

// Cache-aligned buffer macro
#define CORD_BUFFER(name, size) uint8_t name[size] __attribute__ ((aligned (CORD_CACHE_LINE_SIZE)))

// Raw socket packet buffer configuration
#define CORD_RAW_HEADROOM       128
#define CORD_RAW_BUF_SIZE       2048

// Raw socket packet descriptor
struct cord_raw_pkt_desc
{
    uint8_t *buf_addr;
    uint8_t *data;
    uint16_t data_len;
};

typedef struct cord_raw_pkt_desc cord_raw_pkt_desc_t;

// Initialize packet descriptor
static inline void cord_raw_pkt_init(cord_raw_pkt_desc_t *pkt, void *buf)
{
    pkt->buf_addr = (uint8_t *)buf;
    pkt->data = pkt->buf_addr + CORD_RAW_HEADROOM;
    pkt->data_len = 0;
}

// Prepend data (DPDK-style)
static inline void* cord_raw_pkt_prepend(cord_raw_pkt_desc_t *pkt, uint16_t len)
{
    if ((pkt->data - len) < pkt->buf_addr)
    {
        return NULL;
    }

    pkt->data -= len;
    pkt->data_len += len;
    return pkt->data;
}

// Remove data from start (DPDK-style adj)
static inline void* cord_raw_pkt_adj(cord_raw_pkt_desc_t *pkt, uint16_t len)
{
    if (len > pkt->data_len)
    {
        return NULL;
    }

    pkt->data += len;
    pkt->data_len -= len;
    return pkt->data;
}

// Reset packet descriptor
static inline void cord_raw_pkt_reset(cord_raw_pkt_desc_t *pkt)
{
    pkt->data = pkt->buf_addr + CORD_RAW_HEADROOM;
    pkt->data_len = 0;
}

struct cord_tpacketv3_ring
{
    int fd;
    struct iovec *iov_ring;
    uint8_t *map;
    size_t map_size;
    struct tpacket_req3 req;
    unsigned int block_idx;
};

struct cord_tpacketv3_ring* cord_tpacketv3_ring_alloc(uint32_t block_size, uint32_t frame_size, uint32_t block_num);
void cord_tpacketv3_ring_init(struct cord_tpacketv3_ring **ring);
void cord_tpacketv3_ring_free(struct cord_tpacketv3_ring **ring);

#ifdef ENABLE_XDP_DATAPLANE

#include <xdp/xsk.h>

struct cord_xdp_pkt_desc
{
    void *data;
    uint64_t addr;
    uint32_t len;
    struct cord_xdp_socket_info *src_socket;
};

struct cord_xdp_socket_info
{
    struct xsk_ring_cons rx;
    struct xsk_ring_prod tx;
    struct xsk_ring_prod fq;
    struct xsk_ring_cons cq;
    struct xsk_umem *umem;
    struct xsk_socket *xsk;
    void *umem_area;
    size_t umem_size;
    uint64_t *umem_frames_rx;
    uint64_t *umem_frames_tx;
    uint32_t num_frames;
    uint32_t frame_size;
    uint32_t free_frames_rx;
    uint32_t free_frames_tx;
    int ifindex;
    const char *ifname;
    uint16_t queue_id;
    uint16_t rx_ring_size;
    uint16_t tx_ring_size;
    uint16_t fill_ring_size;
    uint16_t comp_ring_size;
    struct cord_xdp_socket_info *umem_owner;
};

struct cord_xdp_socket_info* cord_xdp_socket_alloc(const char *ifname,
                                                    uint16_t queue_id,
                                                    uint32_t num_frames,
                                                    uint32_t frame_size,
                                                    uint16_t rx_ring_size,
                                                    uint16_t tx_ring_size,
                                                    uint16_t fill_ring_size,
                                                    uint16_t comp_ring_size);

void cord_xdp_socket_init(struct cord_xdp_socket_info **xsk_info);
void cord_xdp_socket_init_shared(struct cord_xdp_socket_info **xsk_info, struct cord_xdp_socket_info **shared_umem_socket);
void cord_xdp_socket_free(struct cord_xdp_socket_info **xsk_info);

uint64_t cord_xdp_alloc_frame_rx(struct cord_xdp_socket_info *xsk_info);
void cord_xdp_free_frame_rx(struct cord_xdp_socket_info *xsk_info, uint64_t frame);
uint64_t cord_xdp_alloc_frame_tx(struct cord_xdp_socket_info *xsk_info);
void cord_xdp_free_frame_tx(struct cord_xdp_socket_info *xsk_info, uint64_t frame);

#endif // ENABLE_XDP_DATAPLANE

#endif // CORD_MEMORY_H
