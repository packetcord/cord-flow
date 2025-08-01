#include <flow_point/cord_l3_stack_inject_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordL3StackInjectFlowPoint_rx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL3StackInjectFlowPoint *l3_stack_inj_fp = (const CordL3StackInjectFlowPoint *)self;
    
    //
    //  Pass
    //

    return CORD_OK;
}

static cord_retval_t CordL3StackInjectFlowPoint_tx_(CordFlowPoint const * const self, void *buffer)
{
    const CordL3StackInjectFlowPoint *l3_stack_inj_fp = (const CordL3StackInjectFlowPoint *)self;

    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id,
                                     size_t rx_buffer_size,
                                     int fd)
{
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL3StackInjectFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer))&CordL3StackInjectFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id, rx_buffer_size);
    self->base.vptr = &vtbl;

    self->fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (self->fd < 0)
    {
        CORD_ERROR("CordL3StackInjectFlowPoint: socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(self->fd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("CordL3StackInjectFlowPoint: setsockopt(IPPROTO_IP, IP_HDRINCL)");
        CORD_EXIT(EXIT_FAILURE);
    }
}

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self)
{
    free(self);
}
