//
// Add _Generic compile time polymorphism
//

#include <flow_point/cord_flow_point.h>
#include <cord_retval.h>

static cord_retval_t CordFlowPoint_rx_(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    (void)self;
    (void)buffer;
    (void)len;
    (void)rx_bytes;
    CORD_LOG("[CordFlowPoint] rx()\n");
    return CORD_OK;
}

static cord_retval_t CordFlowPoint_tx_(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    (void)self;
    (void)buffer;
    (void)len;
    (void)tx_bytes;
    CORD_LOG("[CordFlowPoint] tx()\n");
    return CORD_OK;
}

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id, size_t rx_buffer_size)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = CordFlowPoint_rx_,
        .tx = CordFlowPoint_tx_,
    };

    self->vptr = &vtbl;
    self->id = id;
    self->rx_buffer_size = rx_buffer_size;
}

void CordFlowPoint_dtor(CordFlowPoint * const self)
{
    free(self);
}
