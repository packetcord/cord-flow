//
// Add _Generic compile time polymorphism
//

#include <event_handler/cord_event_handler.h>

void CordEventHandler_ctor(CordEventHandler * const self, uint8_t evh_id)
{
    self->evh_id = evh_id;
}

void CordEventHandler_dtor(CordEventHandler * const self)
{
    free(self);
}
