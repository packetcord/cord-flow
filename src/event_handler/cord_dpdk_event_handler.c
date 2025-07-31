#ifdef ENABLE_DPDK_DATAPLANE

#include <event_handler/cord_dpdk_event_handler.h>

void CordDpdkEventHandler_ctor(CordDpdkEventHandler * const self,
                               uint8_t evh_id,
                               uint8_t nb_ports,
                               void *params)
{
    CordEventHandler_ctor(&self->base, evh_id);
    self->nb_ports = nb_ports;
    self->params = params;
}

void CordDpdkEventHandler_dtor(CordDpdkEventHandler * const self)
{
    free(self);
}

#endif
