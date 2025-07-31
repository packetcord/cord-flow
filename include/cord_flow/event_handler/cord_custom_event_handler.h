#ifndef CORD_CUSTOM_EVENT_HANDLER_H
#define CORD_CUSTOM_EVENT_HANDLER_H

#include <event_handler/cord_event_handler.h>

typedef struct CordCustomEventHandler
{
    CordEventHandler base;
    uint8_t nb_ports;
    void *params;
} CordCustomEventHandler;

void CordCustomEventHandler_ctor(CordCustomEventHandler * const self,
                                 uint8_t evh_id,
                                 void *params);

void CordCustomEventHandler_dtor(CordCustomEventHandler * const self);

#endif // CORD_CUSTOM_EVENT_HANDLER_H
