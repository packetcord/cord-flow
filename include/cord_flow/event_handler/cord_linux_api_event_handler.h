#ifndef CORD_LINUX_API_EVENT_HANDLER_H
#define CORD_LINUX_API_EVENT_HANDLER_H

#include <event_handler/cord_event_handler.h>
#include <sys/epoll.h>

#define CORD_MAX_NB_EVENTS 16

#define CORD_CREATE_LINUX_API_EVENT_HANDLER CORD_CREATE_LINUX_API_EVENT_HANDLER_ON_HEAP
#define CORD_DESTROY_LINUX_API_EVENT_HANDLER CORD_DESTROY_LINUX_API_EVENT_HANDLER_ON_HEAP

#define CORD_CREATE_LINUX_API_EVENT_HANDLER_ON_HEAP(id, timeout) \
    (CordEventHandler *) NEW_ON_HEAP(CordLinuxApiEventHandler, id, timeout)

#define CORD_CREATE_LINUX_API_EVENT_HANDLER_ON_STACK(id, timeout)\
    (CordEventHandler *) &NEW_ON_STACK(CordLinuxApiEventHandler, id, timeout)

#define CORD_DESTROY_LINUX_API_EVENT_HANDLER_ON_HEAP(name) \
    do {                                                   \
        DESTROY_ON_HEAP(CordEventHandler, name);           \
    } while(0)

#define CORD_DESTROY_LINUX_API_EVENT_HANDLER_ON_STACK(name)\
    do {                                                   \
        DESTROY_ON_STACK(CordEventHandler, name);          \
    } while(0)

typedef struct CordLinuxApiEventHandler
{
    CordEventHandler base;
    int evh_fd;
    struct epoll_event ev;
    struct epoll_event events[CORD_MAX_NB_EVENTS];
    int timeout;
} CordLinuxApiEventHandler;

void CordLinuxApiEventHandler_ctor(CordLinuxApiEventHandler * const self,
                                   uint8_t evh_id,
                                   int timeout);                                
void CordLinuxApiEventHandler_dtor(CordLinuxApiEventHandler * const self);

#endif // CORD_LINUX_API_EVENT_HANDLER_H
