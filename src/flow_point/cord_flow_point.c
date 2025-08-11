//
// Add _Generic compile time polymorphism
//

#include <flow_point/cord_flow_point.h>
#include <cord_retval.h>

static cord_retval_t CordFlowPoint_rx_(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    CORD_LOG("[CordFlowPoint] rx()\n");
    (void)self;
    (void)buffer;
    (void)len;
    (void)rx_bytes;
    return CORD_OK;
}

static cord_retval_t CordFlowPoint_tx_(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    CORD_LOG("[CordFlowPoint] tx()\n");
    (void)self;
    (void)buffer;
    (void)len;
    (void)tx_bytes;
    return CORD_OK;
}

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id)
{
    CORD_LOG("[CordFlowPoint] ctor()\n");
    static const CordFlowPointVtbl vtbl = {
        .rx = CordFlowPoint_rx_,
        .tx = CordFlowPoint_tx_,
    };

    self->vptr = &vtbl;
    self->id = id;
}

void CordFlowPoint_dtor(CordFlowPoint * const self)
{
    CORD_LOG("[CordFlowPoint] dtor()\n");
    free(self);
}
