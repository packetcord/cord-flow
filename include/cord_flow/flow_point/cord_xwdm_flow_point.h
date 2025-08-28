#ifndef CORD_XWDM_FLOW_POINT_H
#define CORD_XWDM_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_XWDM_FLOW_POINT CORD_CREATE_XWDM_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_XWDM_FLOW_POINT CORD_DESTROY_XWDM_FLOW_POINT_ON_HEAP

#define CORD_CREATE_XWDM_FLOW_POINT_ON_HEAP(id, wavelength_nm) \
    (CordFlowPoint *) NEW_ON_HEAP(CordXwdmFlowPoint, id, wavelength_nm)

#define CORD_CREATE_XWDM_FLOW_POINT_ON_STACK(id, wavelength_nm)\
    (CordFlowPoint *) &NEW_ON_STACK(CordXwdmFlowPoint, id, wavelength_nm)

#define CORD_DESTROY_XWDM_FLOW_POINT_ON_HEAP(name) \
    do {                                           \
        DESTROY_ON_HEAP(CordXwdmFlowPoint, name);  \
    } while(0)

#define CORD_DESTROY_XWDM_FLOW_POINT_ON_STACK(name)\
    do {                                           \
        DESTROY_ON_STACK(CordXwdmFlowPoint, name); \
    } while(0)

typedef enum
{
    CORD_XWDM_DWDM_C_BAND,
    CORD_XWDM_DWDM_L_BAND,
    CORD_XWDM_CWDM,
    CORD_XWDM_LWDM,
    CORD_XWDM_CUSTOM
} CordXwdmBandType;

typedef enum
{
    CORD_XWDM_MODULATION_NRZ,
    CORD_XWDM_MODULATION_PAM4,
    CORD_XWDM_MODULATION_QPSK,
    CORD_XWDM_MODULATION_8QAM,
    CORD_XWDM_MODULATION_16QAM,
    CORD_XWDM_MODULATION_64QAM,
    CORD_XWDM_MODULATION_OFDM
} CordXwdmModulationType;

typedef struct CordXwdmFlowPoint
{
    CordFlowPoint base;
    float wavelength_nm;
    uint32_t channel_number;
    float frequency_thz;
    CordXwdmBandType band_type;
    CordXwdmModulationType modulation;
    float channel_spacing_ghz;
    float power_dbm;
    float osnr_db;
    uint32_t bit_rate_gbps;
    bool optical_amplifier_enabled;
    bool fec_enabled;
    char *transponder_type;
    void *optical_params;
} CordXwdmFlowPoint;

void CordXwdmFlowPoint_ctor(CordXwdmFlowPoint * const self,
                            uint8_t id,
                            float wavelength_nm);

void CordXwdmFlowPoint_dtor(CordXwdmFlowPoint * const self);

cord_retval_t CordXwdmFlowPoint_set_wavelength(CordXwdmFlowPoint * const self, float wavelength_nm);
cord_retval_t CordXwdmFlowPoint_set_channel_number(CordXwdmFlowPoint * const self, uint32_t channel_number);
cord_retval_t CordXwdmFlowPoint_set_band_type(CordXwdmFlowPoint * const self, CordXwdmBandType band_type);
cord_retval_t CordXwdmFlowPoint_set_modulation(CordXwdmFlowPoint * const self, CordXwdmModulationType modulation);
cord_retval_t CordXwdmFlowPoint_set_channel_spacing(CordXwdmFlowPoint * const self, float channel_spacing_ghz);
cord_retval_t CordXwdmFlowPoint_set_power(CordXwdmFlowPoint * const self, float power_dbm);
cord_retval_t CordXwdmFlowPoint_set_osnr(CordXwdmFlowPoint * const self, float osnr_db);
cord_retval_t CordXwdmFlowPoint_set_bit_rate(CordXwdmFlowPoint * const self, uint32_t bit_rate_gbps);
cord_retval_t CordXwdmFlowPoint_enable_optical_amplifier(CordXwdmFlowPoint * const self, bool enable);
cord_retval_t CordXwdmFlowPoint_enable_fec(CordXwdmFlowPoint * const self, bool enable);
cord_retval_t CordXwdmFlowPoint_set_transponder_type(CordXwdmFlowPoint * const self, const char *transponder_type);

float CordXwdmFlowPoint_get_wavelength(const CordXwdmFlowPoint * const self);
uint32_t CordXwdmFlowPoint_get_channel_number(const CordXwdmFlowPoint * const self);
float CordXwdmFlowPoint_get_frequency(const CordXwdmFlowPoint * const self);
CordXwdmBandType CordXwdmFlowPoint_get_band_type(const CordXwdmFlowPoint * const self);
CordXwdmModulationType CordXwdmFlowPoint_get_modulation(const CordXwdmFlowPoint * const self);
float CordXwdmFlowPoint_get_channel_spacing(const CordXwdmFlowPoint * const self);
float CordXwdmFlowPoint_get_power(const CordXwdmFlowPoint * const self);
float CordXwdmFlowPoint_get_osnr(const CordXwdmFlowPoint * const self);
uint32_t CordXwdmFlowPoint_get_bit_rate(const CordXwdmFlowPoint * const self);

#endif // CORD_XWDM_FLOW_POINT_H