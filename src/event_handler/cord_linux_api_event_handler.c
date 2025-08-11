#include <event_handler/cord_linux_api_event_handler.h>
#include <cord_error.h>

static cord_retval_t CordLinuxApiEventHandler_register_flow_point_(CordLinuxApiEventHandler * const self, CordFlowPoint *fp)
{
    CORD_LOG("[CordLinuxApiEventHandler] register_flow_point()\n");

    self->base.ev.events = EPOLLIN;
    self->base.ev.data.fd = fp->io_handle;

    if (epoll_ctl(self->base.evh_fd, EPOLL_CTL_ADD, self->base.ev.data.fd, &(self->base.ev)) == -1)
    {
        CORD_ERROR("[CordLinuxApiEventHandler] epoll_ctl(EPOLL_CTL_ADD)");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->base.nb_registered_fps += 1;

    return CORD_OK;
}

static int CordLinuxApiEventHandler_wait_(CordLinuxApiEventHandler * const self)
{
    CORD_LOG("[CordLinuxApiEventHandler] wait()\n");
    return epoll_wait(self->base.evh_fd, self->base.events, self->base.nb_registered_fps, self->timeout);
}

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   int timeout)
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
    self->base.evh_fd = epoll_create1(epoll_create_flags);

    if (self->base.evh_fd == -1)
    {
        CORD_ERROR("[CordLinuxApiEventHandler] epoll_create1()");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self)
{
    CORD_LOG("[CordLinuxApiEventHandler] dtor()\n");
    close(self->base.evh_fd);
    free(self);
}
