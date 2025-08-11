#include <flow_point/cord_l2_custom_flow_point.h>

static cord_retval_t CordL2CustomFlowPoint_rx_(CordL2CustomFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes)
{
    CORD_LOG("[CordL2CustomFlowPoint] rx()\n");    
    //
    // Implement the custom rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL2CustomFlowPoint_tx_(CordL2CustomFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes)
{
    CORD_LOG("[CordL2CustomFlowPoint] tx()\n");
    //
    // Implement the custom tx() logic
    //

    return CORD_OK;
}

void CordL2CustomFlowPoint_ctor(CordL2CustomFlowPoint * const self,
                                uint8_t id,
                                size_t rx_buffer_size,
                                void *params)
{
    CORD_LOG("[CordL2CustomFlowPoint] ctor()\n");
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *rx_bytes))&CordL2CustomFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, ssize_t len, ssize_t *tx_bytes))&CordL2CustomFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;
    self->params = params;
}

void CordL2CustomFlowPoint_dtor(CordL2CustomFlowPoint * const self)
{
    CORD_LOG("[CordL2CustomFlowPoint] dtor()\n");
    free(self);
}
