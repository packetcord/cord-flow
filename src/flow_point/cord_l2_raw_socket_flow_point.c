#include <flow_point/cord_l2_raw_socket_flow_point.h>
#include <cord_error.h>
#include <linux/filter.h>

static cord_retval_t CordL2RawSocketFlowPoint_rx_(CordL2RawSocketFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2RawSocketFlowPoint] rx()\n");
#endif
    socklen_t addr_len = sizeof(self->anchor_bind_addr);
    *rx_bytes = recvfrom(self->base.io_handle, buffer, len, 0, (struct sockaddr *)&(self->anchor_bind_addr), &addr_len);
    if (*rx_bytes < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] rx : recvfrom()");
        return CORD_ERR;
    }

    return CORD_OK;
}

static cord_retval_t CordL2RawSocketFlowPoint_tx_(CordL2RawSocketFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2RawSocketFlowPoint] tx()\n");
#endif
    socklen_t addr_len = sizeof(self->anchor_bind_addr);
    *tx_bytes = sendto(self->base.io_handle, buffer, len, 0, (struct sockaddr *)&(self->anchor_bind_addr), addr_len);
    if (*tx_bytes < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] tx : sendto()");
        return CORD_ERR;
    }

    return CORD_OK;
}

static cord_retval_t CordL2RawSocketFlowPoint_attach_xBPF_(CordL2RawSocketFlowPoint const * const self, void *filter, void *params)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2RawSocketFlowPoint] attach_xBPF()\n");
#endif
    if (filter != NULL)
    {
        if (params != NULL)
        {
            // The params variable holds the filter type
            cord_filter_type_t filter_type = *((cord_filter_type_t *)params);
            if (filter_type == CBPF_FILTER)
            {
                CORD_LOG("[CordL2RawSocketFlowPoint] Filter type is cBPF.\n");
                // The filter variable holds a pointer to (struct sock_fprog)
                struct sock_fprog * const filter_fprog = (struct sock_fprog *)filter;

                if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_ATTACH_FILTER, filter_fprog, sizeof(struct sock_fprog)) < 0)
                {
	                CORD_ERROR("[CordL2RawSocketFlowPoint] setsockopt attach cBPF filter");
	                CORD_CLOSE(self->base.io_handle);
	                CORD_EXIT(EXIT_FAILURE);
                }
                CORD_LOG("[CordL2RawSocketFlowPoint] cBPF filter attached.\n");
            }
            else if (filter_type == EBPF_FILTER)
            {
                CORD_LOG("[CordL2RawSocketFlowPoint] Filter type is eBPF.\n");
                // The filter variable holds the file descriptor (int) to the eBPF program
                int ebpf_filter_program_fd = *((int *)filter);

                if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_ATTACH_BPF, &ebpf_filter_program_fd, sizeof(ebpf_filter_program_fd)) < 0)
                {
                    CORD_ERROR("[CordL2RawSocketFlowPoint] setsockopt attach eBPF filter");
	                CORD_CLOSE(self->base.io_handle);
	                CORD_EXIT(EXIT_FAILURE);
                }
                CORD_LOG("[CordL2RawSocketFlowPoint] eBPF filter attached.\n");
            }
            else 
            {
                return CORD_ERR;
            }
        }
    }

    return CORD_OK;
}

void CordL2RawSocketFlowPoint_ctor(CordL2RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   const char *anchor_iface_name)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2RawSocketFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes))&CordL2RawSocketFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes))&CordL2RawSocketFlowPoint_tx_,
        .attach_xBPF = (cord_retval_t (*)(CordFlowPoint const * const self, void *filter, void *params))&CordL2RawSocketFlowPoint_attach_xBPF_,
        .cleanup = (void     (*)(CordFlowPoint const * const))&CordL2RawSocketFlowPoint_dtor,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->anchor_iface_name = anchor_iface_name;

    self->base.io_handle = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_BINDTODEVICE, self->anchor_iface_name, strlen(self->anchor_iface_name)) < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] setsockopt(SO_BINDTODEVICE)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    struct ifreq anchor_iface_req;
    memset(&anchor_iface_req, 0, sizeof(struct ifreq));
    strncpy(anchor_iface_req.ifr_name, self->anchor_iface_name, IFNAMSIZ);
    if (ioctl(self->base.io_handle, SIOCGIFINDEX, &anchor_iface_req) < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] ioctl(SIOCGIFINDEX)");
    }
    self->ifindex = anchor_iface_req.ifr_ifindex;

    memset(&(self->anchor_bind_addr), 0, sizeof(struct sockaddr_ll));
    self->anchor_bind_addr.sll_family = AF_PACKET;
    self->anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    self->anchor_bind_addr.sll_ifindex = anchor_iface_req.ifr_ifindex;
    self->anchor_bind_addr.sll_halen = ETH_ALEN;
    if (bind(self->base.io_handle, (struct sockaddr *)&(self->anchor_bind_addr), sizeof(struct sockaddr_ll)) < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] bind()");
    }

    int enable = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_IGNORE_OUTGOING, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("[CordL2RawSocketFlowPoint] setsockopt(PACKET_IGNORE_OUTGOING)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK);
}

void CordL2RawSocketFlowPoint_dtor(CordL2RawSocketFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2RawSocketFlowPoint] dtor()\n");
#endif
    close(self->base.io_handle);
    free(self);
}
