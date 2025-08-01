//
// Add _Generic compile time polymorphism
//

#include <event_handler/cord_event_handler.h>

static cord_retval_t CordEventHandler_register_flow_point_(CordEventHandler * const self, int fp_fd)
{
    (void)self;
    (void)fp_fd;
    CORD_LOG("[CordEventHandler] register_flow_point()\n");
    return CORD_OK;
}

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = CordEventHandler_register_flow_point_,
    };

    self->evh_id = evh_id;
}

void CordEventHandler_dtor(CordEventHandler * const self)
{
    free(self);
}
