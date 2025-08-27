#ifndef CORD_3GPP_PROTOCOLS_H
#define CORD_3GPP_PROTOCOLS_H

#include "cord_protocol_common.h"

// GTP-C (GPRS Tunneling Protocol Control Plane) header
typedef struct CORD_PACKED cord_gtpc_hdr {
    uint8_t flags;              // Version:3, PT:1, Reserved:1, E:1, S:1, PN:1
    uint8_t message_type;       // Message Type
    uint16_t length;            // Message Length
    uint32_t teid;              // Tunnel Endpoint Identifier
    uint16_t sequence_number;   // Sequence Number
    uint8_t n_pdu_number;       // N-PDU Number
    uint8_t next_ext_hdr_type;  // Next Extension Header Type
} cord_gtpc_hdr_t;

// GTP-U (already defined in tunneling, but include key fields here for completeness)
// See cord_gtpu_hdr_t in cord_tunneling_protocols.h

// SCTP M3UA (MTP3 User Adaptation) header
typedef struct CORD_PACKED cord_m3ua_hdr {
    uint8_t version;            // Version
    uint8_t reserved;           // Reserved
    uint8_t msg_class;          // Message Class
    uint8_t msg_type;           // Message Type
    uint32_t msg_length;        // Message Length
    // Followed by parameters
} cord_m3ua_hdr_t;

// SCTP Parameter header
typedef struct CORD_PACKED cord_sctp_param {
    uint16_t tag;               // Parameter Tag
    uint16_t length;            // Parameter Length
    uint8_t value[];            // Parameter Value
} cord_sctp_param_t;

// Diameter 3GPP specific AVPs (Attribute-Value Pairs)
// Note: Uses standard DIAMETER header from VoIP protocols
typedef struct CORD_PACKED cord_diameter_avp {
    uint32_t code;              // AVP Code
    uint8_t flags;              // AVP Flags
    uint8_t length[3];          // AVP Length
    uint32_t vendor_id;         // Vendor-ID (optional, if V flag set)
    uint8_t data[];             // AVP Data
} cord_diameter_avp_t;

// S1AP (S1 Application Protocol) header
typedef struct CORD_PACKED cord_s1ap_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t procedure_code;     // Procedure Code
    uint8_t criticality;        // Criticality
    uint8_t length[];           // Length (variable)
    // Followed by ASN.1 encoded message
} cord_s1ap_hdr_t;

// X2AP (X2 Application Protocol) header
typedef struct CORD_PACKED cord_x2ap_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t procedure_code;     // Procedure Code
    uint8_t criticality;        // Criticality
    uint8_t length[];           // Length (variable)
    // Followed by ASN.1 encoded message
} cord_x2ap_hdr_t;

// NGAP (NG Application Protocol) for 5G
typedef struct CORD_PACKED cord_ngap_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t procedure_code;     // Procedure Code
    uint8_t criticality;        // Criticality
    uint8_t length[];           // Length (variable)
    // Followed by ASN.1 encoded message
} cord_ngap_hdr_t;

// NAS (Non-Access Stratum) header for EPC
typedef struct CORD_PACKED cord_nas_epc_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t security_header_type;   // Security Header Type
    uint8_t message_type;       // Message Type
    // Variable format based on message type
} cord_nas_epc_hdr_t;

// NAS header for 5GC (5G Core)
typedef struct CORD_PACKED cord_nas_5gc_hdr {
    uint8_t extended_protocol_discriminator; // Extended Protocol Discriminator
    uint8_t security_header_type;   // Security Header Type
    uint8_t message_type;       // Message Type
    // Variable format based on message type
} cord_nas_5gc_hdr_t;

// PFCP (Packet Forwarding Control Protocol) header
typedef struct CORD_PACKED cord_pfcp_hdr {
    uint8_t version_spare_mp;   // Version:3, Spare:3, MP:1, S:1
    uint8_t message_type;       // Message Type
    uint16_t message_length;    // Message Length
    uint64_t seid_or_seq;       // SEID (if S=1) or Sequence Number
    // Followed by Information Elements
} cord_pfcp_hdr_t;

// RRC (Radio Resource Control) header - simplified
typedef struct CORD_PACKED cord_rrc_hdr {
    uint8_t msg_type;           // Message Type
    // Highly variable format, ASN.1 encoded
} cord_rrc_hdr_t;

// PDCP (Packet Data Convergence Protocol) header
typedef struct CORD_PACKED cord_pdcp_data_hdr {
    uint8_t dc_sn_high;         // D/C:1, Reserved:3, SN high:4
    uint8_t sn_low;             // SN low:8 (for 12-bit SN)
} cord_pdcp_data_hdr_t;

