#ifndef CORD_DPDK_FLOW_POINT_H
#define CORD_DPDK_FLOW_POINT_H

#ifdef ENABLE_DPDK_DATAPLANE

#include <flow_point/cord_flow_point.h>

#include <rte_ethdev.h>
#include <rte_mempool.h>

#define CORD_CREATE_DPDK_FLOW_POINT CORD_CREATE_DPDK_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_DPDK_FLOW_POINT CORD_DESTROY_DPDK_FLOW_POINT_ON_HEAP

#define CORD_CREATE_DPDK_FLOW_POINT_ON_HEAP(id, ...) \
    (CordFlowPoint *) NEW_ON_HEAP(CordDpdkFlowPoint, id, ...)

#define CORD_CREATE_DPDK_FLOW_POINT_ON_STACK(id, ...)\
    (CordFlowPoint *) &NEW_ON_STACK(CordDpdkFlowPoint, id, ...)

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
    uint16_t rx_queue_id;
    uint16_t tx_queue_id;
    uint16_t nb_rx_queues;
    uint16_t nb_tx_queues;
    uint16_t rx_burst_size;
    uint16_t tx_burst_size;
    struct rte_mempool *mbuf_pool;
    uint32_t flags;
    struct rte_eth_stats stats;
    void *params;
} CordDpdkFlowPoint;

void CordDpdkFlowPoint_ctor(CordDpdkFlowPoint * const self,
                            uint8_t id,
                            uint16_t port_id,
                            uint16_t rx_queue_id,
                            uint16_t tx_queue_id,
                            uint16_t nb_rx_queues,
                            uint16_t nb_tx_queues,
                            uint16_t rx_burst_size,
                            uint16_t tx_burst_size,
                            struct rte_mempool *mbuf_pool,
                            uint32_t flags,
                            struct rte_eth_stats stats,
                            void *params);

void CordDpdkFlowPoint_dtor(CordDpdkFlowPoint * const self);

#endif // ENABLE_DPDK_DATAPLANE

#endif // CORD_DPDK_FLOW_POINT_H
