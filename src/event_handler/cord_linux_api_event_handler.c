#include <event_handler/cord_linux_api_event_handler.h>
#include <cord_error.h>

static cord_retval_t CordLinuxApiEventHandler_register_flow_point_(CordEventHandler * const base_self, void *fp_param)
{
    CORD_LOG("[CordLinuxApiEventHandler] register_flow_point()\n");
    CordLinuxApiEventHandler *self = (CordLinuxApiEventHandler *)base_self;

    self->ev.events = EPOLLIN;
    self->ev.data.fd = *((int *)fp_param);

    if (epoll_ctl(self->fd, EPOLL_CTL_ADD, self->ev.data.fd, &(self->ev)) == -1)
    {
        CORD_ERROR("CordLinuxApiEventHandler: epoll_ctl(EPOLL_CTL_ADD)");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->nb_registered_fds += 1;

    return CORD_OK;
}

static int CordLinuxApiEventHandler_wait_(CordLinuxApiEventHandler * const self)
{
    CORD_LOG("[CordLinuxApiEventHandler] wait()\n");
    return epoll_wait(self->fd, self->events, self->nb_registered_fds, self->timeout);
}

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   uint32_t timeout)
{
    CORD_LOG("[CordLinuxApiEventHandler] ctor()\n");
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler * const self, void *fp_param))&CordLinuxApiEventHandler_register_flow_point_,
        .wait = (int (*)(CordEventHandler * const self))&CordLinuxApiEventHandler_wait_,
    };

    CordEventHandler_ctor(&self->base, evh_id);

    self->base.vptr = &vtbl;
    self->base.register_flow_point = CordEventHandler_register_flow_point_impl;
    self->base.wait = CordEventHandler_wait_impl;
    
    self->nb_registered_fds = 0;
    self->timeout = timeout;

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
    CORD_LOG("[CordLinuxApiEventHandler] dtor()\n");
    close(self->fd);
    free(self);
}
