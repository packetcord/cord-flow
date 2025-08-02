//
// Add _Generic compile time polymorphism
//

#include <event_handler/cord_event_handler.h>

static cord_retval_t CordEventHandler_register_flow_point_(CordEventHandler * const self, void *fp_param)
{
    (void)self;
    (void)fp_param;
    CORD_LOG("[CordEventHandler] register_flow_point()\n");
    return CORD_OK;
}

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = CordEventHandler_register_flow_point_,
    };

    self->vptr                = &vtbl;
    self->register_flow_point = CordEventHandler_register_flow_point_impl;
    self->evh_id              = evh_id;
    self->nb_registered_fps   = 0;
}

void CordEventHandler_dtor(CordEventHandler * const self)
{
    free(self);
}
