#ifndef CORD_L3_RAW_SOCKET_FLOW_POINT_H
#define CORD_L3_RAW_SOCKET_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP(id, rx_buffer_size, anchor_iface_name) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL3RawSocketFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_STACK(id, rx_buffer_size, anchor_iface_name)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL3RawSocketFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP(name) \
    do {                                                    \
        DESTROY_ON_HEAP(CordL3RawSocketFlowPoint, name);    \
    } while(0)

#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_STACK(name)\
    do {                                                    \
        DESTROY_ON_STACK(CordL3RawSocketFlowPoint, name);   \
    } while(0)

typedef struct CordL3RawSocketFlowPoint
{
    CordFlowPoint base;
    cord_retval_t (*attach_filter)(struct CordL3RawSocketFlowPoint const * const self, void *filter);
    int ifindex;
    const char *anchor_iface_name;
    int fanout_id;
    bool use_tpacket_v3;
    void *ring;
    void *attached_filter;
    void *params;
} CordL3RawSocketFlowPoint;

void CordL3RawSocketFlowPoint_ctor(CordL3RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   size_t rx_buffer_size,
                                   const char *anchor_iface_name);

void CordL3RawSocketFlowPoint_dtor(CordL3RawSocketFlowPoint * const self);

#define CORDL3RAWSOCKETFLOWPOINT_ATTACH_FILTER_VCALL(self, filter)  (*(self->attach_filter))((self), (filter))

static inline cord_retval_t CordL3RawSocketFlowPoint_attach_filter_vcall(CordL3RawSocketFlowPoint const * const self, void *filter)
{
    return (*(self->attach_filter))(self, filter);
}

#endif // CORD_L3_RAW_SOCKET_FLOW_POINT_H
