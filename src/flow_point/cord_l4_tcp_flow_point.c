#include <flow_point/cord_l4_tcp_flow_point.h>

static cord_retval_t CordL4TcpFlowPoint_rx_(CordL4TcpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    CORD_LOG("[CordL4TcpFlowPoint] rx()\n");
    //
    //  Implement the rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4TcpFlowPoint_tx_(CordL4TcpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    CORD_LOG("[CordL4TcpFlowPoint] tx()\n");
    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

void CordL4TcpFlowPoint_ctor(CordL4TcpFlowPoint * const self,
                             uint8_t id,
                             in_addr_t ipv4_src_addr,
                             in_addr_t ipv4_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port)
{
    CORD_LOG("[CordL4TcpFlowPoint] ctor()\n");
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL4TcpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL4TcpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->ipv4_src_addr = ipv4_src_addr;
    self->ipv4_dst_addr = ipv4_dst_addr;

    inet_pton(AF_INET6, "::1", &(self->ipv6_src_addr));
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));

    self->src_port = src_port;
    self->dst_port = dst_port;
}

void CordL4TcpFlowPoint_dtor(CordL4TcpFlowPoint * const self)
{
    CORD_LOG("[CordL4TcpFlowPoint] dtor()\n");
    close(self->base.io_handle);
    free(self);
}
