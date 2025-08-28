#ifndef CORD_SDR_FLOW_POINT_H
#define CORD_SDR_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_SDR_FLOW_POINT CORD_CREATE_SDR_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_SDR_FLOW_POINT CORD_DESTROY_SDR_FLOW_POINT_ON_HEAP

#define CORD_CREATE_SDR_FLOW_POINT_ON_HEAP(id, device_name, channel, modulation) \
    (CordFlowPoint *) NEW_ON_HEAP(CordSdrFlowPoint, id, device_name, channel, modulation)

#define CORD_CREATE_SDR_FLOW_POINT_ON_STACK(id, device_name, channel, modulation)\
    (CordFlowPoint *) &NEW_ON_STACK(CordSdrFlowPoint, id, device_name, channel, modulation)

#define CORD_DESTROY_SDR_FLOW_POINT_ON_HEAP(name) \
    do {                                           \
        DESTROY_ON_HEAP(CordSdrFlowPoint, name);   \
    } while(0)

#define CORD_DESTROY_SDR_FLOW_POINT_ON_STACK(name)\
    do {                                          \
        DESTROY_ON_STACK(CordSdrFlowPoint, name); \
    } while(0)

typedef enum
{
    CORD_SDR_MODULATION_AM,
    CORD_SDR_MODULATION_FM,
    CORD_SDR_MODULATION_BPSK,
    CORD_SDR_MODULATION_QPSK,
    CORD_SDR_MODULATION_8PSK,
    CORD_SDR_MODULATION_16QAM,
    CORD_SDR_MODULATION_64QAM,
    CORD_SDR_MODULATION_256QAM,
    CORD_SDR_MODULATION_OFDM
} CordSdrModulationType;

typedef struct CordSdrFlowPoint
{
    CordFlowPoint base;
    char *device_name;
    uint32_t channel;
    uint64_t frequency_hz;
    uint32_t sample_rate;
    uint32_t bandwidth_hz;
    CordSdrModulationType modulation;
    float gain_db;
    bool tx_enabled;
    bool rx_enabled;
    void *sdr_handle;
    void *params;
} CordSdrFlowPoint;

void CordSdrFlowPoint_ctor(CordSdrFlowPoint * const self,
                           uint8_t id,
                           const char *device_name,
                           uint32_t channel,
                           CordSdrModulationType modulation);

void CordSdrFlowPoint_dtor(CordSdrFlowPoint * const self);

cord_retval_t CordSdrFlowPoint_set_channel(CordSdrFlowPoint * const self, uint32_t channel);
cord_retval_t CordSdrFlowPoint_set_modulation(CordSdrFlowPoint * const self, CordSdrModulationType modulation);
cord_retval_t CordSdrFlowPoint_set_frequency(CordSdrFlowPoint * const self, uint64_t frequency_hz);
cord_retval_t CordSdrFlowPoint_set_sample_rate(CordSdrFlowPoint * const self, uint32_t sample_rate);
cord_retval_t CordSdrFlowPoint_set_bandwidth(CordSdrFlowPoint * const self, uint32_t bandwidth_hz);
cord_retval_t CordSdrFlowPoint_set_gain(CordSdrFlowPoint * const self, float gain_db);
cord_retval_t CordSdrFlowPoint_enable_tx(CordSdrFlowPoint * const self, bool enable);
cord_retval_t CordSdrFlowPoint_enable_rx(CordSdrFlowPoint * const self, bool enable);

uint32_t CordSdrFlowPoint_get_channel(const CordSdrFlowPoint * const self);
CordSdrModulationType CordSdrFlowPoint_get_modulation(const CordSdrFlowPoint * const self);
uint64_t CordSdrFlowPoint_get_frequency(const CordSdrFlowPoint * const self);
uint32_t CordSdrFlowPoint_get_sample_rate(const CordSdrFlowPoint * const self);
uint32_t CordSdrFlowPoint_get_bandwidth(const CordSdrFlowPoint * const self);
float CordSdrFlowPoint_get_gain(const CordSdrFlowPoint * const self);

#endif // CORD_SDR_FLOW_POINT_H