#ifdef ENABLE_DPDK_DATAPLANE

#include <flow_point/cord_dpdk_flow_point.h>

static cord_retval_t CordDpdkFlowPoint_rx_(CordDpdkFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{    
    //
    // Implement the DPDK rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordDpdkFlowPoint_tx_(CordDpdkFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    //
    // Implement the DPDK tx() logic
    //

    return CORD_OK;
}

void CordDpdkFlowPoint_ctor(CordDpdkFlowPoint * const self,
    uint8_t id,
    size_t rx_buffer_size,
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
    void *params)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordDpdkFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordDpdkFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->port_id = port_id;
    self->rx_queue_id = rx_queue_id;
    self->tx_queue_id = tx_queue_id;
    self->nb_rx_queues = nb_rx_queues;
    self->nb_tx_queues = nb_tx_queues;
    self->rx_burst_size = rx_burst_size;
    self->tx_burst_size = tx_burst_size;
    self->mbuf_pool = mbuf_pool;
    self->flags = flags;
    self->stats = stats;
    self->params = params;
}

void CordDpdkFlowPoint_dtor(CordDpdkFlowPoint * const self)
{
    free(self);
}

#endif
