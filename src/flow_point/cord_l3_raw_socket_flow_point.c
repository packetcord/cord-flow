#include <flow_point/cord_l3_raw_socket_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL3RawSocketFlowPoint_rx_(CordL3RawSocketFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordL3RawSocketFlowPoint] rx()\n");
#endif
    //
    // Implement the AF_PACKET/AF_INET rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL3RawSocketFlowPoint_tx_(CordL3RawSocketFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordL3RawSocketFlowPoint] tx()\n");
#endif
    //
    // Implement the AF_PACKET/AF_INET tx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL3RawSocketFlowPoint_attach_filter_(CordL3RawSocketFlowPoint const * const self, void *filter)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordL3RawSocketFlowPoint] attach_filter()\n");
#endif
    //
    // Implement the attach_filter logic
    //

    return CORD_OK;
}

void CordL3RawSocketFlowPoint_ctor(CordL3RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   const char *anchor_iface_name)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordL3RawSocketFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordL3RawSocketFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordL3RawSocketFlowPoint_tx_,
    };

    static const CordL3RawSocketFlowPointVtbl vtbl_deriv = {
        .attach_filter = (cord_retval_t (*)(CordFlowPoint const * const self, void *filter))&CordL3RawSocketFlowPoint_attach_filter_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->vptr = &vtbl_deriv;
    self->anchor_iface_name = anchor_iface_name;

    self->base.io_handle = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL3RawSocketFlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_BINDTODEVICE, self->anchor_iface_name, strlen(self->anchor_iface_name)) < 0)
    {
        CORD_ERROR("[CordL3RawSocketFlowPoint] setsockopt(SO_BINDTODEVICE)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    struct ifreq anchor_iface_req;
    memset(&anchor_iface_req, 0, sizeof(struct ifreq));
    strncpy(anchor_iface_req.ifr_name, self->anchor_iface_name, IFNAMSIZ);
    if (ioctl(self->base.io_handle, SIOCGIFINDEX, &anchor_iface_req) < 0)
    {
        CORD_ERROR("[CordL3RawSocketFlowPoint] ioctl(SIOCGIFINDEX)");
    }
    self->ifindex = anchor_iface_req.ifr_ifindex;

    struct sockaddr_ll anchor_bind_addr;
    memset(&anchor_bind_addr, 0, sizeof(struct sockaddr_ll));
    anchor_bind_addr.sll_family = AF_PACKET;
    anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    anchor_bind_addr.sll_ifindex = anchor_iface_req.ifr_ifindex;
    if (bind(self->base.io_handle, (struct sockaddr *)&anchor_bind_addr, sizeof(struct sockaddr_ll)) < 0)
    {
        CORD_ERROR("[CordL3RawSocketFlowPoint] bind()");
    }

    int enable = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_IGNORE_OUTGOING, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("[CordL3RawSocketFlowPoint] setsockopt(PACKET_IGNORE_OUTGOING)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK);
}

void CordL3RawSocketFlowPoint_dtor(CordL3RawSocketFlowPoint * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordL3RawSocketFlowPoint] dtor()\n");
#endif
    close(self->base.io_handle);
    free(self);
}
