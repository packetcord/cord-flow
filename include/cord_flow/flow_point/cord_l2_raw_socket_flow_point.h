#ifndef CORD_L2_RAW_SOCKET_FLOW_POINT_H
#define CORD_L2_RAW_SOCKET_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L2_RAW_SOCKET_FLOW_POINT CORD_CREATE_L2_RAW_SOCKET_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L2_RAW_SOCKET_FLOW_POINT_ON_HEAP(id, rx_buffer_size, anchor_iface_name) \
    (CordFlowPoint *) NEW(CordL2RawSocketFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_CREATE_L2_RAW_SOCKET_FLOW_POINT_ON_STACK(id, rx_buffer_size, anchor_iface_name) \
    (CordFlowPoint *) &NEW_ON_STACK(CordL2RawSocketFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_DESTROY_L2_RAW_SOCKET_FLOW_POINT(name) \
    do {                                            \
        DESTROY(CordL2RawSocketFlowPoint, name);    \
    } while(0)

typedef struct CordL2RawSocketFlowPoint
{
    CordFlowPoint base;
    cord_retval_t (*attach_filter)(struct CordL2RawSocketFlowPoint const * const self, void *filter);
    int fd;
    int ifindex;
    const char *anchor_iface_name;
    struct sockaddr_ll anchor_bind_addr;
    int fanout_id;
    bool use_tpacket_v3;
    void *ring;
    void *attached_filter;
    void *params;
} CordL2RawSocketFlowPoint;

void CordL2RawSocketFlowPoint_ctor(CordL2RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   size_t rx_buffer_size,
                                   const char *anchor_iface_name);

void CordL2RawSocketFlowPoint_dtor(CordL2RawSocketFlowPoint * const self);

#define CORDL2RAWSOCKETFLOWPOINT_ATTACH_FILTER_VCALL(self, filter)  (*(self->attach_filter))((self), (filter))

static inline cord_retval_t CordL2RawSocketFlowPoint_attach_filter_vcall(CordL2RawSocketFlowPoint const * const self, void *filter)
{
    return (*(self->attach_filter))(self, filter);
}

#endif // CORD_L2_RAW_SOCKET_FLOW_POINT_H
