#include <flow_point/cord_l2_custom_flow_point.h>

static cord_retval_t CordL2CustomFlowPoint_rx_(CordL2CustomFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2CustomFlowPoint] rx()\n");
#endif
    //
    // Implement the custom rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL2CustomFlowPoint_tx_(CordL2CustomFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2CustomFlowPoint] tx()\n");
#endif
    //
    // Implement the custom tx() logic
    //

    return CORD_OK;
}

void CordL2CustomFlowPoint_ctor(CordL2CustomFlowPoint * const self,
                                uint8_t id,
                                void *params)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2CustomFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordL2CustomFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordL2CustomFlowPoint_tx_,
        .cleanup = (void     (*)(CordFlowPoint const * const))&CordL2CustomFlowPoint_dtor,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->params = params;
}

void CordL2CustomFlowPoint_dtor(CordL2CustomFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2CustomFlowPoint] dtor()\n");
#endif
    free(self);
}
