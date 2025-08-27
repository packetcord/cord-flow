#ifndef CORD_TUNNELING_PROTOCOLS_H
#define CORD_TUNNELING_PROTOCOLS_H

#include "cord_protocol_common.h"

// GRE (Generic Routing Encapsulation) header
typedef struct CORD_PACKED cord_gre_hdr {
    uint16_t flags_version;     // C:1, R:1, K:1, S:1, s:1, Recur:3, Flags:5, Ver:3
    uint16_t protocol;          // Protocol Type
    // Optional fields follow based on flags
} cord_gre_hdr_t;

// GRE optional fields
typedef struct CORD_PACKED cord_gre_opt {
    uint16_t checksum;          // Checksum (if C=1)
    uint16_t reserved1;         // Reserved (if C=1)
    uint32_t key;               // Key (if K=1)
    uint32_t seq_number;        // Sequence Number (if S=1)
} cord_gre_opt_t;

// VXLAN header
typedef struct CORD_PACKED cord_vxlan_hdr {
    uint8_t flags;              // Flags (I:1, Reserved:7)
    uint8_t reserved1[3];       // Reserved
    uint8_t vni[3];             // VXLAN Network Identifier
    uint8_t reserved2;          // Reserved
} cord_vxlan_hdr_t;

// NVGRE header
typedef struct CORD_PACKED cord_nvgre_hdr {
    uint16_t flags_version;     // Flags and Version (reuses GRE format)
    uint16_t protocol;          // Protocol Type (0x6558 for NVGRE)
    uint8_t vsid[3];            // Virtual Subnet ID
    uint8_t flowid;             // FlowID
} cord_nvgre_hdr_t;

// GENEVE header
typedef struct CORD_PACKED cord_geneve_hdr {
    uint8_t ver_opt_len;        // Version:2, Opt Len:6
    uint8_t flags;              // O:1, C:1, Rsvd:6
    uint16_t protocol;          // Protocol Type
    uint8_t vni[3];             // Virtual Network Identifier
    uint8_t reserved;           // Reserved
    // Variable length options follow
} cord_geneve_hdr_t;

// STT (Stateless Transport Tunneling) header
typedef struct CORD_PACKED cord_stt_hdr {
    uint8_t version_flags;      // Version:4, Flags:4
    uint8_t length;             // Header Length
    uint16_t reserved;          // Reserved
    uint64_t context_id;        // Context ID
    uint32_t padding_rsvd;      // Padding:16, Reserved:16
} cord_stt_hdr_t;

// L2TP header (no sequence numbers)
typedef struct CORD_PACKED cord_l2tp_hdr {
    uint16_t flags_ver_len;     // T:1, L:1, x:2, S:1, x:1, O:1, P:1, x:4, Ver:4
    uint16_t tunnel_id;         // Tunnel ID
    uint16_t session_id;        // Session ID
} cord_l2tp_hdr_t;

// L2TP header with sequence numbers
typedef struct CORD_PACKED cord_l2tp_seq_hdr {
    uint16_t flags_ver_len;     // Flags, Version, Length
    uint16_t length;            // Length (if L=1)
    uint16_t tunnel_id;         // Tunnel ID
    uint16_t session_id;        // Session ID
    uint16_t ns;                // Next Sent (if S=1)
    uint16_t nr;                // Next Received (if S=1)
} cord_l2tp_seq_hdr_t;

// PPTP GRE header
typedef struct CORD_PACKED cord_pptp_gre_hdr {
    uint16_t flags_version;     // Flags and Version
    uint16_t protocol;          // Protocol Type (0x880B for PPTP)
    uint16_t payload_len;       // Payload Length
    uint16_t call_id;           // Call ID
    uint32_t seq_number;        // Sequence Number (if S=1)
    uint32_t ack_number;        // Acknowledgment Number (if A=1)
} cord_pptp_gre_hdr_t;

// IPinIP header (just an IP header)
typedef struct cord_ipv4_hdr cord_ipinip_hdr_t;

// IP-in-IPv6 header
typedef struct cord_ipv6_hdr cord_ipv6inipv6_hdr_t;

// MPLS-in-GRE (MPLS over GRE)
typedef struct CORD_PACKED cord_mpls_gre {
    cord_gre_hdr_t gre;         // GRE header
    cord_mpls_hdr_t mpls;       // MPLS header
} cord_mpls_gre_t;

// VPLS (Virtual Private LAN Service) - uses MPLS
typedef struct CORD_PACKED cord_vpls_hdr {
    cord_mpls_hdr_t mpls_labels[2]; // Transport and VC labels
    uint32_t control_word;      // Control Word (optional)
} cord_vpls_hdr_t;

// MPLS-TP (MPLS Transport Profile) OAM header
typedef struct CORD_PACKED cord_mpls_tp_oam {
    uint8_t mel;                // MEG Level
    uint8_t version;            // Version
    uint8_t opcode;             // OpCode
    uint8_t flags;              // Flags
    uint8_t tlv_offset;         // TLV Offset
} cord_mpls_tp_oam_t;

