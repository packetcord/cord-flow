#ifndef CORD_LINUX_API_EVENT_HANDLER_H
#define CORD_LINUX_API_EVENT_HANDLER_H

#include <event_handler/cord_event_handler.h>
#include <sys/epoll.h>

#define CORD_MAX_NB_EVENTS 16

typedef struct CordLinuxApiEventHandler
{
    CordEventHandler base;
    int evh_fd;
    struct epoll_event ev;
    struct epoll_event events[CORD_MAX_NB_EVENTS];
    uint32_t timeout;
    void *params;
} CordLinuxApiEventHandler;

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   uint32_t timeout);                                
void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self);

#endif // CORD_LINUX_API_EVENT_HANDLER_H
