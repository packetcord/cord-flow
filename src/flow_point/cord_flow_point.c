//
// Add _Generic compile time polymorphism
//

#include <flow_point/cord_flow_point.h>
#include <cord_retval.h>

static cord_retval_t CordFlowPoint_rx_(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordFlowPoint] rx()\n");
#endif
    (void)self;
    (void)buffer;
    (void)len;
    (void)rx_bytes;
    return CORD_OK;
}

static cord_retval_t CordFlowPoint_tx_(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordFlowPoint] tx()\n");
#endif
    (void)self;
    (void)buffer;
    (void)len;
    (void)tx_bytes;
    return CORD_OK;
}

void CordFlowPoint_ctor(CordFlowPoint * const self, uint8_t id)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = CordFlowPoint_rx_,
        .tx = CordFlowPoint_tx_,
        .cleanup = (void (*)(CordFlowPoint * const))&CordFlowPoint_dtor,
    };

    self->vptr = &vtbl;
    self->id = id;
}

void CordFlowPoint_dtor(CordFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordFlowPoint] dtor()\n");
#endif

    if (self && self->vptr && self->vptr->cleanup) {
        self->vptr->cleanup(self);
    } else {
        // Fallback if no cleanup defined
        free(self);
    }
}
