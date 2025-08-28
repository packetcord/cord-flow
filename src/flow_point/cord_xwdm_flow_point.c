#include <flow_point/cord_xwdm_flow_point.h>
#include <cord_error.h>

static float wavelength_to_frequency_thz(float wavelength_nm)
{
    return 299792458.0f / (wavelength_nm * 1e-9) / 1e12;
}

static uint32_t wavelength_to_dwdm_channel(float wavelength_nm)
{
    const float ref_wavelength = 1530.33f;
    const float channel_spacing = 0.8f;
    
    return (uint32_t)((wavelength_nm - ref_wavelength) / channel_spacing) + 1;
}

static cord_retval_t CordXwdmFlowPoint_rx_(CordXwdmFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] rx() on wavelength %.2f nm\n", self->wavelength_nm);
#endif
    
    *rx_bytes = len;
    return CORD_OK;
}

static cord_retval_t CordXwdmFlowPoint_tx_(CordXwdmFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] tx() on wavelength %.2f nm\n", self->wavelength_nm);
#endif
    
    *tx_bytes = len;
    return CORD_OK;
}

void CordXwdmFlowPoint_ctor(CordXwdmFlowPoint * const self,
                            uint8_t id,
                            float wavelength_nm)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordXwdmFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordXwdmFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    
    self->wavelength_nm = wavelength_nm;
    self->frequency_thz = wavelength_to_frequency_thz(wavelength_nm);
    self->channel_number = wavelength_to_dwdm_channel(wavelength_nm);
    
    if (wavelength_nm >= 1530.0f && wavelength_nm <= 1565.0f) {
        self->band_type = CORD_XWDM_DWDM_C_BAND;
        self->channel_spacing_ghz = 100.0f;
    } else if (wavelength_nm >= 1565.0f && wavelength_nm <= 1625.0f) {
        self->band_type = CORD_XWDM_DWDM_L_BAND;
        self->channel_spacing_ghz = 100.0f;
    } else if (wavelength_nm >= 1270.0f && wavelength_nm <= 1610.0f) {
        self->band_type = CORD_XWDM_CWDM;
        self->channel_spacing_ghz = 2500.0f;
    } else {
        self->band_type = CORD_XWDM_CUSTOM;
        self->channel_spacing_ghz = 50.0f;
    }
    
    self->modulation = CORD_XWDM_MODULATION_NRZ;
    self->power_dbm = 0.0f;
    self->osnr_db = 20.0f;
    self->bit_rate_gbps = 10;
    self->optical_amplifier_enabled = false;
    self->fec_enabled = true;
    self->transponder_type = NULL;
    self->optical_params = NULL;
    
    self->base.io_handle = -1;
}

void CordXwdmFlowPoint_dtor(CordXwdmFlowPoint * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] dtor()\n");
#endif
    if (self->transponder_type) {
        free(self->transponder_type);
    }
    free(self);
}

