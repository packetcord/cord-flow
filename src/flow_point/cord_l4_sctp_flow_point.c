#include <flow_point/cord_l4_sctp_flow_point.h>

static cord_retval_t CordL4SctpFlowPoint_rx_(CordL4SctpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    //
    //  Implement the rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4SctpFlowPoint_tx_(CordL4SctpFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

void CordL4SctpFlowPoint_ctor(CordL4SctpFlowPoint * const self,
                             uint8_t id,
                             size_t rx_buffer_size,
                             int fd,
                             bool server_mode,
                             uint32_t ipv4_src_addr,
                             uint32_t ipv4_dst_addr,
                             struct sockaddr_in6 ipv6_src_addr,
                             struct sockaddr_in6 ipv6_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port,
                             void *params)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL4SctpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL4SctpFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->fd = fd;
    self->server_mode = server_mode;
    self->ipv4_src_addr = ipv4_src_addr;
    self->ipv4_dst_addr = ipv4_dst_addr;

    inet_pton(AF_INET6, "::1", &(self->ipv6_src_addr));
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));

    self->src_port = src_port;
    self->dst_port = dst_port;
    self->params = params;
}

void CordL4SctpFlowPoint_dtor(CordL4SctpFlowPoint * const self)
{
    free(self);
}
