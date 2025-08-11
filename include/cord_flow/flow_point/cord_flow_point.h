//
// Add _Generic compile time polymorphism
//

#ifndef CORD_FLOW_POINT_H
#define CORD_FLOW_POINT_H

#include <cord_type.h>
#include <cord_retval.h>

#define CORD_CREATE_FLOW_POINT CORD_CREATE_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_FLOW_POINT CORD_DESTROY_FLOW_POINT_ON_HEAP

#define CORD_CREATE_FLOW_POINT_ON_HEAP(id, rx_buffer_size, anchor_iface_name) \
    NEW_ON_HEAP(CordFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_CREATE_FLOW_POINT_ON_STACK(id, rx_buffer_size, anchor_iface_name) \
    &NEW_ON_STACK(CordFlowPoint, id, rx_buffer_size, anchor_iface_name)

#define CORD_DESTROY_FLOW_POINT_ON_HEAP(name) \
    do {                                                    \
        DESTROY_ON_HEAP(CordFlowPoint, name);    \
    } while(0)

#define CORD_DESTROY_FLOW_POINT_ON_STACK(name) \
do {                                            \
    DESTROY_ON_STACK(CordFlowPoint, name);    \
} while(0)

typedef struct CordFlowPoint CordFlowPoint;

typedef struct
{
    cord_retval_t (*rx)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes);
    cord_retval_t (*tx)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes);
} CordFlowPointVtbl;

typedef struct
{
    uint32_t nb_rx_packets;
    uint32_t nb_tx_packets;
    uint64_t nb_rx_bytes;
    uint64_t nb_tx_bytes;
    uint32_t nb_lost_packets;
    uint64_t nb_lost_bytes;
    uint64_t nb_cache_hits;
    uint64_t nb_cache_misses;
    uint32_t nb_dropped_packets;
    uint32_t nb_filtered_packets;
} CordFlowPointStats;

struct CordFlowPoint
{
    const CordFlowPointVtbl *vptr;
    uint8_t id;
    int io_handle;
    size_t rx_buffer_size;
};

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id, size_t rx_buffer_size);
void CordFlowPoint_dtor(CordFlowPoint * const self);

#define CORDFLOWPOINT_RX_VCALL(self, buffer, len, rx_bytes)   (*(self->vptr->rx))((self), (buffer), (len), (rx_bytes))
#define CORDFLOWPOINT_TX_VCALL(self, buffer, len, tx_bytes)   (*(self->vptr->tx))((self), (buffer), (len), (tx_bytes))

static inline cord_retval_t CordFlowPoint_rx_vcall(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    return (*(self->vptr->rx))(self, buffer, len, rx_bytes);
}

static inline cord_retval_t CordFlowPoint_tx_vcall(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    return (*(self->vptr->tx))(self, buffer, len, tx_bytes);
}

#endif // CORD_FLOW_POINT_H