// TRILL (Transparent Interconnection of Lots of Links) header
typedef struct CORD_PACKED cord_trill_hdr {
    uint16_t version_reserved_multicast_optlen; // V:2, R:2, M:1, Op-Length:5, Hop Count:6
    uint16_t egress_rbridge;    // Egress RBridge Nickname
    uint16_t ingress_rbridge;   // Ingress RBridge Nickname
} cord_trill_hdr_t;

// NSH (Network Service Header) - Base Header
typedef struct CORD_PACKED cord_nsh_base_hdr {
    uint16_t ver_oam_critical_reserved; // Ver:2, O:1, C:1, Reserved:6, TTL:6
    uint8_t length;             // Length
    uint8_t md_type;            // MD Type
    uint8_t next_proto;         // Next Protocol
    uint8_t service_path_id[3]; // Service Path Identifier
    uint8_t service_index;      // Service Index
} cord_nsh_base_hdr_t;

// NSH Context Header (MD Type 1)
typedef struct CORD_PACKED cord_nsh_md1_ctx {
    uint32_t context[4];        // Context Data
} cord_nsh_md1_ctx_t;

// LISP (Locator/ID Separation Protocol) header
typedef struct CORD_PACKED cord_lisp_hdr {
    uint8_t flags;              // N:1, L:1, E:1, V:1, I:1, R:3
    uint8_t reserved[3];        // Reserved/Flags
    uint32_t nonce_locator_status_bits; // Nonce/Map-Version/LSB fields
} cord_lisp_hdr_t;

// GTP-U (GPRS Tunneling Protocol User Plane) header
typedef struct CORD_PACKED cord_gtpu_hdr {
    uint8_t version_pt_reserved_e_s_pn; // Version:3, PT:1, Reserved:1, E:1, S:1, PN:1
    uint8_t message_type;       // Message Type
    uint16_t length;            // Length
    uint32_t teid;              // Tunnel Endpoint Identifier
} cord_gtpu_hdr_t;

// GTP-U optional fields
typedef struct CORD_PACKED cord_gtpu_opt {
    uint16_t seq_number;        // Sequence Number (if S=1)
    uint8_t n_pdu_number;       // N-PDU Number (if PN=1)
    uint8_t next_ext_hdr_type;  // Next Extension Header Type (if E=1)
} cord_gtpu_opt_t;

// GRE flags
#define CORD_GRE_CSUM           0x8000  // Checksum Present
#define CORD_GRE_ROUTING        0x4000  // Routing Present
#define CORD_GRE_KEY            0x2000  // Key Present
#define CORD_GRE_SEQ            0x1000  // Sequence Number Present
#define CORD_GRE_STRICT         0x0800  // Strict Source Route
#define CORD_GRE_REC            0x0700  // Recursion Control
#define CORD_GRE_FLAGS          0x00F8  // Flags
#define CORD_GRE_VERSION        0x0007  // Version

// VXLAN constants
#define CORD_VXLAN_VNI_MASK     0xFFFFFF00
#define CORD_VXLAN_FLAGS_VALID_VNI 0x08

// GENEVE constants
#define CORD_GENEVE_VER_MASK    0xC0
#define CORD_GENEVE_OPT_LEN_MASK 0x3F
#define CORD_GENEVE_FLAG_OAM    0x80
#define CORD_GENEVE_FLAG_CRIT   0x40

// L2TP flags
#define CORD_L2TP_TYPE          0x8000  // Type (0=Data, 1=Control)
#define CORD_L2TP_LENGTH        0x4000  // Length field present
#define CORD_L2TP_SEQUENCE      0x0800  // Sequence numbers present
#define CORD_L2TP_OFFSET        0x0200  // Offset size present
#define CORD_L2TP_PRIORITY      0x0100  // Priority
#define CORD_L2TP_VERSION       0x000F  // Version

// NSH constants
#define CORD_NSH_VER_MASK       0xC000
#define CORD_NSH_OAM_MASK       0x2000
#define CORD_NSH_CRITICAL_MASK  0x1000
#define CORD_NSH_TTL_MASK       0x003F

// Helper macros for VXLAN
#define CORD_VXLAN_GET_VNI(vxh) \
    (((vxh)->vni[0] << 16) | ((vxh)->vni[1] << 8) | (vxh)->vni[2])
#define CORD_VXLAN_SET_VNI(vxh, vni) do { \
    (vxh)->vni[0] = ((vni) >> 16) & 0xFF; \
    (vxh)->vni[1] = ((vni) >> 8) & 0xFF; \
    (vxh)->vni[2] = (vni) & 0xFF; \
} while(0)

// Helper macros for GRE
#define CORD_GRE_GET_CSUM(greh)     (((greh)->flags_version & CORD_GRE_CSUM) != 0)
#define CORD_GRE_GET_KEY(greh)      (((greh)->flags_version & CORD_GRE_KEY) != 0)
#define CORD_GRE_GET_SEQ(greh)      (((greh)->flags_version & CORD_GRE_SEQ) != 0)
#define CORD_GRE_GET_VERSION(greh)  ((greh)->flags_version & CORD_GRE_VERSION)

#endif // CORD_TUNNELING_PROTOCOLS_H