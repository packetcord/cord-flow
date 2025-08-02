#include <flow_point/cord_l4_udp_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL4UdpFlowPoint_rx_(CordL4UdpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{    
    *rx_bytes = recvfrom(self->fd, buffer, len, 0, NULL, NULL);
    if (*rx_bytes < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint_rx_: recvfrom()");
    }

    return CORD_OK;
}

static cord_retval_t CordL4UdpFlowPoint_tx_(CordL4UdpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    *tx_bytes = sendto(self->fd, buffer, len, 0, (struct sockaddr *)&(self->dst_addr_in), sizeof(self->dst_addr_in));
    if (*tx_bytes < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint_tx_: sendto()");
    }

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
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL4UdpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL4UdpFlowPoint_tx_,
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

    self->src_addr_in.sin_family = AF_INET;
    self->src_addr_in.sin_port = htons(self->src_port);
    self->src_addr_in.sin_addr.s_addr = self->ipv4_src_addr;

    self->dst_addr_in.sin_family = AF_INET;
    self->dst_addr_in.sin_port = htons(self->dst_port);
    self->dst_addr_in.sin_addr.s_addr = self->ipv4_dst_addr;
    
    int reuse = 1;
    if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint: setsockopt(SO_REUSEADDR)");
        CORD_CLOSE(self->fd);
        CORD_EXIT(EXIT_FAILURE);
    }

    if (bind(self->fd, (struct sockaddr *)&(self->src_addr_in), sizeof(self->src_addr_in)) < 0)
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
