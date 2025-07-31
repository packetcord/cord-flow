#include <event_handler/cord_linux_api_event_handler.h>

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   int fd,
                                   uint32_t timeout,
                                   void *params)
{
    CordEventHandler_ctor(&self->base, evh_id);
    self->fd = fd;
    self->timeout = timeout;
    self->params = params;
}

void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self)
{
    free(self);
}
