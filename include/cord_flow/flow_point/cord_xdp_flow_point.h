#ifndef CORD_XDP_FLOW_POINT_H
#define CORD_XDP_FLOW_POINT_H

#ifdef ENABLE_XDP_DATAPLANE

#include <flow_point/cord_flow_point.h>
#include <memory/cord_memory.h>

#include <xdp/xsk.h>
#include <linux/if_xdp.h>
#include <linux/if_link.h>

#define CORD_CREATE_XDP_FLOW_POINT CORD_CREATE_XDP_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_XDP_FLOW_POINT CORD_DESTROY_XDP_FLOW_POINT_ON_HEAP

#define CORD_CREATE_XDP_FLOW_POINT_ON_HEAP(id, xsk_info) \
    (CordFlowPoint *) NEW_ON_HEAP(CordXdpFlowPoint, id, xsk_info)

#define CORD_CREATE_XDP_FLOW_POINT_ON_STACK(id, xsk_info)\
    (CordFlowPoint *) &NEW_ON_STACK(CordXdpFlowPoint, id, xsk_info)

#define CORD_DESTROY_XDP_FLOW_POINT_ON_HEAP(name) \
    do {                                          \
        DESTROY_ON_HEAP(CordXdpFlowPoint, name);  \
    } while(0)

#define CORD_DESTROY_XDP_FLOW_POINT_ON_STACK(name)\
    do {                                          \
        DESTROY_ON_STACK(CordXdpFlowPoint, name); \
    } while(0)

typedef struct CordXdpFlowPoint
{
    CordFlowPoint base;
    cord_retval_t (*fill)(struct CordXdpFlowPoint const * const self);
    cord_retval_t (*drain_completion)(struct CordXdpFlowPoint const * const self);
    struct cord_xdp_socket_info **xsk_info;
    void *params;
} CordXdpFlowPoint;

void CordXdpFlowPoint_ctor(CordXdpFlowPoint * const self,
                           uint8_t id,
                           struct cord_xdp_socket_info **xsk_info);

void CordXdpFlowPoint_dtor(CordXdpFlowPoint * const self);

#define CORDXDPFLOWPOINT_FILL_VCALL(self)   (*(self->fill))((self))
#define CORDXDPFLOWPOINT_DRAIN_COMPLETION_VCALL(self)   (*(self->drain_completion))((self))

static inline cord_retval_t CordXdpFlowPoint_fill_vcall(CordXdpFlowPoint const * const self)
{
    return (*(self->fill))(self);
}

static inline cord_retval_t CordXdpFlowPoint_drain_completion_vcall(CordXdpFlowPoint const * const self)
{
    return (*(self->drain_completion))(self);
}

#endif // ENABLE_XDP_DATAPLANE

#endif // CORD_XDP_FLOW_POINT_H