cord_retval_t CordXwdmFlowPoint_set_wavelength(CordXwdmFlowPoint * const self, float wavelength_nm)
{
    if (!self) return CORD_ERR_INVALID;
    if (wavelength_nm <= 0.0f) return CORD_ERR_INVALID;
    
    self->wavelength_nm = wavelength_nm;
    self->frequency_thz = wavelength_to_frequency_thz(wavelength_nm);
    self->channel_number = wavelength_to_dwdm_channel(wavelength_nm);
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Wavelength set to %.2f nm (%.3f THz, Ch %u)\n", 
             wavelength_nm, self->frequency_thz, self->channel_number);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_channel_number(CordXwdmFlowPoint * const self, uint32_t channel_number)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->channel_number = channel_number;
    self->wavelength_nm = 1530.33f + ((channel_number - 1) * 0.8f);
    self->frequency_thz = wavelength_to_frequency_thz(self->wavelength_nm);
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Channel set to %u (%.2f nm)\n", channel_number, self->wavelength_nm);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_band_type(CordXwdmFlowPoint * const self, CordXwdmBandType band_type)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->band_type = band_type;
    
    switch (band_type) {
        case CORD_XWDM_DWDM_C_BAND:
        case CORD_XWDM_DWDM_L_BAND:
            self->channel_spacing_ghz = 100.0f;
            break;
        case CORD_XWDM_CWDM:
            self->channel_spacing_ghz = 2500.0f;
            break;
        case CORD_XWDM_LWDM:
            self->channel_spacing_ghz = 800.0f;
            break;
        default:
            self->channel_spacing_ghz = 50.0f;
            break;
    }
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Band type set to %d\n", band_type);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_modulation(CordXwdmFlowPoint * const self, CordXwdmModulationType modulation)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->modulation = modulation;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Modulation set to %d\n", modulation);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_channel_spacing(CordXwdmFlowPoint * const self, float channel_spacing_ghz)
{
    if (!self) return CORD_ERR_INVALID;
    if (channel_spacing_ghz <= 0.0f) return CORD_ERR_INVALID;
    
    self->channel_spacing_ghz = channel_spacing_ghz;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Channel spacing set to %.1f GHz\n", channel_spacing_ghz);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_power(CordXwdmFlowPoint * const self, float power_dbm)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->power_dbm = power_dbm;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Power set to %.2f dBm\n", power_dbm);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_osnr(CordXwdmFlowPoint * const self, float osnr_db)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->osnr_db = osnr_db;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] OSNR set to %.2f dB\n", osnr_db);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_bit_rate(CordXwdmFlowPoint * const self, uint32_t bit_rate_gbps)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->bit_rate_gbps = bit_rate_gbps;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Bit rate set to %u Gbps\n", bit_rate_gbps);
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_enable_optical_amplifier(CordXwdmFlowPoint * const self, bool enable)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->optical_amplifier_enabled = enable;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Optical amplifier %s\n", enable ? "enabled" : "disabled");
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_enable_fec(CordXwdmFlowPoint * const self, bool enable)
{
    if (!self) return CORD_ERR_INVALID;
    
    self->fec_enabled = enable;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] FEC %s\n", enable ? "enabled" : "disabled");
#endif
    
    return CORD_OK;
}

cord_retval_t CordXwdmFlowPoint_set_transponder_type(CordXwdmFlowPoint * const self, const char *transponder_type)
{
    if (!self || !transponder_type) return CORD_ERR_INVALID;
    
    if (self->transponder_type) {
        free(self->transponder_type);
    }
    
    self->transponder_type = malloc(strlen(transponder_type) + 1);
    if (!self->transponder_type) {
        return CORD_ERR_NO_MEMORY;
    }
    
    strcpy(self->transponder_type, transponder_type);
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordXwdmFlowPoint] Transponder type set to %s\n", transponder_type);
#endif
    
    return CORD_OK;
}

float CordXwdmFlowPoint_get_wavelength(const CordXwdmFlowPoint * const self)
{
    return self ? self->wavelength_nm : 0.0f;
}

uint32_t CordXwdmFlowPoint_get_channel_number(const CordXwdmFlowPoint * const self)
{
    return self ? self->channel_number : 0;
}

float CordXwdmFlowPoint_get_frequency(const CordXwdmFlowPoint * const self)
{
    return self ? self->frequency_thz : 0.0f;
}

CordXwdmBandType CordXwdmFlowPoint_get_band_type(const CordXwdmFlowPoint * const self)
{
    return self ? self->band_type : CORD_XWDM_CUSTOM;
}

CordXwdmModulationType CordXwdmFlowPoint_get_modulation(const CordXwdmFlowPoint * const self)
{
    return self ? self->modulation : CORD_XWDM_MODULATION_NRZ;
}

float CordXwdmFlowPoint_get_channel_spacing(const CordXwdmFlowPoint * const self)
{
    return self ? self->channel_spacing_ghz : 0.0f;
}

float CordXwdmFlowPoint_get_power(const CordXwdmFlowPoint * const self)
{
    return self ? self->power_dbm : 0.0f;
}

float CordXwdmFlowPoint_get_osnr(const CordXwdmFlowPoint * const self)
{
    return self ? self->osnr_db : 0.0f;
}

uint32_t CordXwdmFlowPoint_get_bit_rate(const CordXwdmFlowPoint * const self)
{
    return self ? self->bit_rate_gbps : 0;
}