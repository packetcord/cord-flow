#include <flow_point/cord_sdr_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordSdrFlowPoint_rx_(CordSdrFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] rx()\n");
#endif
    
    if (!self->rx_enabled) {
        *rx_bytes = 0;
        return CORD_OK;
    }

    *rx_bytes = len;
    return CORD_OK;
}

static cord_retval_t CordSdrFlowPoint_tx_(CordSdrFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] tx()\n");
#endif
    
    if (!self->tx_enabled) {
        *tx_bytes = 0;
        return CORD_OK;
    }

    *tx_bytes = len;
    return CORD_OK;
}

void CordSdrFlowPoint_ctor(CordSdrFlowPoint * const self,
                           uint8_t id,
                           const char *device_name,
                           uint32_t channel,
                           CordSdrModulationType modulation)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordSdrFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordSdrFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    
    self->device_name = malloc(strlen(device_name) + 1);
    if (self->device_name) {
        strcpy(self->device_name, device_name);
    }
    
    self->channel = channel;
    self->frequency_hz = 2400000000ULL + (channel * 5000000ULL);
    self->sample_rate = 2000000;
    self->bandwidth_hz = 2000000;
    self->modulation = modulation;
    self->gain_db = 0.0f;
    self->tx_enabled = false;
    self->rx_enabled = false;
    self->sdr_handle = NULL;
    self->params = NULL;
    
    self->base.io_handle = -1;
}

void CordSdrFlowPoint_dtor(CordSdrFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] dtor()\n");
#endif
    if (self->device_name) {
        free(self->device_name);
    }
    free(self);
}

cord_retval_t CordSdrFlowPoint_set_channel(CordSdrFlowPoint * const self, uint32_t channel)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->channel = channel;
    self->frequency_hz = 2400000000ULL + (channel * 5000000ULL);
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Channel set to %u (freq: %lu Hz)\n", channel, self->frequency_hz);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_set_modulation(CordSdrFlowPoint * const self, CordSdrModulationType modulation)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->modulation = modulation;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Modulation set to %d\n", modulation);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_set_frequency(CordSdrFlowPoint * const self, uint64_t frequency_hz)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->frequency_hz = frequency_hz;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Frequency set to %lu Hz\n", frequency_hz);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_set_sample_rate(CordSdrFlowPoint * const self, uint32_t sample_rate)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->sample_rate = sample_rate;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Sample rate set to %u\n", sample_rate);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_set_bandwidth(CordSdrFlowPoint * const self, uint32_t bandwidth_hz)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->bandwidth_hz = bandwidth_hz;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Bandwidth set to %u Hz\n", bandwidth_hz);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_set_gain(CordSdrFlowPoint * const self, float gain_db)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->gain_db = gain_db;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] Gain set to %.2f dB\n", gain_db);
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_enable_tx(CordSdrFlowPoint * const self, bool enable)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->tx_enabled = enable;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] TX %s\n", enable ? "enabled" : "disabled");
#endif
    
    return CORD_OK;
}

cord_retval_t CordSdrFlowPoint_enable_rx(CordSdrFlowPoint * const self, bool enable)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->rx_enabled = enable;
    
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordSdrFlowPoint] RX %s\n", enable ? "enabled" : "disabled");
#endif
    
    return CORD_OK;
}

uint32_t CordSdrFlowPoint_get_channel(const CordSdrFlowPoint * const self)
{
    return self ? self->channel : 0;
}

CordSdrModulationType CordSdrFlowPoint_get_modulation(const CordSdrFlowPoint * const self)
{
    return self ? self->modulation : CORD_SDR_MODULATION_AM;
}

uint64_t CordSdrFlowPoint_get_frequency(const CordSdrFlowPoint * const self)
{
    return self ? self->frequency_hz : 0;
}

uint32_t CordSdrFlowPoint_get_sample_rate(const CordSdrFlowPoint * const self)
{
    return self ? self->sample_rate : 0;
}

uint32_t CordSdrFlowPoint_get_bandwidth(const CordSdrFlowPoint * const self)
{
    return self ? self->bandwidth_hz : 0;
}

float CordSdrFlowPoint_get_gain(const CordSdrFlowPoint * const self)
{
    return self ? self->gain_db : 0.0f;
}