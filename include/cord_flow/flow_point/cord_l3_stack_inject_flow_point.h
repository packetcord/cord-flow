#ifndef CORD_L3_STACK_INJECT_FLOW_POINT_H
#define CORD_L3_STACK_INJECT_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL3StackInjectFlowPoint
{
    CordFlowPoint base;
    int fd;
} CordL3StackInjectFlowPoint;

void CordL3StackInjectFlowPoint_ctor(CordL3StackInjectFlowPoint * const self,
                                     uint8_t id,
                                     size_t rx_buffer_size,
                                     int fd);

void CordL3StackInjectFlowPoint_dtor(CordL3StackInjectFlowPoint * const self);

#endif // CORD_L3_STACK_INJECT_FLOW_POINT_H
