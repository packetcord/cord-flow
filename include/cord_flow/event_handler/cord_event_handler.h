//
// Add _Generic compile time polymorphism
//

#ifndef CORD_EVENT_HANDLER_H
#define CORD_EVENT_HANDLER_H

#include <cord_type.h>
#include <cord_retval.h>
#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_EVENT_HANDLER CORD_CREATE_EVENT_HANDLER_ON_HEAP
#define CORD_DESTROY_EVENT_HANDLER CORD_DESTROY_EVENT_HANDLER_ON_HEAP

#define CORD_CREATE_EVENT_HANDLER_ON_HEAP(id) \
    (CordEventHandler *) NEW_ON_HEAP(CordEventHandler, id)

#define CORD_CREATE_EVENT_HANDLER_ON_STACK(id) \
    (CordEventHandler *) &NEW_ON_STACK(CordEventHandler, id)

#define CORD_DESTROY_EVENT_HANDLER_ON_HEAP(name) \
    do {                                         \
        DESTROY_ON_HEAP(CordEventHandler, name); \
    } while(0)

#define CORD_DESTROY_EVENT_HANDLER_ON_STACK(name)\
    do {                                         \
        DESTROY_ON_STACK(CordEventHandler, name);\
    } while(0)

typedef struct CordEventHandler CordEventHandler;

typedef struct
{
    cord_retval_t (*register_flow_point)(CordEventHandler * const self, CordFlowPoint *fp);
    int (*wait)(CordEventHandler * const self);
} CordEventHandlerVtbl;

struct CordEventHandler
{
    const CordEventHandlerVtbl *vptr;
    uint8_t evh_id;
    uint8_t nb_registered_fps;
};

static inline cord_retval_t CordEventHandler_register_flow_point_vcall(CordEventHandler * const self, CordFlowPoint *fp)
{
    return (*(self->vptr->register_flow_point))(self, fp);
}

static inline int CordEventHandler_wait_vcall(CordEventHandler * const self)
{
    return (*(self->vptr->wait))(self);
}

#define CORDEVENTHANDLER_REGISTER_FLOW_POINT_VCALL(self, fp)   (*(self->vptr->register_flow_point))((self), (fp))
#define CORDEVENTHANDLER_WAIT_VCALL(self)   (*(self->vptr->wait))((self));

#define CORDEVENTHANDLER_REGISTER_FLOW_POINT   CORDEVENTHANDLER_REGISTER_FLOW_POINT_VCALL
#define CORDEVENTHANDLER_WAIT   CORDEVENTHANDLER_WAIT_VCALL

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id);
void CordEventHandler_dtor(CordEventHandler * const self);

#endif // CORD_EVENT_HANDLER_H
