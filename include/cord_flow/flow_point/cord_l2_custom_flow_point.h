#ifndef CORD_L2_CUSTOM_FLOW_POINT_H
#define CORD_L2_CUSTOM_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

typedef struct CordL2CustomFlowPoint
{
    CordFlowPoint base;
    int fd;
    void *params;
} CordL2CustomFlowPoint;

void CordL2CustomFlowPoint_ctor(CordL2CustomFlowPoint * const self,
                                uint8_t id,
                                size_t rx_buffer_size,
                                int fd,
                                void *params);

void CordL2CustomFlowPoint_dtor(CordL2CustomFlowPoint * const self);

#endif // CORD_L2_CUSTOM_FLOW_POINT_H
