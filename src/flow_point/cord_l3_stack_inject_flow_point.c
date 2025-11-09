#include <flow_point/cord_l3_stack_inject_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL3StackInjectFlowPoint_rx_(CordL3StackInjectFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] rx()\n");
#endif
    //
    //  Pass
    //

    return CORD_OK;
}

static cord_retval_t CordL3StackInjectFlowPoint_tx_(CordL3StackInjectFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] tx()\n");
#endif
    *tx_bytes = sendto(self->base.io_handle, buffer, len, 0, (struct sockaddr *)&(self->dst_addr_in), sizeof(self->dst_addr_in));
    if (*tx_bytes < 0)
    {
        CORD_ERROR("[CordL3StackInjectFlowPoint] sendto()");
    }

    return CORD_OK;
}

void CordL3StackInjectFlowPoint_set_target_ipv4_(CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] set_target_ipv4()\n");
#endif
    self->dst_addr_in.sin_addr.s_addr = ipv4_addr;
}

void CordL3StackInjectFlowPoint_set_target_ipv6_(CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] set_target_ipv6()\n");
#endif
    self->dst_addr_in6.sin6_addr = ipv6_addr;
}

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordL3StackInjectFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordL3StackInjectFlowPoint_tx_,
        .cleanup = (void (*)(CordFlowPoint * const))&CordL3StackInjectFlowPoint_dtor,
    };

    static const CordL3StackInjectFlowPointVtbl vtbl_deriv = {
        .set_target_ipv4 = (void (*)(CordFlowPoint * const self, in_addr_t ipv4_addr))&CordL3StackInjectFlowPoint_set_target_ipv4_,
        .set_target_ipv6 = (void (*)(CordFlowPoint * const self, struct in6_addr ipv6_addr))&CordL3StackInjectFlowPoint_set_target_ipv6_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->vptr = &vtbl_deriv;
    self->base.io_handle = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL3StackInjectFlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->ipv4_dst_addr = htonl(INADDR_LOOPBACK);
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));
    
    self->dst_addr_in.sin_family = AF_INET;
    self->dst_addr_in.sin_addr.s_addr = self->ipv4_dst_addr;

    self->dst_addr_in6.sin6_family = AF_INET6;
    self->dst_addr_in6.sin6_addr = self->ipv6_dst_addr;
    
    int enable = 1;
    if (setsockopt(self->base.io_handle, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("[CordL3StackInjectFlowPoint] setsockopt(IPPROTO_IP, IP_HDRINCL)");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL3StackInjectFlowPoint] dtor()\n");
#endif
    close(self->base.io_handle);
    free(self);
}
