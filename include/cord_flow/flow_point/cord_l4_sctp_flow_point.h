#ifndef CORD_L4_SCTP_FLOW_POINT_H
#define CORD_L4_SCTP_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL4SctpFlowPoint
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
} CordL4SctpFlowPoint;

void CordL4SctpFlowPoint_ctor(CordL4SctpFlowPoint * const self,
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

void CordL4SctpFlowPoint_dtor(CordL4SctpFlowPoint * const self);

#endif // CORD_L4_SCTP_FLOW_POINT_H
