#include <event_handler/cord_linux_api_event_handler.h>
#include <cord_error.h>

static cord_retval_t CordLinuxApiEventHandler_register_flow_point_(CordLinuxApiEventHandler const * const self, CordFlowPoint *fp)
{
    //
    // Implement the logic
    //

    return CORD_OK;
}

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   int fd,
                                   uint32_t timeout,
                                   void *params)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler const * const self, CordFlowPoint *fp))&CordLinuxApiEventHandler_register_flow_point_,
    };

    CordEventHandler_ctor(&self->base, evh_id);
    self->timeout = timeout;
    self->params = params;

    int epoll_create_flags = 0;
    self->fd = epoll_create1(epoll_create_flags);

    if (self->fd == -1)
    {
        CORD_ERROR("CordLinuxApiEventHandler: epoll_create1()");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self)
{
    free(self);
}
