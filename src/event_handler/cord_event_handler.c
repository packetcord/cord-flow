//
// Add _Generic compile time polymorphism
//

#include <event_handler/cord_event_handler.h>

static cord_retval_t CordEventHandler_register_flow_point_(CordEventHandler * const self, CordFlowPoint *fp)
{
    CORD_LOG("[CordEventHandler] register_flow_point()\n");
    (void)self;
    (void)fp;
    return CORD_OK;
}

static int CordEventHandler_wait_(CordEventHandler * const self)
{
    CORD_LOG("[CordEventHandler] wait()\n");
    (void)self;
    return 0;
}

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id)
{
    CORD_LOG("[CordEventHandler] ctor()\n");
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = CordEventHandler_register_flow_point_,
        .wait = CordEventHandler_wait_,
    };

    self->vptr = &vtbl;
    self->evh_id = evh_id;
    self->nb_registered_fps = 0;
}

void CordEventHandler_dtor(CordEventHandler * const self)
{
    CORD_LOG("[CordEventHandler] dtor()\n");
    close(self->evh_fd);
    free(self);
}
