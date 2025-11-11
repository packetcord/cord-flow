#ifndef CORD_DPDK_FLOW_POINT_H
#define CORD_DPDK_FLOW_POINT_H

#ifdef ENABLE_DPDK_DATAPLANE

#include <flow_point/cord_flow_point.h>

#include <rte_ethdev.h>
#include <rte_mempool.h>

#define CORD_CREATE_DPDK_FLOW_POINT CORD_CREATE_DPDK_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_DPDK_FLOW_POINT CORD_DESTROY_DPDK_FLOW_POINT_ON_HEAP

#define CORD_CREATE_DPDK_FLOW_POINT_ON_HEAP(id, port_id, queue_count, queue_size, mbuf_pool) \
    (CordFlowPoint *) NEW_ON_HEAP(CordDpdkFlowPoint, id, port_id, queue_count, queue_size, mbuf_pool)

#define CORD_CREATE_DPDK_FLOW_POINT_ON_STACK(id, port_id, queue_count, queue_size, mbuf_pool)\
    (CordFlowPoint *) &NEW_ON_STACK(CordDpdkFlowPoint, id, port_id, queue_count, queue_size, mbuf_pool)

#define CORD_DESTROY_DPDK_FLOW_POINT_ON_HEAP(name) \
    do {                                           \
        DESTROY_ON_HEAP(CordDpdkFlowPoint, name);  \
    } while(0)

#define CORD_DESTROY_DPDK_FLOW_POINT_ON_STACK(name)\
    do {                                           \
        DESTROY_ON_STACK(CordDpdkFlowPoint, name); \
    } while(0)

typedef struct CordDpdkFlowPoint
{
    CordFlowPoint base;
    uint16_t port_id;
    uint16_t queue_count;
    uint16_t queue_size;
    struct rte_mempool **mbuf_pool;
    struct rte_eth_stats stats;
    void *params;
} CordDpdkFlowPoint;

void CordDpdkFlowPoint_ctor(CordDpdkFlowPoint * const self,
                            uint8_t id,
                            uint16_t port_id,
                            uint16_t queue_count,
                            uint16_t queue_size,
                            struct rte_mempool **mbuf_pool);

void CordDpdkFlowPoint_dtor(CordDpdkFlowPoint * const self);

#endif // ENABLE_DPDK_DATAPLANE

#endif // CORD_DPDK_FLOW_POINT_H
