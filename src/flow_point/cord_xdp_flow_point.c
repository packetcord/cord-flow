#ifdef ENABLE_XDP_DATAPLANE

#include <flow_point/cord_xdp_flow_point.h>
#include <cord_error.h>
#include <string.h>

static cord_retval_t CordXdpFlowPoint_rx_(CordXdpFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_packets)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] rx()\n");
#endif

    (void)queue_id;

    struct cord_xdp_pkt_desc *pkt_descs = (struct cord_xdp_pkt_desc *)buffer;
    struct cord_xdp_socket_info *xsk_info = *(self->xsk_info);
    uint32_t idx_rx = 0, idx_fq = 0;
    uint32_t received, stock_frames;
    int ret;

    received = xsk_ring_cons__peek(&xsk_info->rx, len, &idx_rx);

    if (received > 0)
    {
        for (uint32_t i = 0; i < received; i++)
        {
            const struct xdp_desc *rx_desc = xsk_ring_cons__rx_desc(&xsk_info->rx, idx_rx++);
            pkt_descs[i].addr = rx_desc->addr;
            pkt_descs[i].len = rx_desc->len;
            pkt_descs[i].data = xsk_umem__get_data(xsk_info->umem_area, rx_desc->addr);
            pkt_descs[i].src_socket = xsk_info;
        }

        xsk_ring_cons__release(&xsk_info->rx, received);
    }

    stock_frames = xsk_prod_nb_free(&xsk_info->fq, xsk_info->free_frames_rx);
    if (stock_frames > 0)
    {
        ret = xsk_ring_prod__reserve(&xsk_info->fq, stock_frames, &idx_fq);
        while ((uint32_t)ret != stock_frames)
            ret = xsk_ring_prod__reserve(&xsk_info->fq, stock_frames, &idx_fq);

        for (uint32_t i = 0; i < stock_frames; i++)
            *xsk_ring_prod__fill_addr(&xsk_info->fq, idx_fq++) = cord_xdp_alloc_frame_rx(xsk_info);

        xsk_ring_prod__submit(&xsk_info->fq, stock_frames);
    }

    *rx_packets = received;
    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_tx_(CordXdpFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_packets)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] tx()\n");
#endif

    (void)queue_id;

    struct cord_xdp_pkt_desc *pkt_descs = (struct cord_xdp_pkt_desc *)buffer;
    struct cord_xdp_socket_info *xsk_info = *(self->xsk_info);
    uint32_t idx_tx = 0, idx_cq = 0;
    uint32_t ret;
    uint32_t completed;

    completed = xsk_ring_cons__peek(&xsk_info->cq, xsk_info->comp_ring_size, &idx_cq);
    if (completed > 0)
    {
        for (uint32_t i = 0; i < completed; i++)
            cord_xdp_free_frame_tx(xsk_info, *xsk_ring_cons__comp_addr(&xsk_info->cq, idx_cq++));

        xsk_ring_cons__release(&xsk_info->cq, completed);
    }

    ret = xsk_ring_prod__reserve(&xsk_info->tx, len, &idx_tx);
    if (ret == 0)
    {
        *tx_packets = 0;
        return CORD_OK;
    }

    for (uint32_t i = 0; i < ret; i++)
    {
        uint64_t tx_addr = cord_xdp_alloc_frame_tx(xsk_info);
        if (tx_addr == UINT64_MAX)
        {
            ret = i;
            break;
        }

        memcpy(xsk_umem__get_data(xsk_info->umem_area, tx_addr),
               pkt_descs[i].data,
               pkt_descs[i].len);

        cord_xdp_free_frame_rx(pkt_descs[i].src_socket, pkt_descs[i].addr);

        struct xdp_desc *tx_desc = xsk_ring_prod__tx_desc(&xsk_info->tx, idx_tx++);
        tx_desc->addr = tx_addr;
        tx_desc->len = pkt_descs[i].len;
    }

    xsk_ring_prod__submit(&xsk_info->tx, ret);

    sendto(xsk_socket__fd(xsk_info->xsk), NULL, 0, MSG_DONTWAIT, NULL, 0);

    *tx_packets = ret;
    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_fill_(CordXdpFlowPoint const * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] fill()\n");
#endif

    struct cord_xdp_socket_info *xsk_info = *(self->xsk_info);
    uint32_t idx_fq = 0;
    uint32_t stock_frames;
    int ret;

    stock_frames = xsk_prod_nb_free(&xsk_info->fq, xsk_info->free_frames_rx);
    if (stock_frames == 0)
        return CORD_OK;

    ret = xsk_ring_prod__reserve(&xsk_info->fq, stock_frames, &idx_fq);
    if (ret == 0)
        return CORD_OK;

    for (uint32_t i = 0; i < (uint32_t)ret; i++)
        *xsk_ring_prod__fill_addr(&xsk_info->fq, idx_fq++) = cord_xdp_alloc_frame_rx(xsk_info);

    xsk_ring_prod__submit(&xsk_info->fq, ret);

    return CORD_OK;
}

static cord_retval_t CordXdpFlowPoint_drain_completion_(CordXdpFlowPoint const * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] drain_completion()\n");
#endif

    struct cord_xdp_socket_info *xsk_info = *(self->xsk_info);
    uint32_t idx_cq = 0;
    uint32_t completed;

    completed = xsk_ring_cons__peek(&xsk_info->cq, xsk_info->comp_ring_size, &idx_cq);
    if (completed == 0)
        return CORD_OK;

    for (uint32_t i = 0; i < completed; i++)
        cord_xdp_free_frame_tx(xsk_info, *xsk_ring_cons__comp_addr(&xsk_info->cq, idx_cq++));

    xsk_ring_cons__release(&xsk_info->cq, completed);

    return CORD_OK;
}

void CordXdpFlowPoint_ctor(CordXdpFlowPoint * const self,
                           uint8_t id,
                           struct cord_xdp_socket_info **xsk_info)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] ctor()\n");
#endif

    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_packets))&CordXdpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_packets))&CordXdpFlowPoint_tx_,
        .cleanup = (void (*)(CordFlowPoint const * const))&CordXdpFlowPoint_dtor,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->fill = &CordXdpFlowPoint_fill_;
    self->drain_completion = &CordXdpFlowPoint_drain_completion_;
    self->xsk_info = xsk_info;

    self->base.io_handle = xsk_socket__fd((*xsk_info)->xsk);
}

void CordXdpFlowPoint_dtor(CordXdpFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordXdpFlowPoint] dtor()\n");
#endif

    if (self->xsk_info)
    {
        cord_xdp_socket_free(self->xsk_info);
    }

    free(self);
}

#endif
