#ifndef CORD_L4_TCP_FLOW_POINT_H
#define CORD_L4_TCP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L4_TCP_FLOW_POINT CORD_CREATE_L4_TCP_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L4_TCP_FLOW_POINT CORD_DESTROY_L4_TCP_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L4_TCP_FLOW_POINT_ON_HEAP(id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL4TcpFlowPoint, id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_CREATE_L4_TCP_FLOW_POINT_ON_STACK(id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL4TcpFlowPoint, id, rx_buffer_size, ipv4_src_addr, ipv4_dst_addr, src_port, dst_port)

#define CORD_DESTROY_L4_TCP_FLOW_POINT_ON_HEAP(name) \
    do {                                             \
        DESTROY_ON_HEAP(CordL4TcpFlowPoint, name);   \
    } while(0)

#define CORD_DESTROY_L4_TCP_FLOW_POINT_ON_STACK(name)\
    do {                                             \
        DESTROY_ON_STACK(CordL4TcpFlowPoint, name);  \
    } while(0)

typedef struct CordL4TcpFlowPoint
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
} CordL4TcpFlowPoint;

void CordL4TcpFlowPoint_ctor(CordL4TcpFlowPoint * const self,
                             uint8_t id,
                             size_t rx_buffer_size,
                             in_addr_t ipv4_src_addr,
                             in_addr_t ipv4_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port);

void CordL4TcpFlowPoint_dtor(CordL4TcpFlowPoint * const self);

#endif // CORD_L4_TCP_FLOW_POINT_H
