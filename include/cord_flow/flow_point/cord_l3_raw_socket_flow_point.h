#ifndef CORD_L3_RAW_SOCKET_FLOW_POINT_H
#define CORD_L3_RAW_SOCKET_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP(id, anchor_iface_name) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL3RawSocketFlowPoint, id, anchor_iface_name)

#define CORD_CREATE_L3_RAW_SOCKET_FLOW_POINT_ON_STACK(id, anchor_iface_name)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL3RawSocketFlowPoint, id, anchor_iface_name)

#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_HEAP(name) \
    do {                                                    \
        DESTROY_ON_HEAP(CordL3RawSocketFlowPoint, name);    \
    } while(0)

#define CORD_DESTROY_L3_RAW_SOCKET_FLOW_POINT_ON_STACK(name)\
    do {                                                    \
        DESTROY_ON_STACK(CordL3RawSocketFlowPoint, name);   \
    } while(0)

typedef struct
{
    cord_retval_t (*attach_xBPF_filter)(struct CordFlowPoint const * const self, void *filter);     // Hint: setsockopt via SO_ATTACH_FILTER
    cord_retval_t (*attach_xBPF_program)(struct CordFlowPoint const * const self, void *program);   // Hint: setsockopt via SO_ATTACH_BPF
} CordL3RawSocketFlowPointVtbl;

typedef struct CordL3RawSocketFlowPoint
{
    CordFlowPoint base;
    const CordL3RawSocketFlowPointVtbl *vptr;
    int ifindex;
    const char *anchor_iface_name;
    struct sockaddr_ll anchor_bind_addr;
    int fanout_id;
    bool use_tpacket_v3;
    void *ring;
    void *attached_filter;
    void *attached_program;
    void *params;
} CordL3RawSocketFlowPoint;

void CordL3RawSocketFlowPoint_ctor(CordL3RawSocketFlowPoint * const self,
                                   uint8_t id,
                                   const char *anchor_iface_name);

void CordL3RawSocketFlowPoint_dtor(CordL3RawSocketFlowPoint * const self);

#define CORD_L3_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_FILTER_VCALL(self, filter)  (*(((CordL3RawSocketFlowPoint *)self)->vptr->attach_xBPF_filter))((self), (filter))
#define CORD_L3_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_FILTER CORD_L3_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_FILTER_VCALL

static inline cord_retval_t CordL3RawSocketFlowPoint_attach_xBPF_filter_vcall(CordFlowPoint const * const self, void *filter)
{
    return (*(((CordL3RawSocketFlowPoint *)self)->vptr->attach_xBPF_filter))(self, filter);
}

#define CORD_L2_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_PROGRAM_VCALL(self, program)  (*(((CordL3RawSocketFlowPoint *)self)->vptr->attach_xBPF_program))((self), (program))
#define CORD_L2_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_PROGRAM CORD_L2_RAW_SOCKET_FLOW_POINT_ATTACH_XBPF_PROGRAM_VCALL

static inline cord_retval_t CordL3RawSocketFlowPoint_attach_xBPF_program_vcall(CordFlowPoint const * const self, void *program)
{
    return (*(((CordL3RawSocketFlowPoint *)self)->vptr->attach_xBPF_program))(self, program);
}

#define CORD_L3_RAW_SOCKET_FLOW_POINT_ENSURE_INBOUD(self) (CordL3RawSocketFlowPoint_ensure_packet_inboud(self))

static inline cord_retval_t CordL3RawSocketFlowPoint_ensure_packet_inboud(CordFlowPoint const * const self)
{
    if (((CordL3RawSocketFlowPoint *)self)->anchor_bind_addr.sll_pkttype == PACKET_OUTGOING)
        return CORD_ERR;
    else
        return CORD_OK;
}

#endif // CORD_L3_RAW_SOCKET_FLOW_POINT_H
