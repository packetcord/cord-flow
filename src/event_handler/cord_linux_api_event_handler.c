#include <event_handler/cord_linux_api_event_handler.h>
#include <cord_error.h>

static cord_retval_t CordLinuxApiEventHandler_register_flow_point_(CordLinuxApiEventHandler * const self, void *fp_param)
{
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
    return epoll_wait(self->fd, self->events, self->nb_registered_fds, self->timeout);
}

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   uint32_t timeout)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler * const self, void *fp_param))&CordLinuxApiEventHandler_register_flow_point_,
    };

    CordEventHandler_ctor(&self->base, evh_id);

    self->base.vptr = &vtbl;
    self->base.register_flow_point = CordEventHandler_register_flow_point_impl;

    self->wait = &CordLinuxApiEventHandler_wait_;
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
    close(self->fd);
    free(self);
}
