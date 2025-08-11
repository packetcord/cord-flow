#ifdef ENABLE_XDP_DATAPLANE

#include <flow_point/cord_xdp_flow_point.h>

static cord_retval_t CordXdpFlowPoint_rx_(CordXdpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    CORD_LOG("[CordXdpFlowPoint] rx()\n");
    //
    // Implement the AF_XDP rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_tx_(CordXdpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    CORD_LOG("[CordXdpFlowPoint] tx()\n");
    //
    // Implement the AF_XDP tx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_fill_(CordXdpFlowPoint const * const self)
{
    CORD_LOG("[CordXdpFlowPoint] fill()\n");
    //
    // Implement the fill logic
    //

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_drain_completion_(CordXdpFlowPoint const * const self)
{
    CORD_LOG("[CordXdpFlowPoint] drain_completion()\n");
    //
    // Implement the drain completion logic
    //

    return CORD_OK;
}

void CordXdpFlowPoint_ctor(CordXdpFlowPoint * const self,
                           uint8_t id,
                           size_t rx_buffer_size,
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
    CORD_LOG("[CordXdpFlowPoint] ctor()\n");
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordXdpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordXdpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
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
    CORD_LOG("[CordXdpFlowPoint] dtor()\n");
    free(self);
}

#endif