// PDCP header for 18-bit SN
typedef struct CORD_PACKED cord_pdcp_data_hdr_18 {
    uint8_t dc_reserved_sn_high; // D/C:1, Reserved:1, SN high:6
    uint8_t sn_mid;             // SN mid:8
    uint8_t sn_low;             // SN low:4, Reserved:4
} cord_pdcp_data_hdr_18_t;

// RLC (Radio Link Control) AM (Acknowledged Mode) header
typedef struct CORD_PACKED cord_rlc_am_hdr {
    uint8_t dc_rf_p_fi_e_sn_high; // D/C:1, RF:1, P:1, FI:2, E:1, SN high:2
    uint8_t sn_low;             // SN low:8
} cord_rlc_am_hdr_t;

// RLC UM (Unacknowledged Mode) header
typedef struct CORD_PACKED cord_rlc_um_hdr {
    uint8_t dc_fi_e_sn_high;    // D/C:1, FI:2, E:1, SN high:4
    uint8_t sn_low;             // SN low:8 (for 10-bit SN)
} cord_rlc_um_hdr_t;

// MAC (Medium Access Control) header
typedef struct CORD_PACKED cord_mac_hdr {
    uint8_t lcid_e_r;           // LCID:5, E:1, R:2
    // Length field optional based on LCID value
} cord_mac_hdr_t;

// MAC header with length
typedef struct CORD_PACKED cord_mac_hdr_with_len {
    uint8_t lcid_e_r;           // LCID:5, E:1, R:2
    uint8_t length_f;           // Length:7, F:1
    // Additional length byte if F=1
} cord_mac_hdr_with_len_t;

// Mobility Management (MM) header for GSM/GPRS
typedef struct CORD_PACKED cord_mm_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t message_type;       // Message Type
} cord_mm_hdr_t;

// Session Management (SM) header for GSM/GPRS
typedef struct CORD_PACKED cord_sm_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t transaction_identifier; // Transaction Identifier
    uint8_t message_type;       // Message Type
} cord_sm_hdr_t;

// GMM (GPRS Mobility Management) header
typedef struct CORD_PACKED cord_gmm_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t message_type;       // Message Type
} cord_gmm_hdr_t;

// GPRS LLC (Logical Link Control) header
typedef struct CORD_PACKED cord_llc_hdr_gprs {
    uint8_t address_control;    // Address and Control fields
    // Variable format based on frame type
} cord_llc_hdr_gprs_t;

// BSSGP (Base Station Subsystem GPRS Protocol) header
typedef struct CORD_PACKED cord_bssgp_hdr {
    uint8_t pdu_type;           // PDU Type
    // Followed by Information Elements
} cord_bssgp_hdr_t;

// 3GPP Protocol Constants

// GTP-C message types
#define CORD_GTPC_CREATE_PDP_CONTEXT_REQUEST    16
#define CORD_GTPC_CREATE_PDP_CONTEXT_RESPONSE   17
#define CORD_GTPC_UPDATE_PDP_CONTEXT_REQUEST    18
#define CORD_GTPC_UPDATE_PDP_CONTEXT_RESPONSE   19
#define CORD_GTPC_DELETE_PDP_CONTEXT_REQUEST    20
#define CORD_GTPC_DELETE_PDP_CONTEXT_RESPONSE   21

// NAS Protocol Discriminators
#define CORD_NAS_PD_MM                  0x05    // Mobility Management
#define CORD_NAS_PD_CC                  0x03    // Call Control
#define CORD_NAS_PD_SS                  0x0B    // Supplementary Services
#define CORD_NAS_PD_SMS                 0x09    // Short Message Service
#define CORD_NAS_PD_GMM                 0x08    // GPRS Mobility Management
#define CORD_NAS_PD_SM                  0x0A    // Session Management
#define CORD_NAS_PD_EMM                 0x07    // EPS Mobility Management
#define CORD_NAS_PD_ESM                 0x02    // EPS Session Management
#define CORD_NAS_PD_5GMM                0x7E    // 5GS Mobility Management
#define CORD_NAS_PD_5GSM                0x2E    // 5GS Session Management

// S1AP Procedure Codes
#define CORD_S1AP_HANDOVER_PREPARATION          0
#define CORD_S1AP_HANDOVER_RESOURCE_ALLOCATION  1
#define CORD_S1AP_HANDOVER_NOTIFICATION         2
#define CORD_S1AP_PATH_SWITCH_REQUEST           3
#define CORD_S1AP_HANDOVER_CANCEL               4
#define CORD_S1AP_E_RAB_SETUP_REQUEST           5
#define CORD_S1AP_E_RAB_MODIFY_REQUEST          6
#define CORD_S1AP_E_RAB_RELEASE_COMMAND         7
#define CORD_S1AP_E_RAB_RELEASE_INDICATION      8
#define CORD_S1AP_INITIAL_CONTEXT_SETUP_REQUEST 9

