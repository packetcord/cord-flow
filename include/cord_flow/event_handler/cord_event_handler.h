//
// Add _Generic compile time polymorphism
//

#ifndef CORD_EVENT_HANDLER_H
#define CORD_EVENT_HANDLER_H

#include <cord_type.h>
#include <cord_retval.h>

typedef struct CordEventHandler CordEventHandler;

struct CordEventHandler
{
    uint8_t evh_id;
};

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id);
void CordEventHandler_dtor(CordEventHandler * const self);

#endif // CORD_EVENT_HANDLER_H
