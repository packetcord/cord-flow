#include <event_handler/cord_custom_event_handler.h>

static cord_retval_t CordCustomEventHandler_register_flow_point_(CordEventHandler const * const self, CordFlowPoint *fp)
{
    //
    // Implement the logic
    //

    return CORD_OK;
}

void CordCustomEventHandler_ctor(CordCustomEventHandler * const self,
                                 uint8_t evh_id,
                                 void *params)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler const * const self, CordFlowPoint *fp))&CordCustomEventHandler_register_flow_point_,
    };
    
    CordEventHandler_ctor(&self->base, evh_id);
    self->params = params;
}

void CordCustomEventHandler_dtor(CordCustomEventHandler * const self)
{
    free(self);
}
