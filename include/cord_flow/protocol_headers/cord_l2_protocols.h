#ifndef CORD_L2_PROTOCOLS_H
#define CORD_L2_PROTOCOLS_H

#include "cord_protocol_common.h"

// Ethernet II header
typedef struct CORD_PACKED cord_eth_hdr {
    cord_mac_addr_t h_dest;     // Destination MAC address
    cord_mac_addr_t h_source;   // Source MAC address
    uint16_t h_proto;           // Protocol type
} cord_eth_hdr_t;

// 802.1Q VLAN header
typedef struct CORD_PACKED cord_vlan_hdr {
    uint16_t tci;               // Tag Control Information (PCP:3, DEI:1, VID:12)
    uint16_t h_proto;           // Protocol type
} cord_vlan_hdr_t;

// 802.1ad Service VLAN header (same structure as 802.1Q)
typedef cord_vlan_hdr_t cord_svlan_hdr_t;

// MPLS header
typedef struct CORD_PACKED cord_mpls_hdr {
    uint32_t label_exp_s_ttl;   // Label:20, EXP:3, S:1, TTL:8
} cord_mpls_hdr_t;

// ARP header
typedef struct CORD_PACKED cord_arp_hdr {
    uint16_t ar_hrd;            // Hardware type
    uint16_t ar_pro;            // Protocol type
    uint8_t ar_hln;             // Hardware address length
    uint8_t ar_pln;             // Protocol address length
    uint16_t ar_op;             // Operation code
    cord_mac_addr_t ar_sha;     // Sender hardware address
    cord_ipv4_addr_t ar_sip;    // Sender IP address
    cord_mac_addr_t ar_tha;     // Target hardware address
    cord_ipv4_addr_t ar_tip;    // Target IP address
} cord_arp_hdr_t;

// LLC header (802.2)
typedef struct CORD_PACKED cord_llc_hdr {
    uint8_t dsap;               // Destination Service Access Point
    uint8_t ssap;               // Source Service Access Point
    uint8_t ctrl;               // Control field
} cord_llc_hdr_t;

// SNAP header
typedef struct CORD_PACKED cord_snap_hdr {
    uint8_t oui[3];             // Organizationally Unique Identifier
    uint16_t type;              // Type
} cord_snap_hdr_t;

// PPPoE header
typedef struct CORD_PACKED cord_pppoe_hdr {
    uint8_t ver_type;           // Version:4, Type:4
    uint8_t code;               // Code
    uint16_t session_id;        // Session ID
    uint16_t length;            // Length
} cord_pppoe_hdr_t;

// PPP header
typedef struct CORD_PACKED cord_ppp_hdr {
    uint16_t protocol;          // Protocol
} cord_ppp_hdr_t;

// STP/RSTP BPDU header
typedef struct CORD_PACKED cord_stp_bpdu {
    uint16_t protocol_id;       // Protocol Identifier
    uint8_t version;            // Protocol Version Identifier
    uint8_t bpdu_type;          // BPDU Type
    uint8_t flags;              // Flags
    uint64_t root_id;           // Root Identifier
    uint32_t root_path_cost;    // Root Path Cost
    uint64_t bridge_id;         // Bridge Identifier
    uint16_t port_id;           // Port Identifier
    uint16_t message_age;       // Message Age
    uint16_t max_age;           // Max Age
    uint16_t hello_time;        // Hello Time
    uint16_t forward_delay;     // Forward Delay
} cord_stp_bpdu_t;

// LLDP header
typedef struct CORD_PACKED cord_lldp_tlv {
    uint16_t type_length;       // Type:7, Length:9
    uint8_t value[];            // Variable length value
} cord_lldp_tlv_t;

// LACP header
typedef struct CORD_PACKED cord_lacp_hdr {
    uint8_t subtype;            // LACP subtype
    uint8_t version;            // Version number
    uint8_t actor_type;         // Actor TLV type
    uint8_t actor_length;       // Actor information length
    uint16_t actor_system_priority;     // Actor system priority
    cord_mac_addr_t actor_system;       // Actor system ID
    uint16_t actor_key;         // Actor key
    uint16_t actor_port_priority;       // Actor port priority
    uint16_t actor_port;        // Actor port number
    uint8_t actor_state;        // Actor state
    uint8_t reserved1[3];       // Reserved
    uint8_t partner_type;       // Partner TLV type
    uint8_t partner_length;     // Partner information length
    uint16_t partner_system_priority;   // Partner system priority
    cord_mac_addr_t partner_system;     // Partner system ID
    uint16_t partner_key;       // Partner key
    uint16_t partner_port_priority;     // Partner port priority
    uint16_t partner_port;      // Partner port number
    uint8_t partner_state;      // Partner state
    uint8_t reserved2[3];       // Reserved
    uint8_t collector_type;     // Collector TLV type
    uint8_t collector_length;   // Collector information length
    uint16_t collector_max_delay;       // Collector max delay
    uint8_t reserved3[12];      // Reserved
    uint8_t terminator_type;    // Terminator TLV type
    uint8_t terminator_length;  // Terminator length
    uint8_t reserved4[50];      // Reserved
} cord_lacp_hdr_t;

// L2 protocol constants
#define CORD_ARP_REQUEST    1
#define CORD_ARP_REPLY      2
#define CORD_ARP_RREQUEST   3
#define CORD_ARP_RREPLY     4

#define CORD_VLAN_PCP_MASK  0xE000
#define CORD_VLAN_DEI_MASK  0x1000
#define CORD_VLAN_VID_MASK  0x0FFF

#define CORD_MPLS_LABEL_MASK    0xFFFFF000
#define CORD_MPLS_EXP_MASK      0x00000E00
#define CORD_MPLS_S_MASK        0x00000100
#define CORD_MPLS_TTL_MASK      0x000000FF

// Helper macros for VLAN
#define CORD_VLAN_GET_PCP(tci)  (((tci) & CORD_VLAN_PCP_MASK) >> 13)
#define CORD_VLAN_GET_DEI(tci)  (((tci) & CORD_VLAN_DEI_MASK) >> 12)
#define CORD_VLAN_GET_VID(tci)  ((tci) & CORD_VLAN_VID_MASK)
#define CORD_VLAN_SET_TCI(pcp, dei, vid) \
    (((pcp) << 13) | ((dei) << 12) | ((vid) & CORD_VLAN_VID_MASK))

// Helper macros for MPLS
#define CORD_MPLS_GET_LABEL(entry) (((entry) & CORD_MPLS_LABEL_MASK) >> 12)
#define CORD_MPLS_GET_EXP(entry)   (((entry) & CORD_MPLS_EXP_MASK) >> 9)
#define CORD_MPLS_GET_S(entry)     (((entry) & CORD_MPLS_S_MASK) >> 8)
#define CORD_MPLS_GET_TTL(entry)   ((entry) & CORD_MPLS_TTL_MASK)
#define CORD_MPLS_SET_ENTRY(label, exp, s, ttl) \
    (((label) << 12) | ((exp) << 9) | ((s) << 8) | (ttl))

#endif // CORD_L2_PROTOCOLS_H