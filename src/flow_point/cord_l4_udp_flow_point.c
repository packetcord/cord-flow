#include <flow_point/cord_l4_udp_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL4UdpFlowPoint_rx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL4UdpFlowPoint *l3_stack_inj_fp = (const CordL4UdpFlowPoint *)self;
    
    //
    //  Implement the rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4UdpFlowPoint_tx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL4UdpFlowPoint *l3_stack_inj_fp = (const CordL4UdpFlowPoint *)self;

    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

void CordL4UdpFlowPoint_ctor(CordL4UdpFlowPoint * const self,
                             uint8_t id,
                             size_t rx_buffer_size,
                             int fd,
                             bool server_mode,
                             uint32_t ipv4_src_addr,
                             uint32_t ipv4_dst_addr,
                             uint8_t ipv6_src_addr[16],
                             uint8_t ipv6_dst_addr[16],
                             uint16_t src_port,
                             uint16_t dst_port,
                             void *params)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL4UdpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL4UdpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->server_mode = server_mode;
    self->ipv4_src_addr = ipv4_src_addr;
    self->ipv4_dst_addr = ipv4_dst_addr;

    for (uint8_t n = 0; n < 16; n++)
    {
        self->ipv6_src_addr[n] = ipv6_src_addr[n];
        self->ipv6_dst_addr[n] = ipv6_dst_addr[n];
    }

    self->src_port = src_port;
    self->dst_port = dst_port;
    self->params = params;

    self->fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in src_addr_in = {
        .sin_family = AF_INET,
        .sin_port = htons(self->src_port),
        .sin_addr.s_addr = self->ipv4_src_addr,
    };

    struct sockaddr_in dst_addr_in = {
        .sin_family = AF_INET,
        .sin_port = htons(self->dst_port),
        .sin_addr.s_addr = self->ipv4_dst_addr,
    };

    int reuse = 1;
    if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint: setsockopt(SO_REUSEADDR)");
        CORD_CLOSE(self->fd);
        CORD_EXIT(EXIT_FAILURE);
    }

    if (bind(self->fd, (struct sockaddr *)&src_addr_in, sizeof(src_addr_in)) < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint: bind()");
        CORD_CLOSE(self->fd);
        CORD_EXIT(EXIT_FAILURE);
    }
    
    CORD_LOG("CordL4UdpFlowPoint: Successfully bound to port %d\n", self->src_port);

    fcntl(self->fd, F_SETFL, O_NONBLOCK);
}

void CordL4UdpFlowPoint_dtor(CordL4UdpFlowPoint * const self)
{
    free(self);
}
