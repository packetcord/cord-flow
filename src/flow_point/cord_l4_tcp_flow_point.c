#include <flow_point/cord_l4_tcp_flow_point.h>

static cord_retval_t CordL4TcpFlowPoint_rx_(CordL4TcpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{    
    //
    //  Implement the rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4TcpFlowPoint_tx_(CordL4TcpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

void CordL4TcpFlowPoint_ctor(CordL4TcpFlowPoint * const self,
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
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL4TcpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL4TcpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->fd = fd;
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
}

void CordL4TcpFlowPoint_dtor(CordL4TcpFlowPoint * const self)
{
    free(self);
}
