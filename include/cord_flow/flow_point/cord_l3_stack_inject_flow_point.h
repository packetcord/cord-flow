#ifndef CORD_L3_STACK_INJECT_FLOW_POINT_H
#define CORD_L3_STACK_INJECT_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL3StackInjectFlowPoint
{
    CordFlowPoint base;
    void (*set_target_ipv4)(struct CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr);
    void (*set_target_ipv6)(struct CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr);
    int fd;
    struct sockaddr_in dst_addr_in;
    struct sockaddr_in6 dst_addr_in6;
    in_addr_t ipv4_dst_addr;
    struct in6_addr ipv6_dst_addr;
} CordL3StackInjectFlowPoint;

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id,
                                     size_t rx_buffer_size,
                                     int fd);

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self);

#define CORDL3STACKINJECTFLOWPOINT_SET_TARGET_IPV4_VCALL(self, ipv4_addr)  (*(self->set_target_ipv4))((self), (ipv4_addr))
#define CORDL3STACKINJECTFLOWPOINT_SET_TARGET_IPV6_VCALL(self, ipv6_addr)  (*(self->set_target_ipv6))((self), (ipv6_addr))

static inline void CordL3StackInjectFlowPoint_set_target_ipv4_vcall(CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr)
{
    (*(self->set_target_ipv4))(self, ipv4_addr);
}

static inline void CordL3StackInjectFlowPoint_set_target_ipv6_vcall(CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr)
{
    (*(self->set_target_ipv6))(self, ipv6_addr);
}

#endif // CORD_L3_STACK_INJECT_FLOW_POINT_H
