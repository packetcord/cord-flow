#ifndef CORD_L4_UDP_FLOW_POINT_H
#define CORD_L4_UDP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL4UdpFlowPoint
{
    CordFlowPoint base;
    int fd;
    bool server_mode;
    uint32_t ipv4_src_addr;
    uint32_t ipv4_dst_addr;
    uint8_t ipv6_src_addr[16];
    uint8_t ipv6_dst_addr[16];
    uint16_t src_port;
    uint16_t dst_port;
    void *params;
} CordL4UdpFlowPoint;

void CordL4UdpFlowPoint_ctor(CordL4UdpFlowPoint * const self,
                             uint8_t id,
                             size_t rx_buffer_size,
                             int fd,
                             bool server_mode,
                             uint32_t ipv4_src_addr,
                             uint32_t ipv4_dst_addr,
                             uint8_t ipv6_src_addr[16],
                             uint8_t ipv6_dst_addr[16],
                             uint16_t src_port,
                             uint16_t dst_port,
                             void *params);

void CordL4UdpFlowPoint_dtor(CordL4UdpFlowPoint * const self);

#endif // CORD_L4_UDP_FLOW_POINT_H
