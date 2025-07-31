#include <event_handler/cord_custom_event_handler.h>

void CordCustomEventHandler_ctor(CordCustomEventHandler * const self,
                                 uint8_t evh_id,
                                 void *params)
{
    CordEventHandler_ctor(&self->base, evh_id);
    self->params = params;
}

void CordCustomEventHandler_dtor(CordCustomEventHandler * const self)
{
    free(self);
}
