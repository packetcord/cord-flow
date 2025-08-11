#ifndef CORD_L4_UDP_FLOW_POINT_H
#define CORD_L4_UDP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L4_UDP_FLOW_POINT CORD_CREATE_L4_UDP_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L4_UDP_FLOW_POINT CORD_DESTROY_L4_UDP_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L4_UDP_FLOW_POINT_ON_HEAP(id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL4UdpFlowPoint, id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_CREATE_L4_UDP_FLOW_POINT_ON_STACK(id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL4UdpFlowPoint, id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_DESTROY_L4_UDP_FLOW_POINT_ON_HEAP(name) \
    do {                                             \
        DESTROY_ON_HEAP(CordL4UdpFlowPoint, name);   \
    } while(0)

#define CORD_DESTROY_L4_UDP_FLOW_POINT_ON_STACK(name)\
    do {                                             \
        DESTROY_ON_STACK(CordL4UdpFlowPoint, name);  \
    } while(0)

typedef struct CordL4UdpFlowPoint
{
    CordFlowPoint base;
    struct sockaddr_in src_addr_in;
    struct sockaddr_in dst_addr_in;
    bool server_mode;
    in_addr_t ipv4_src_addr;
    in_addr_t ipv4_dst_addr;
    struct sockaddr_in6 ipv6_src_addr;
    struct sockaddr_in6 ipv6_dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    void *params;
} CordL4UdpFlowPoint;

void CordL4UdpFlowPoint_ctor(CordL4UdpFlowPoint * const self,
                             uint8_t id,
                             size_t rx_buffer_size,
                             in_addr_t ipv4_src_addr,
                             in_addr_t ipv4_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port);

void CordL4UdpFlowPoint_dtor(CordL4UdpFlowPoint * const self);

#endif // CORD_L4_UDP_FLOW_POINT_H
