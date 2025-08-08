#include <event_handler/cord_linux_api_event_handler.h>
#include <cord_error.h>

static cord_retval_t CordLinuxApiEventHandler_register_flow_point_(CordEventHandler * const base_self, CordFlowPoint *fp)
{
    CORD_LOG("[CordLinuxApiEventHandler] register_flow_point()\n");
    CordLinuxApiEventHandler *self = (CordLinuxApiEventHandler *)base_self;

    self->ev.events = EPOLLIN;
    self->ev.data.fd = fp->io_handle;

    if (epoll_ctl(self->evh_fd, EPOLL_CTL_ADD, self->ev.data.fd, &(self->ev)) == -1)
    {
        CORD_ERROR("CordLinuxApiEventHandler: epoll_ctl(EPOLL_CTL_ADD)");
        CORD_EXIT(EXIT_FAILURE);
    }

    base_self->nb_registered_fps += 1;

    return CORD_OK;
}

static int CordLinuxApiEventHandler_wait_(CordLinuxApiEventHandler * const self)
{
    CORD_LOG("[CordLinuxApiEventHandler] wait()\n");
    return epoll_wait(self->evh_fd, self->events, self->base.nb_registered_fps, self->timeout);
}

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   uint32_t timeout)
{
    CORD_LOG("[CordLinuxApiEventHandler] ctor()\n");
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler * const self, CordFlowPoint *fp))&CordLinuxApiEventHandler_register_flow_point_,
        .wait = (int (*)(CordEventHandler * const self))&CordLinuxApiEventHandler_wait_,
    };

    CordEventHandler_ctor(&self->base, evh_id);

    self->base.vptr = &vtbl;
    self->timeout = timeout;

    int epoll_create_flags = 0;
    self->evh_fd = epoll_create1(epoll_create_flags);

    if (self->evh_fd == -1)
    {
        CORD_ERROR("CordLinuxApiEventHandler: epoll_create1()");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self)
{
    CORD_LOG("[CordLinuxApiEventHandler] dtor()\n");
    close(self->evh_fd);
    free(self);
}
