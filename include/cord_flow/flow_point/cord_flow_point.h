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
    cord_retval_t (*rx)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes);
    cord_retval_t (*tx)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes);
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
};

static inline cord_retval_t CordFlowPoint_rx_vcall(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
    return (*(self->vptr->rx))(self, buffer, len, rx_bytes);
}

static inline cord_retval_t CordFlowPoint_tx_vcall(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
    return (*(self->vptr->tx))(self, buffer, len, tx_bytes);
}

#define CORD_FLOW_POINT_RX_VCALL(self, buffer, len, rx_bytes)   (*(self->vptr->rx))((self), (buffer), (len), (rx_bytes))
#define CORD_FLOW_POINT_TX_VCALL(self, buffer, len, tx_bytes)   (*(self->vptr->tx))((self), (buffer), (len), (tx_bytes))

#define CORD_FLOW_POINT_RX CORD_FLOW_POINT_RX_VCALL
#define CORD_FLOW_POINT_TX CORD_FLOW_POINT_TX_VCALL

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id);
void CordFlowPoint_dtor(CordFlowPoint * const self);

#endif // CORD_FLOW_POINT_H
