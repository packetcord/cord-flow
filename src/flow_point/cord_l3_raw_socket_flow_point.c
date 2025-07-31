#include <flow_point/cord_l3_raw_socket_flow_point.h>

static cord_retval_t CordL3RawSocketFlowPoint_rx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL3RawSocketFlowPoint *l2_raw_fp = (const CordL3RawSocketFlowPoint *)self;
    
    //
    // Implement the AF_PACKET/AF_INET rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL3RawSocketFlowPoint_tx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL3RawSocketFlowPoint *l2_raw_fp = (const CordL3RawSocketFlowPoint *)self;

    //
    // Implement the AF_PACKET/AF_INET tx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL3RawSocketFlowPoint_attach_filter_(CordL3RawSocketFlowPoint const * const self, void *filter)
{
    //
    // Implement the attach_filter logic
    //

    return CORD_OK;
}

void CordL3RawSocketFlowPoint_ctor(CordL3RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   size_t rx_buffer_size,
                                   int fd,
                                   int ifindex,
                                   const char *anchor_iface_name,
                                   int fanout_id,
                                   bool use_tpacket_v3,
                                   void *ring,
                                   void *attached_filter,
                                   void *params)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL3RawSocketFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL3RawSocketFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->attach_filter = &CordL3RawSocketFlowPoint_attach_filter_;
    self->fd = fd;
    self->ifindex = ifindex;
    self->anchor_iface_name = anchor_iface_name;
    self->fanout_id = fanout_id;
    self->use_tpacket_v3 = use_tpacket_v3;
    self->ring = ring;
    self->attached_filter = attached_filter;
    self->params = params;
}

void CordL3RawSocketFlowPoint_dtor(CordL3RawSocketFlowPoint * const self)
{
    free(self);
}
