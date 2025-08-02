#ifndef CORD_L4_TCP_FLOW_POINT_H
#define CORD_L4_TCP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL4TcpFlowPoint
{
    CordFlowPoint base;
    int fd;
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
                             int fd,
                             bool server_mode,
                             uint32_t ipv4_src_addr,
                             uint32_t ipv4_dst_addr,
                             struct sockaddr_in6 ipv6_src_addr,
                             struct sockaddr_in6 ipv6_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port,
                             void *params);

void CordL4TcpFlowPoint_dtor(CordL4TcpFlowPoint * const self);

#endif // CORD_L4_TCP_FLOW_POINT_H
