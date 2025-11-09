#ifdef ENABLE_DPDK_DATAPLANE

#include <flow_point/cord_dpdk_flow_point.h>
#include <rte_eal.h>
#include <rte_errno.h>
#include <string.h>
#include <stdbool.h>

static cord_retval_t CordDpdkFlowPoint_rx_(CordDpdkFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] rx()\n");
#endif
    //
    // Implement the DPDK rx() logic
    //
    return CORD_OK;
}

static cord_retval_t CordDpdkFlowPoint_tx_(CordDpdkFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] tx()\n");
#endif
    //
    // Implement the DPDK tx() logic
    //

    return CORD_OK;
}

void CordDpdkFlowPoint_ctor(CordDpdkFlowPoint * const self,
    uint8_t id,
    uint16_t port_id,
    uint16_t queue_count,
    uint16_t queue_size,
    uint8_t cpu_mask,
    struct rte_mempool *mbuf_pool,
    struct rte_eth_stats stats,
    void *params)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] ctor()\n");
#endif

    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordDpdkFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordDpdkFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->port_id = port_id;
    self->queue_count = queue_count;
    self->queue_size = queue_size;
    self->cpu_mask = cpu_mask;
    self->mbuf_pool = mbuf_pool;
    self->stats = stats;
    self->params = params;
}

void CordDpdkFlowPoint_dtor(CordDpdkFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] dtor()\n");
#endif
    free(self);
}

#endif
