#include <flow_point/cord_l3_raw_socket_flow_point.h>
#include <cord_error.h>

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
    self->ifindex = ifindex;
    self->anchor_iface_name = anchor_iface_name;
    self->fanout_id = fanout_id;
    self->use_tpacket_v3 = use_tpacket_v3;
    self->ring = ring;
    self->attached_filter = attached_filter;
    self->params = params;

    self->fd = fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
    if (self->fd < 0)
    {
        CORD_ERROR("CordL3RawSocketFlowPoint: socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    if (setsockopt(self->fd, SOL_SOCKET, SO_BINDTODEVICE, self->anchor_iface_name, strlen(self->anchor_iface_name)) < 0)
    {
        CORD_ERROR("CordL3RawSocketFlowPoint: setsockopt(SO_BINDTODEVICE)");
        CORD_CLOSE(self->fd);
        CORD_EXIT(EXIT_FAILURE);
    }

    struct ifreq anchor_iface_req;
    memset(&anchor_iface_req, 0, sizeof(struct ifreq));
    strncpy(anchor_iface_req.ifr_name, self->anchor_iface_name, IFNAMSIZ);
    if (ioctl(self->fd, SIOCGIFINDEX, &anchor_iface_req) < 0)
    {
        CORD_ERROR("CordL3RawSocketFlowPoint: ioctl(SIOCGIFINDEX)");
    }
    self->ifindex = anchor_iface_req.ifr_ifindex;

    struct sockaddr_ll anchor_bind_addr;
    memset(&anchor_bind_addr, 0, sizeof(struct sockaddr_ll));
    anchor_bind_addr.sll_family = AF_PACKET;
    anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    anchor_bind_addr.sll_ifindex = anchor_iface_req.ifr_ifindex;
    if (bind(self->fd, (struct sockaddr *)&anchor_bind_addr, sizeof(struct sockaddr_ll)) < 0)
    {
        CORD_ERROR("CordL3RawSocketFlowPoint: bind()");
    }

    int enable = 1;
    if (setsockopt(self->fd, SOL_PACKET, PACKET_IGNORE_OUTGOING, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("CordL3RawSocketFlowPoint: setsockopt(PACKET_IGNORE_OUTGOING)");
        CORD_CLOSE(self->fd);
        CORD_EXIT(EXIT_FAILURE);
    }

    fcntl(self->fd, F_SETFL, O_NONBLOCK);
}

void CordL3RawSocketFlowPoint_dtor(CordL3RawSocketFlowPoint * const self)
{
    free(self);
}
