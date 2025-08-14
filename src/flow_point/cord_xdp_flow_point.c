#ifdef ENABLE_XDP_DATAPLANE

#include <flow_point/cord_xdp_flow_point.h>

static cord_retval_t CordXdpFlowPoint_rx_(CordXdpFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] rx()\n");
#endif
    //
    // Implement the AF_XDP rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_tx_(CordXdpFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] tx()\n");
#endif
    //
    // Implement the AF_XDP tx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_fill_(CordXdpFlowPoint const * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] fill()\n");
#endif
    //
    // Implement the fill logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_drain_completion_(CordXdpFlowPoint const * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] drain_completion()\n");
#endif
    //
    // Implement the drain completion logic
    //

    return CORD_OK;
}

void CordXdpFlowPoint_ctor(CordXdpFlowPoint * const self,
                           uint8_t id,
                           int ifindex,
                           const char *anchor_iface_name,
                           uint16_t queue_id,
                           struct xsk_socket *xsk,
                           struct xsk_umem *umem,
                           void *umem_area,
                           size_t umem_size,
                           uint16_t rx_ring_size,
                           uint16_t tx_ring_size,
                           uint16_t fill_ring_size,
                           uint16_t comp_ring_size,
                           uint16_t frame_size,
                           void *params)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordXdpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordXdpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->fill = &CordXdpFlowPoint_fill_;
    self->drain_completion = &CordXdpFlowPoint_drain_completion_;
    self->ifindex = ifindex;
    self->anchor_iface_name = anchor_iface_name;
    self->queue_id = queue_id;
    self->xsk = xsk;
    self->umem = umem;
    self->umem_area = umem_area;
    self->umem_size = umem_size;
    self->rx_ring_size = rx_ring_size;
    self->tx_ring_size = tx_ring_size;
    self->fill_ring_size = fill_ring_size;
    self->comp_ring_size = comp_ring_size;
    self->frame_size = frame_size;
    self->params;
}

void CordXdpFlowPoint_dtor(CordXdpFlowPoint * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXdpFlowPoint] dtor()\n");
#endif
    free(self);
}

#endif
