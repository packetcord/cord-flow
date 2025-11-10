//
// Add _Generic compile time polymorphism
//

#ifndef CORD_FLOW_POINT_H
#define CORD_FLOW_POINT_H

#include <cord_type.h>
#include <cord_retval.h>

#define CORD_CREATE_FLOW_POINT CORD_CREATE_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_FLOW_POINT CORD_DESTROY_FLOW_POINT_ON_HEAP

#define CORD_CREATE_FLOW_POINT_ON_HEAP(id) \
    NEW_ON_HEAP(CordFlowPoint, id)

#define CORD_CREATE_FLOW_POINT_ON_STACK(id)\
    &NEW_ON_STACK(CordFlowPoint, id)

#define CORD_DESTROY_FLOW_POINT_ON_HEAP(name) \
    do {                                      \
        DESTROY_ON_HEAP(CordFlowPoint, name); \
    } while(0)

#define CORD_DESTROY_FLOW_POINT_ON_STACK(name)\
    do {                                      \
        DESTROY_ON_STACK(CordFlowPoint, name);\
    } while(0)

typedef struct CordFlowPoint CordFlowPoint;

typedef struct
{
    cord_retval_t (*rx)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rxed);
    cord_retval_t (*tx)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *txed);
    void     (*cleanup)(CordFlowPoint const * const self);
} CordFlowPointVtbl;

typedef struct
{
    uint32_t nb_rx_packets;
    uint32_t nb_tx_packets;
    uint64_t nb_rxed;
    uint64_t nb_txed;
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
};

static inline cord_retval_t CordFlowPoint_rx_vcall(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rxed)
{
    return (*(self->vptr->rx))(self, queue_id, buffer, len, rxed);
}

static inline cord_retval_t CordFlowPoint_tx_vcall(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *txed)
{
    return (*(self->vptr->tx))(self, queue_id, buffer, len, txed);
}

#define CORD_FLOW_POINT_RX_VCALL(self, queue_id, buffer, len, rxed)   (*(self->vptr->rx))((self), (queue_id), (buffer), (len), (rxed))
#define CORD_FLOW_POINT_TX_VCALL(self, queue_id, buffer, len, txed)   (*(self->vptr->tx))((self), (queue_id), (buffer), (len), (txed))

#define CORD_FLOW_POINT_RX CORD_FLOW_POINT_RX_VCALL
#define CORD_FLOW_POINT_TX CORD_FLOW_POINT_TX_VCALL

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id);
void CordFlowPoint_dtor(CordFlowPoint * const self);

#endif // CORD_FLOW_POINT_H
