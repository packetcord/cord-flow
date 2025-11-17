#ifndef CORD_L2_TPACKETV3_FLOW_POINT_H
#define CORD_L2_TPACKETV3_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>
#include <memory/cord_memory.h>
#include <linux/if_packet.h>

#define CORD_CREATE_L2_TPACKETV3_FLOW_POINT CORD_CREATE_L2_TPACKETV3_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_L2_TPACKETV3_FLOW_POINT CORD_DESTROY_L2_TPACKETV3_FLOW_POINT_ON_HEAP

#define CORD_CREATE_L2_TPACKETV3_FLOW_POINT_ON_HEAP(id, anchor_iface_name, rx_ring) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL2Tpacketv3FlowPoint, id, anchor_iface_name, rx_ring)

#define CORD_CREATE_L2_TPACKETV3_FLOW_POINT_ON_STACK(id, anchor_iface_name, rx_ring) \
    (CordFlowPoint *) &NEW_ON_STACK(CordL2Tpacketv3FlowPoint, id, anchor_iface_name, rx_ring)

#define CORD_DESTROY_L2_TPACKETV3_FLOW_POINT_ON_HEAP(name) \
    do {                                                    \
        DESTROY_ON_HEAP(CordL2Tpacketv3FlowPoint, name);    \
    } while(0)

#define CORD_DESTROY_L2_TPACKETV3_FLOW_POINT_ON_STACK(name) \
    do {                                                     \
        DESTROY_ON_STACK(CordL2Tpacketv3FlowPoint, name);   \
    } while(0)

typedef struct
{
    cord_retval_t (*attach_xBPF_filter)(struct CordFlowPoint const * const self, void *filter);
    cord_retval_t (*attach_xBPF_program)(struct CordFlowPoint const * const self, void *program);
} CordL2Tpacketv3FlowPointVtbl;

typedef struct CordL2Tpacketv3FlowPoint
{
    CordFlowPoint base;
    const CordL2Tpacketv3FlowPointVtbl *vptr;
    int ifindex;
    const char *anchor_iface_name;
    struct sockaddr_ll anchor_bind_addr;
    struct cord_tpacketv3_ring **rx_ring;
    void *attached_filter;
    void *attached_program;
    // Block processing state for RX/TX coordination
    unsigned int rx_start_block_idx;
    unsigned int rx_num_blocks_processed;
    ssize_t rx_packet_count;
} CordL2Tpacketv3FlowPoint;

void CordL2Tpacketv3FlowPoint_ctor(CordL2Tpacketv3FlowPoint * const self,
                                    uint8_t id,
                                    const char *anchor_iface_name,
                                    struct cord_tpacketv3_ring **rx_ring);

void CordL2Tpacketv3FlowPoint_dtor(CordL2Tpacketv3FlowPoint * const self);

#define CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_FILTER_VCALL(self, filter)  (*(((CordL2Tpacketv3FlowPoint *)self)->vptr->attach_xBPF_filter))((self), (filter))
#define CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_FILTER CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_FILTER_VCALL

static inline cord_retval_t CordL2Tpacketv3FlowPoint_attach_xBPF_filter_vcall(CordFlowPoint const * const self, void *filter)
{
    return (*(((CordL2Tpacketv3FlowPoint *)self)->vptr->attach_xBPF_filter))(self, filter);
}

#define CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_PROGRAM_VCALL(self, program)  (*(((CordL2Tpacketv3FlowPoint *)self)->vptr->attach_xBPF_program))((self), (program))
#define CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_PROGRAM CORD_L2_TPACKETV3_FLOW_POINT_ATTACH_XBPF_PROGRAM_VCALL

static inline cord_retval_t CordL2Tpacketv3FlowPoint_attach_xBPF_program_vcall(CordFlowPoint const * const self, void *program)
{
    return (*(((CordL2Tpacketv3FlowPoint *)self)->vptr->attach_xBPF_program))(self, program);
}

#define CORD_L2_TPACKETV3_FLOW_POINT_ENSURE_INBOUND(self) (CordL2Tpacketv3FlowPoint_ensure_packet_inbound(self))

static inline cord_retval_t CordL2Tpacketv3FlowPoint_ensure_packet_inbound(CordFlowPoint const * const self)
{
    if (((CordL2Tpacketv3FlowPoint *)self)->anchor_bind_addr.sll_pkttype == PACKET_OUTGOING)
        return CORD_ERR;
    else
        return CORD_OK;
}

#endif // CORD_L2_TPACKETV3_FLOW_POINT_H