// PFCP Message Types
#define CORD_PFCP_HEARTBEAT_REQUEST             1
#define CORD_PFCP_HEARTBEAT_RESPONSE            2
#define CORD_PFCP_PFD_MANAGEMENT_REQUEST        3
#define CORD_PFCP_PFD_MANAGEMENT_RESPONSE       4
#define CORD_PFCP_ASSOCIATION_SETUP_REQUEST     5
#define CORD_PFCP_ASSOCIATION_SETUP_RESPONSE    6
#define CORD_PFCP_SESSION_ESTABLISHMENT_REQUEST 50
#define CORD_PFCP_SESSION_ESTABLISHMENT_RESPONSE 51
#define CORD_PFCP_SESSION_MODIFICATION_REQUEST  52
#define CORD_PFCP_SESSION_MODIFICATION_RESPONSE 53
#define CORD_PFCP_SESSION_DELETION_REQUEST      54
#define CORD_PFCP_SESSION_DELETION_RESPONSE     55

// MAC LCID (Logical Channel ID) values
#define CORD_MAC_LCID_CCCH              0x00    // CCCH
#define CORD_MAC_LCID_DCCH_DTCH_1       0x01    // DCCH/DTCH 1
#define CORD_MAC_LCID_DCCH_DTCH_2       0x02    // DCCH/DTCH 2
#define CORD_MAC_LCID_DCCH_DTCH_3       0x03    // DCCH/DTCH 3
#define CORD_MAC_LCID_PADDING           0x1F    // Padding
#define CORD_MAC_LCID_BSR               0x1D    // Buffer Status Report
#define CORD_MAC_LCID_PHR               0x1A    // Power Headroom Report

// 3GPP Application IDs for Diameter
#define CORD_3GPP_APP_ID_S6A            16777251 // S6a/S6d
#define CORD_3GPP_APP_ID_S13            16777252 // S13/S13'
#define CORD_3GPP_APP_ID_SWM            16777264 // SWm
#define CORD_3GPP_APP_ID_SWX            16777265 // SWx
#define CORD_3GPP_APP_ID_GX             16777238 // Gx
#define CORD_3GPP_APP_ID_GY             16777235 // Gy
#define CORD_3GPP_APP_ID_RX             16777236 // Rx
#define CORD_3GPP_APP_ID_SH             16777217 // Sh

// Helper macros for PDCP
#define CORD_PDCP_GET_DC(pdcp)          (((pdcp)->dc_sn_high & 0x80) >> 7)
#define CORD_PDCP_GET_SN_12(pdcp)       ((((pdcp)->dc_sn_high & 0x0F) << 8) | (pdcp)->sn_low)

// Helper macros for RLC
#define CORD_RLC_GET_DC(rlc)            (((rlc)->dc_rf_p_fi_e_sn_high & 0x80) >> 7)
#define CORD_RLC_GET_RF(rlc)            (((rlc)->dc_rf_p_fi_e_sn_high & 0x40) >> 6)
#define CORD_RLC_GET_P(rlc)             (((rlc)->dc_rf_p_fi_e_sn_high & 0x20) >> 5)
#define CORD_RLC_GET_FI(rlc)            (((rlc)->dc_rf_p_fi_e_sn_high & 0x18) >> 3)
#define CORD_RLC_GET_E(rlc)             (((rlc)->dc_rf_p_fi_e_sn_high & 0x04) >> 2)
#define CORD_RLC_GET_SN(rlc)            ((((rlc)->dc_rf_p_fi_e_sn_high & 0x03) << 8) | (rlc)->sn_low)

// Helper macros for MAC
#define CORD_MAC_GET_LCID(mac)          (((mac)->lcid_e_r & 0xF8) >> 3)
#define CORD_MAC_GET_E(mac)             (((mac)->lcid_e_r & 0x04) >> 2)
#define CORD_MAC_GET_R(mac)             ((mac)->lcid_e_r & 0x03)

// Helper macros for PFCP
#define CORD_PFCP_GET_VERSION(pfcp)     (((pfcp)->version_spare_mp & 0xE0) >> 5)
#define CORD_PFCP_GET_MP(pfcp)          (((pfcp)->version_spare_mp & 0x02) >> 1)
#define CORD_PFCP_GET_S(pfcp)           ((pfcp)->version_spare_mp & 0x01)

#endif // CORD_3GPP_PROTOCOLS_H