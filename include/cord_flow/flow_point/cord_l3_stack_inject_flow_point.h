#ifndef CORD_L3_STACK_INJECT_FLOW_POINT_H
#define CORD_L3_STACK_INJECT_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L3_STACK_INJECT_FLOW_POINT CORD_CREATE_L3_STACK_INJECT_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L3_STACK_INJECT_FLOW_POINT CORD_DESTROY_L3_STACK_INJECT_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L3_STACK_INJECT_FLOW_POINT_ON_HEAP(id) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL3StackInjectFlowPoint, id)

#define CORD_CREATE_L3_STACK_INJECT_FLOW_POINT_ON_STACK(id)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL3StackInjectFlowPoint, id)

#define CORD_DESTROY_L3_STACK_INJECT_FLOW_POINT_ON_HEAP(name) \
    do {                                                      \
        DESTROY_ON_HEAP(CordL3StackInjectFlowPoint, name);    \
    } while(0)

#define CORD_DESTROY_L3_STACK_INJECT_FLOW_POINT_ON_STACK(name)\
    do {                                                      \
        DESTROY_ON_STACK(CordL3StackInjectFlowPoint, name);   \
    } while(0)

typedef struct CordL3StackInjectFlowPoint
{
    CordFlowPoint base;
    void (*set_target_ipv4)(struct CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr);
    void (*set_target_ipv6)(struct CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr);
    struct sockaddr_in dst_addr_in;
    struct sockaddr_in6 dst_addr_in6;
    in_addr_t ipv4_dst_addr;
    struct in6_addr ipv6_dst_addr;
} CordL3StackInjectFlowPoint;

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id);

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self);

#define CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV4_VCALL(self, ipv4_addr)  (*(self->set_target_ipv4))((self), (ipv4_addr))
#define CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV6_VCALL(self, ipv6_addr)  (*(self->set_target_ipv6))((self), (ipv6_addr))

#define CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV4   CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV4_VCALL
#define CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV6   CORD_L3_STACK_INJECT_FLOW_POINT_SET_TARGET_IPV6_VCALL

static inline void CordL3StackInjectFlowPoint_set_target_ipv4_vcall(CordL3StackInjectFlowPoint * const self, in_addr_t ipv4_addr)
{
    (*(self->set_target_ipv4))(self, ipv4_addr);
}

static inline void CordL3StackInjectFlowPoint_set_target_ipv6_vcall(CordL3StackInjectFlowPoint * const self, struct in6_addr ipv6_addr)
{
    (*(self->set_target_ipv6))(self, ipv6_addr);
}

#endif // CORD_L3_STACK_INJECT_FLOW_POINT_H
