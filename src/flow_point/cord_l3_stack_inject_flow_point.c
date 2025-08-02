#include <flow_point/cord_l3_stack_inject_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL3StackInjectFlowPoint_rx_(CordL3StackInjectFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{   
    //
    //  Pass
    //

    return CORD_OK;
}

static cord_retval_t CordL3StackInjectFlowPoint_tx_(CordL3StackInjectFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    *tx_bytes = sendto(self->fd, buffer, len, 0, (struct sockaddr *)&(self->dst_addr_in), sizeof(self->dst_addr_in));
    if (*tx_bytes < 0)
    {
        CORD_ERROR("CordL4UdpFlowPoint_tx_: sendto()");
    }

    return CORD_OK;
}

void CordL3StackInjectFlowPoint_set_target_ipv4_(CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr)
{
    self->dst_addr_in.sin_addr.s_addr = ipv4_addr;
}

void CordL3StackInjectFlowPoint_set_target_ipv6_(CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr)
{
    self->dst_addr_in6.sin6_addr = ipv6_addr;
}

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id,
                                     size_t rx_buffer_size,
                                     int fd)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL3StackInjectFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL3StackInjectFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->set_target_ipv4 = &CordL3StackInjectFlowPoint_set_target_ipv4_;
    self->set_target_ipv6 = &CordL3StackInjectFlowPoint_set_target_ipv6_;

    self->fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (self->fd < 0)
    {
        CORD_ERROR("CordL3StackInjectFlowPoint: socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->ipv4_dst_addr = htonl(INADDR_LOOPBACK);
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));
    
    self->dst_addr_in.sin_family = AF_INET;
    self->dst_addr_in.sin_addr.s_addr = self->ipv4_dst_addr;

    self->dst_addr_in6.sin6_family = AF_INET6;
    self->dst_addr_in6.sin6_addr = self->ipv6_dst_addr;
    
    int enable = 1;
    if (setsockopt(self->fd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("CordL3StackInjectFlowPoint: setsockopt(IPPROTO_IP, IP_HDRINCL)");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self)
{
    free(self);
}
