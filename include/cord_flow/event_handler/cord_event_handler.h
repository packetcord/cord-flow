//
// Add _Generic compile time polymorphism
//

#ifndef CORD_EVENT_HANDLER_H
#define CORD_EVENT_HANDLER_H

#include <cord_type.h>
#include <cord_retval.h>
#include <flow_point/cord_flow_point.h>

typedef struct CordEventHandler CordEventHandler;

typedef struct
{
    cord_retval_t (*register_flow_point)(CordEventHandler * const self, void *fp_param);
} CordEventHandlerVtbl;

struct CordEventHandler
{
    const CordEventHandlerVtbl *vptr;
    uint8_t evh_id;
    uint8_t nb_registered_fps;
};

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id);
void CordEventHandler_dtor(CordEventHandler * const self);

#define CORDEVENTHANDLER_REGISTER_FLOW_POINT_VCALL(self, fp_param)   (*(self->vptr->register_flow_point))((self), (fp_param))

static inline cord_retval_t CordEventHandler_register_flow_point_vcall(CordEventHandler * const self, void *fp_param)
{
    return (*(self->vptr->register_flow_point))(self, fp_param);
}

#endif // CORD_EVENT_HANDLER_H
