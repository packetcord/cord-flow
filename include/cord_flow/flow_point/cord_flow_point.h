//
// Add _Generic compile time polymorphism
//

#ifndef CORD_FLOW_POINT_H
#define CORD_FLOW_POINT_H

#include <cord_type.h>
#include <cord_retval.h>

typedef struct CordFlowPoint CordFlowPoint;

typedef struct
{
    cord_retval_t (*rx)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes);
    cord_retval_t (*tx)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes);
} CordFlowPointVtbl;

struct CordFlowPoint
{
    const CordFlowPointVtbl *vptr;
    uint8_t id;
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
