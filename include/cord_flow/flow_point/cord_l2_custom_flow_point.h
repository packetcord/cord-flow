#ifndef CORD_L2_CUSTOM_FLOW_POINT_H
#define CORD_L2_CUSTOM_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_CUSTOM_FLOW_POINT CORD_CREATE_CUSTOM_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_CUSTOM_FLOW_POINT CORD_DESTROY_CUSTOM_FLOW_POINT_ON_HEAP

#define CORD_CREATE_CUSTOM_FLOW_POINT_ON_HEAP(id, rx_buffer_size, params) \
    (CordFlowPoint *) NEW_ON_HEAP(CordL2CustomFlowPoint, id, rx_buffer_size, params)

#define CORD_CREATE_CUSTOM_FLOW_POINT_ON_STACK(id, rx_buffer_size, params)\
    (CordFlowPoint *) &NEW_ON_STACK(CordL2CustomFlowPoint, id, rx_buffer_size, params)

#define CORD_DESTROY_CUSTOM_FLOW_POINT_ON_HEAP(name)  \
    do {                                              \
        DESTROY_ON_HEAP(CordL2CustomFlowPoint, name); \
    } while(0)

#define CORD_DESTROY_CUSTOM_FLOW_POINT_ON_STACK(name) \
    do {                                              \
        DESTROY_ON_STACK(CordL2CustomFlowPoint, name);\
    } while(0)

typedef struct CordL2CustomFlowPoint
{
    CordFlowPoint base;
    void *params;
} CordL2CustomFlowPoint;

void CordL2CustomFlowPoint_ctor(CordL2CustomFlowPoint * const self,
                                uint8_t id,
                                size_t rx_buffer_size,
                                void *params);

void CordL2CustomFlowPoint_dtor(CordL2CustomFlowPoint * const self);

#endif // CORD_L2_CUSTOM_FLOW_POINT_H
