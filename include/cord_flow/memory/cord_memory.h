#ifndef CORD_MEMORY_H
#define CORD_MEMORY_H

#include <cord_type.h>
#include <stddef.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/uio.h>

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

// Cache-aligned buffer macro
#define CORD_BUFFER(name, size) uint8_t name[size] __attribute__ ((aligned (CORD_CACHE_LINE_SIZE)))

void* cord_huge_pages_malloc(size_t size);
void cord_huge_pages_free(void* ptr);
void* cord_cache_aligned_malloc(size_t size);
void cord_cache_aligned_free(void* ptr);
size_t cord_huge_pages_get_size(const void* ptr);
bool cord_huge_pages_available(void);

typedef struct
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
} cord_tpacketv3_ring_t;

cord_tpacketv3_ring_t* cord_tpacketv3_ring_alloc(const char *iface_name, uint32_t block_size, uint32_t frame_size, uint32_t block_num, bool qdisc_bypass);
void cord_tpacketv3_ring_free(cord_tpacketv3_ring_t *ring);

#endif // CORD_MEMORY_H
