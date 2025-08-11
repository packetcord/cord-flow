#ifndef CORD_L4_SCTP_FLOW_POINT_H
#define CORD_L4_SCTP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L4_SCTP_FLOW_POINT CORD_CREATE_L4_SCTP_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L4_SCTP_FLOW_POINT CORD_DESTROY_L4_SCTP_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L4_SCTP_FLOW_POINT_ON_HEAP(id, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL4SctpFlowPoint, id, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_CREATE_L4_SCTP_FLOW_POINT_ON_STACK(id, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL4SctpFlowPoint, id, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_DESTROY_L4_SCTP_FLOW_POINT_ON_HEAP(name) \
    do {                                              \
        DESTROY_ON_HEAP(CordL4SctpFlowPoint, name);   \
    } while(0)

#define CORD_DESTROY_L4_SCTP_FLOW_POINT_ON_STACK(name)\
    do {                                              \
        DESTROY_ON_STACK(CordL4SctpFlowPoint, name);  \
    } while(0)

typedef struct CordL4SctpFlowPoint
{
    CordFlowPoint base;
    bool server_mode;
    uint32_t ipv4_src_addr;
    uint32_t ipv4_dst_addr;
    struct sockaddr_in6 ipv6_src_addr;
    struct sockaddr_in6 ipv6_dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    void *params;
} CordL4SctpFlowPoint;

void CordL4SctpFlowPoint_ctor(CordL4SctpFlowPoint * const self,
                              uint8_t id,
                              in_addr_t ipv4_src_addr,
                              in_addr_t ipv4_dst_addr,
                              uint16_t src_port,
                              uint16_t dst_port);

void CordL4SctpFlowPoint_dtor(CordL4SctpFlowPoint * const self);

#endif // CORD_L4_SCTP_FLOW_POINT_H
