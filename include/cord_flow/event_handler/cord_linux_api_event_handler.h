#ifndef CORD_LINUX_API_EVENT_HANDLER_H
#define CORD_LINUX_API_EVENT_HANDLER_H

#include <event_handler/cord_event_handler.h>
#include <sys/epoll.h>

#define CORD_MAX_NB_EVENTS 16

typedef struct CordLinuxApiEventHandler
{
    CordEventHandler base;
    int (*wait)(struct CordLinuxApiEventHandler * const self);
    int fd;
    struct epoll_event ev;
    struct epoll_event events[CORD_MAX_NB_EVENTS];
    int nb_registered_fds;
    uint32_t timeout;
    void *params;
} CordLinuxApiEventHandler;

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   int fd,
                                   uint32_t timeout,
                                   void *params);

void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self);

#define CORDLINUXAPIEVENTHANDLER_WAIT_VCALL(self)   (*(self->wait))((self))

static inline cord_retval_t CordLinuxApiEventHandler_wait_vcall(CordLinuxApiEventHandler * const self)
{
    return (*(self->wait))(self);
}

#endif // CORD_LINUX_API_EVENT_HANDLER_H
