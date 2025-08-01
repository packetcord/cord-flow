#ifdef ENABLE_DPDK_DATAPLANE

#include <event_handler/cord_dpdk_event_handler.h>

static cord_retval_t CordDpdkEventHandler_register_flow_point_(CordDpdkEventHandler * const self, int fp_fd)
{
    //
    // Implement the logic - may need to switch to (void *)param instead of int fp_fd
    //
    
    return CORD_OK;
}

void CordDpdkEventHandler_ctor(CordDpdkEventHandler * const self,
                               uint8_t evh_id,
                               uint8_t nb_ports,
                               void *params)
{
    static const CordEventHandlerVtbl vtbl = {
        .register_flow_point = (cord_retval_t (*)(CordEventHandler * const self, int fp_fd))&CordDpdkEventHandler_register_flow_point_,
    };
    
    CordEventHandler_ctor(&self->base, evh_id);
    self->base.vptr = &vtbl;
    self->nb_registered_ports = nb_registered_ports;
    self->params = params;
}

void CordDpdkEventHandler_dtor(CordDpdkEventHandler * const self)
{
    free(self);
}

#endif
