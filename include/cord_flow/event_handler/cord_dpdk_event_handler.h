#ifndef CORD_DPDK_EVENT_HANDLER_H
#define CORD_DPDK_EVENT_HANDLER_H

#ifdef ENABLE_DPDK_DATAPLANE

#include <event_handler/cord_event_handler.h>

typedef struct CordDpdkEventHandler
{
    CordEventHandler base;
    uint8_t nb_registered_ports;
    void *params;
} CordDpdkEventHandler;

void CordDpdkEventHandler_ctor(CordDpdkEventHandler * const self,
                               uint8_t evh_id,
                               uint8_t nb_ports,
                               void *params);

void CordDpdkEventHandler_dtor(CordDpdkEventHandler * const self);

#endif // ENABLE_DPDK_DATAPLANE 

#endif // CORD_DPDK_EVENT_HANDLER_H
