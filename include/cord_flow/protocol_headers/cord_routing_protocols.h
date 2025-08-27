#ifndef CORD_ROUTING_PROTOCOLS_H
#define CORD_ROUTING_PROTOCOLS_H

#include "cord_protocol_common.h"

// OSPF (Open Shortest Path First) header
typedef struct CORD_PACKED cord_ospf_hdr {
    uint8_t version;            // Version Number
    uint8_t type;               // Type
    uint16_t length;            // Packet Length
    cord_ipv4_addr_t router_id; // Router ID
    cord_ipv4_addr_t area_id;   // Area ID
    uint16_t checksum;          // Checksum
    uint16_t autype;            // Authentication Type
    uint64_t authentication;    // Authentication
} cord_ospf_hdr_t;

// OSPF Hello packet
typedef struct CORD_PACKED cord_ospf_hello {
    cord_ospf_hdr_t hdr;        // OSPF Header
    cord_ipv4_addr_t network_mask;      // Network Mask
    uint16_t hello_interval;    // Hello Interval
    uint8_t options;            // Options
    uint8_t rtr_priority;       // Router Priority
    uint32_t router_dead_interval;      // Router Dead Interval
    cord_ipv4_addr_t designated_router; // Designated Router
    cord_ipv4_addr_t backup_designated_router; // Backup Designated Router
    // Followed by neighbor list
} cord_ospf_hello_t;

// OSPF Database Description packet
typedef struct CORD_PACKED cord_ospf_db_desc {
    cord_ospf_hdr_t hdr;        // OSPF Header
    uint16_t interface_mtu;     // Interface MTU
    uint8_t options;            // Options
    uint8_t flags;              // Flags (I, M, MS)
    uint32_t dd_seqnum;         // DD Sequence Number
    // Followed by LSA headers
} cord_ospf_db_desc_t;

// OSPF Link State Request packet
typedef struct CORD_PACKED cord_ospf_ls_req {
    cord_ospf_hdr_t hdr;        // OSPF Header
    // Followed by LSA request entries
} cord_ospf_ls_req_t;

// OSPF Link State Request entry
typedef struct CORD_PACKED cord_ospf_ls_req_entry {
    uint8_t ls_type;            // LS Type
    uint8_t reserved[3];        // Reserved
    cord_ipv4_addr_t link_state_id;     // Link State ID
    cord_ipv4_addr_t advertising_router; // Advertising Router
} cord_ospf_ls_req_entry_t;

// OSPF Link State Update packet
typedef struct CORD_PACKED cord_ospf_ls_upd {
    cord_ospf_hdr_t hdr;        // OSPF Header
    uint32_t num_lsas;          // Number of LSAs
    // Followed by LSAs
} cord_ospf_ls_upd_t;

// OSPF Link State Acknowledgment packet
typedef struct CORD_PACKED cord_ospf_ls_ack {
    cord_ospf_hdr_t hdr;        // OSPF Header
    // Followed by LSA headers
} cord_ospf_ls_ack_t;

// OSPF LSA (Link State Advertisement) header
typedef struct CORD_PACKED cord_ospf_lsa_hdr {
    uint16_t ls_age;            // LS Age
    uint8_t options;            // Options
    uint8_t ls_type;            // LS Type
    cord_ipv4_addr_t link_state_id;     // Link State ID
    cord_ipv4_addr_t advertising_router; // Advertising Router
    uint32_t ls_seqnum;         // LS Sequence Number
    uint16_t ls_checksum;       // LS Checksum
    uint16_t length;            // Length
} cord_ospf_lsa_hdr_t;

// OSPF Router LSA (Type 1)
typedef struct CORD_PACKED cord_ospf_router_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    uint8_t flags;              // Flags (V, E, B)
    uint8_t reserved;           // Reserved
    uint16_t num_links;         // Number of Links
    // Followed by link descriptions
} cord_ospf_router_lsa_t;

// OSPF Router LSA Link
typedef struct CORD_PACKED cord_ospf_router_link {
    cord_ipv4_addr_t link_id;   // Link ID
    cord_ipv4_addr_t link_data; // Link Data
    uint8_t type;               // Type
    uint8_t num_tos;            // Number of TOS
    uint16_t metric;            // Metric
    // Followed by TOS metrics
} cord_ospf_router_link_t;

// OSPF Network LSA (Type 2)
typedef struct CORD_PACKED cord_ospf_network_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    cord_ipv4_addr_t network_mask; // Network Mask
    // Followed by attached routers
} cord_ospf_network_lsa_t;

// OSPF Summary LSA (Type 3 & 4)
typedef struct CORD_PACKED cord_ospf_summary_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    cord_ipv4_addr_t network_mask; // Network Mask
    uint8_t reserved;           // Reserved
    uint8_t metric[3];          // Metric (24 bits)
    // Followed by TOS metrics
} cord_ospf_summary_lsa_t;

// OSPF AS External LSA (Type 5)
typedef struct CORD_PACKED cord_ospf_external_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    cord_ipv4_addr_t network_mask; // Network Mask
    uint8_t e_type_metric[4];   // E bit + Metric (31 bits)
    cord_ipv4_addr_t forwarding_addr; // Forwarding Address
    uint32_t external_route_tag; // External Route Tag
    // Followed by TOS metrics
} cord_ospf_external_lsa_t;

// OSPF Group Membership LSA (Type 6)
typedef struct CORD_PACKED cord_ospf_group_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    // Group membership information
} cord_ospf_group_lsa_t;

// OSPF NSSA LSA (Type 7)
typedef struct CORD_PACKED cord_ospf_nssa_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    cord_ipv4_addr_t network_mask; // Network Mask
    uint8_t e_type_metric[4];   // E bit + Metric (31 bits)
    cord_ipv4_addr_t forwarding_addr; // Forwarding Address
    uint32_t external_route_tag; // External Route Tag
    // Followed by TOS metrics
} cord_ospf_nssa_lsa_t;

// OSPF Opaque LSA (Types 9, 10, 11)
typedef struct CORD_PACKED cord_ospf_opaque_lsa {
    cord_ospf_lsa_hdr_t hdr;    // LSA Header
    uint8_t opaque_type;        // Opaque Type
    uint8_t opaque_id[3];       // Opaque ID (24 bits)
    // Followed by opaque information
} cord_ospf_opaque_lsa_t;

// BGP (Border Gateway Protocol) header
typedef struct CORD_PACKED cord_bgp_hdr {
    uint8_t marker[16];         // Marker (all ones)
    uint16_t length;            // Length
    uint8_t type;               // Type
} cord_bgp_hdr_t;

// BGP OPEN message
typedef struct CORD_PACKED cord_bgp_open {
    cord_bgp_hdr_t hdr;         // BGP Header
    uint8_t version;            // Version
    uint16_t as_number;         // AS Number
    uint16_t hold_time;         // Hold Time
    cord_ipv4_addr_t bgp_id;    // BGP Identifier
    uint8_t opt_param_len;      // Optional Parameters Length
    // Followed by optional parameters
} cord_bgp_open_t;

// BGP UPDATE message
typedef struct CORD_PACKED cord_bgp_update {
    cord_bgp_hdr_t hdr;         // BGP Header
    uint16_t withdrawn_routes_len; // Withdrawn Routes Length
    // Followed by withdrawn routes, path attributes length, path attributes, and NLRI
} cord_bgp_update_t;

// BGP NOTIFICATION message
typedef struct CORD_PACKED cord_bgp_notification {
    cord_bgp_hdr_t hdr;         // BGP Header
    uint8_t error_code;         // Error Code
    uint8_t error_subcode;      // Error Subcode
    uint8_t data[];             // Data
} cord_bgp_notification_t;

// BGP KEEPALIVE message
typedef struct CORD_PACKED cord_bgp_keepalive {
    cord_bgp_hdr_t hdr;         // BGP Header
} cord_bgp_keepalive_t;

// BGP UPDATE message path attribute
typedef struct CORD_PACKED cord_bgp_path_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Attribute Type Code
    // Length field varies based on Extended Length flag
    uint8_t data[];             // Attribute data
} cord_bgp_path_attr_t;

// BGP ORIGIN attribute (Type 1)
typedef struct CORD_PACKED cord_bgp_origin_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (1)
    uint8_t length;             // Length (1)
    uint8_t origin;             // Origin (0=IGP, 1=EGP, 2=INCOMPLETE)
} cord_bgp_origin_attr_t;

// BGP AS_PATH attribute (Type 2)
typedef struct CORD_PACKED cord_bgp_as_path_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (2)
    uint16_t length;            // Length (Extended Length)
    // Followed by AS path segments
} cord_bgp_as_path_attr_t;

// BGP AS Path Segment
typedef struct CORD_PACKED cord_bgp_as_path_segment {
    uint8_t type;               // Segment Type (1=AS_SET, 2=AS_SEQUENCE)
    uint8_t length;             // Number of ASNs
    // Followed by ASNs
} cord_bgp_as_path_segment_t;

// BGP NEXT_HOP attribute (Type 3)
typedef struct CORD_PACKED cord_bgp_next_hop_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (3)
    uint8_t length;             // Length (4)
    cord_ipv4_addr_t next_hop;  // Next Hop
} cord_bgp_next_hop_attr_t;

// BGP MULTI_EXIT_DISC attribute (Type 4)
typedef struct CORD_PACKED cord_bgp_med_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (4)
    uint8_t length;             // Length (4)
    uint32_t med;               // Multi Exit Discriminator
} cord_bgp_med_attr_t;

// BGP LOCAL_PREF attribute (Type 5)
typedef struct CORD_PACKED cord_bgp_local_pref_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (5)
    uint8_t length;             // Length (4)
    uint32_t local_pref;        // Local Preference
} cord_bgp_local_pref_attr_t;

// BGP ATOMIC_AGGREGATE attribute (Type 6)
typedef struct CORD_PACKED cord_bgp_atomic_aggregate_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (6)
    uint8_t length;             // Length (0)
} cord_bgp_atomic_aggregate_attr_t;

// BGP AGGREGATOR attribute (Type 7)
typedef struct CORD_PACKED cord_bgp_aggregator_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (7)
    uint8_t length;             // Length (6 or 8)
    uint16_t as_number;         // AS Number (2 bytes) or uint32_t for 4-byte ASN
    cord_ipv4_addr_t aggregator_id; // Aggregator ID
} cord_bgp_aggregator_attr_t;

// BGP COMMUNITIES attribute (Type 8)
typedef struct CORD_PACKED cord_bgp_communities_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (8)
    uint16_t length;            // Length (Extended Length)
    // Followed by communities (4 bytes each)
} cord_bgp_communities_attr_t;

// BGP Community value
typedef struct CORD_PACKED cord_bgp_community {
    uint16_t asn;               // AS Number
    uint16_t value;             // Community Value
} cord_bgp_community_t;

// BGP MP_REACH_NLRI attribute (Type 14)
typedef struct CORD_PACKED cord_bgp_mp_reach_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (14)
    uint16_t length;            // Length (Extended Length)
    uint16_t afi;               // Address Family Identifier
    uint8_t safi;               // Subsequent Address Family Identifier
    uint8_t next_hop_len;       // Length of Next Hop Network Address
    // Followed by next hop, reserved byte, and NLRI
} cord_bgp_mp_reach_attr_t;

// BGP MP_UNREACH_NLRI attribute (Type 15)
typedef struct CORD_PACKED cord_bgp_mp_unreach_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (15)
    uint16_t length;            // Length (Extended Length)
    uint16_t afi;               // Address Family Identifier
    uint8_t safi;               // Subsequent Address Family Identifier
    // Followed by withdrawn routes
} cord_bgp_mp_unreach_attr_t;

// BGP Extended Communities attribute (Type 16)
typedef struct CORD_PACKED cord_bgp_extended_communities_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (16)
    uint16_t length;            // Length (Extended Length)
    // Followed by extended communities (8 bytes each)
} cord_bgp_extended_communities_attr_t;

// BGP Extended Community
typedef struct CORD_PACKED cord_bgp_extended_community {
    uint8_t type;               // Type
    uint8_t subtype;            // Subtype
    uint8_t value[6];           // Value (48 bits)
} cord_bgp_extended_community_t;

// BGP Large Communities attribute (Type 32)
typedef struct CORD_PACKED cord_bgp_large_communities_attr {
    uint8_t flags;              // Attribute Flags
    uint8_t type_code;          // Type Code (32)
    uint16_t length;            // Length (Extended Length)
    // Followed by large communities (12 bytes each)
} cord_bgp_large_communities_attr_t;

// BGP Large Community
typedef struct CORD_PACKED cord_bgp_large_community {
    uint32_t global_admin;      // Global Administrator
    uint32_t local_data1;       // Local Data 1
    uint32_t local_data2;       // Local Data 2
} cord_bgp_large_community_t;

// RIP (Routing Information Protocol) header
typedef struct CORD_PACKED cord_rip_hdr {
    uint8_t command;            // Command
    uint8_t version;            // Version
    uint16_t reserved;          // Must be zero
} cord_rip_hdr_t;

// RIP message
typedef struct CORD_PACKED cord_rip_msg {
    cord_rip_hdr_t hdr;         // RIP Header
    // Followed by route entries
} cord_rip_msg_t;

// RIPv1 route entry
typedef struct CORD_PACKED cord_rip_v1_entry {
    uint16_t address_family;    // Address Family Identifier
    uint16_t reserved1;         // Reserved (must be zero)
    cord_ipv4_addr_t ip_addr;   // IP Address
    uint32_t reserved2;         // Reserved (must be zero)
    uint32_t reserved3;         // Reserved (must be zero)
    uint32_t metric;            // Metric
} cord_rip_v1_entry_t;

// RIPv2 route entry
typedef struct CORD_PACKED cord_rip_v2_entry {
    uint16_t address_family;    // Address Family Identifier
    uint16_t route_tag;         // Route Tag
    cord_ipv4_addr_t ip_addr;   // IP Address
    cord_ipv4_addr_t subnet_mask; // Subnet Mask
    cord_ipv4_addr_t next_hop;  // Next Hop
    uint32_t metric;            // Metric
} cord_rip_v2_entry_t;

// RIPv2 Authentication entry
typedef struct CORD_PACKED cord_rip_v2_auth {
    uint16_t address_family;    // Address Family Identifier (0xFFFF)
    uint16_t auth_type;         // Authentication Type
    uint8_t auth_data[16];      // Authentication Data
} cord_rip_v2_auth_t;

// RIPng (RIP for IPv6) header
typedef struct CORD_PACKED cord_ripng_hdr {
    uint8_t command;            // Command
    uint8_t version;            // Version
    uint16_t reserved;          // Reserved (must be zero)
} cord_ripng_hdr_t;

// RIPng route entry
typedef struct CORD_PACKED cord_ripng_entry {
    cord_ipv6_addr_t prefix;    // IPv6 Prefix
    uint16_t route_tag;         // Route Tag
    uint8_t prefix_len;         // Prefix Length
    uint8_t metric;             // Metric
} cord_ripng_entry_t;

// IS-IS (Intermediate System to Intermediate System) Common header
typedef struct CORD_PACKED cord_isis_common_hdr {
    uint8_t irpd;               // Intradomain Routing Protocol Discriminator
    uint8_t length_indicator;   // Length Indicator
    uint8_t version_protocol_id; // Version/Protocol ID Extension
    uint8_t id_length;          // ID Length
    uint8_t pdu_type;           // PDU Type
    uint8_t version;            // Version
    uint8_t reserved;           // Reserved
    uint8_t maximum_area_addresses; // Maximum Area Addresses
} cord_isis_common_hdr_t;

// IS-IS Hello PDU (Point-to-Point)
typedef struct CORD_PACKED cord_isis_p2p_hello {
    cord_isis_common_hdr_t common; // Common Header
    uint8_t circuit_type;       // Circuit Type
    uint8_t source_id[6];       // Source ID
    uint16_t holding_time;      // Holding Time
    uint16_t pdu_length;        // PDU Length
    uint8_t local_circuit_id;   // Local Circuit ID
    // Followed by TLVs
} cord_isis_p2p_hello_t;

// IS-IS Hello PDU (LAN Level 1)
typedef struct CORD_PACKED cord_isis_lan_hello {
    cord_isis_common_hdr_t common; // Common Header
    uint8_t circuit_type;       // Circuit Type
    uint8_t source_id[6];       // Source ID
    uint16_t holding_time;      // Holding Time
    uint16_t pdu_length;        // PDU Length
    uint8_t priority;           // Priority
    uint8_t lan_id[7];          // LAN ID (Designated IS + Circuit ID)
    // Followed by TLVs
} cord_isis_lan_hello_t;

// IS-IS Link State PDU (Level 1 and Level 2)
typedef struct CORD_PACKED cord_isis_lsp {
    cord_isis_common_hdr_t common; // Common Header
    uint16_t pdu_length;        // PDU Length
    uint16_t remaining_lifetime; // Remaining Lifetime
    uint8_t lsp_id[8];          // LSP ID (Source ID + Pseudonode ID + LSP Number)
    uint32_t sequence_number;   // Sequence Number
    uint16_t checksum;          // Checksum
    uint8_t p_att_ol_is_type;   // P:1, ATT:4, OL:1, IS Type:2
    // Followed by TLVs
} cord_isis_lsp_t;

// IS-IS Complete Sequence Number PDU
typedef struct CORD_PACKED cord_isis_csnp {
    cord_isis_common_hdr_t common; // Common Header
    uint16_t pdu_length;        // PDU Length
    uint8_t source_id[7];       // Source ID + Circuit ID
    uint8_t start_lsp_id[8];    // Start LSP ID
    uint8_t end_lsp_id[8];      // End LSP ID
    // Followed by TLVs
} cord_isis_csnp_t;

// IS-IS Partial Sequence Number PDU
typedef struct CORD_PACKED cord_isis_psnp {
    cord_isis_common_hdr_t common; // Common Header
    uint16_t pdu_length;        // PDU Length
    uint8_t source_id[7];       // Source ID + Circuit ID
    // Followed by TLVs
} cord_isis_psnp_t;

// IS-IS TLV header
typedef struct CORD_PACKED cord_isis_tlv {
    uint8_t type;               // TLV Type
    uint8_t length;             // TLV Length
    uint8_t value[];            // TLV Value
} cord_isis_tlv_t;

// IS-IS Area Addresses TLV (Type 1)
typedef struct CORD_PACKED cord_isis_area_addr_tlv {
    uint8_t type;               // TLV Type (1)
    uint8_t length;             // TLV Length
    // Followed by area addresses
} cord_isis_area_addr_tlv_t;

// IS-IS IIS Neighbors TLV (Type 2)
typedef struct CORD_PACKED cord_isis_iis_neighbors_tlv {
    uint8_t type;               // TLV Type (2)
    uint8_t length;             // TLV Length
    // Followed by neighbor entries
} cord_isis_iis_neighbors_tlv_t;

// IS-IS IIS Neighbor entry
typedef struct CORD_PACKED cord_isis_iis_neighbor {
    uint8_t neighbor_id[6];     // Neighbor ID
    uint8_t default_metric;     // Default Metric
    uint8_t delay_metric;       // Delay Metric (optional)
    uint8_t expense_metric;     // Expense Metric (optional)
    uint8_t error_metric;       // Error Metric (optional)
} cord_isis_iis_neighbor_t;

// IS-IS ES Neighbors TLV (Type 3)
typedef struct CORD_PACKED cord_isis_es_neighbors_tlv {
    uint8_t type;               // TLV Type (3)
    uint8_t length;             // TLV Length
    // Followed by ES neighbor entries
} cord_isis_es_neighbors_tlv_t;

// IS-IS Partition Designated Level 2 IS TLV (Type 4)
typedef struct CORD_PACKED cord_isis_partition_dis_tlv {
    uint8_t type;               // TLV Type (4)
    uint8_t length;             // TLV Length
    uint8_t system_id[6];       // System ID
} cord_isis_partition_dis_tlv_t;

// IS-IS Prefix Neighbors TLV (Type 5)
typedef struct CORD_PACKED cord_isis_prefix_neighbors_tlv {
    uint8_t type;               // TLV Type (5)
    uint8_t length;             // TLV Length
    // Followed by prefix neighbor entries
} cord_isis_prefix_neighbors_tlv_t;

// IS-IS Authentication TLV (Type 10)
typedef struct CORD_PACKED cord_isis_auth_tlv {
    uint8_t type;               // TLV Type (10)
    uint8_t length;             // TLV Length
    uint8_t auth_type;          // Authentication Type
    uint8_t auth_value[];       // Authentication Value
} cord_isis_auth_tlv_t;

// IS-IS LSP Entries TLV (Type 9)
typedef struct CORD_PACKED cord_isis_lsp_entries_tlv {
    uint8_t type;               // TLV Type (9)
    uint8_t length;             // TLV Length
    // Followed by LSP entries
} cord_isis_lsp_entries_tlv_t;

// IS-IS LSP Entry
typedef struct CORD_PACKED cord_isis_lsp_entry {
    uint16_t remaining_lifetime; // Remaining Lifetime
    uint8_t lsp_id[8];          // LSP ID
    uint32_t sequence_number;   // LSP Sequence Number
    uint16_t checksum;          // LSP Checksum
} cord_isis_lsp_entry_t;

// IS-IS Extended IS Reachability TLV (Type 22)
typedef struct CORD_PACKED cord_isis_extended_is_reach_tlv {
    uint8_t type;               // TLV Type (22)
    uint8_t length;             // TLV Length
    // Followed by extended IS reachability entries
} cord_isis_extended_is_reach_tlv_t;

// IS-IS Extended IS Reachability entry
typedef struct CORD_PACKED cord_isis_extended_is_reach {
    uint8_t neighbor_id[7];     // Neighbor System ID + Circuit ID
    uint8_t metric[3];          // Metric (24 bits)
    uint8_t sub_tlv_length;     // Sub-TLV Length
    // Followed by sub-TLVs
} cord_isis_extended_is_reach_t;

// IS-IS IP Internal Reachability TLV (Type 128)
typedef struct CORD_PACKED cord_isis_ip_internal_reach_tlv {
    uint8_t type;               // TLV Type (128)
    uint8_t length;             // TLV Length
    // Followed by IP reachability entries
} cord_isis_ip_internal_reach_tlv_t;

// IS-IS IP Internal Reachability entry
typedef struct CORD_PACKED cord_isis_ip_internal_reach {
    uint8_t default_metric;     // Default Metric
    uint8_t delay_metric;       // Delay Metric (optional)
    uint8_t expense_metric;     // Expense Metric (optional) 
    uint8_t error_metric;       // Error Metric (optional)
    cord_ipv4_addr_t ip_address; // IP Address
    cord_ipv4_addr_t subnet_mask; // Subnet Mask
} cord_isis_ip_internal_reach_t;

// IS-IS IP External Reachability TLV (Type 130)
typedef struct CORD_PACKED cord_isis_ip_external_reach_tlv {
    uint8_t type;               // TLV Type (130)
    uint8_t length;             // TLV Length
    // Followed by IP external reachability entries (same format as internal)
} cord_isis_ip_external_reach_tlv_t;

// IS-IS Extended IP Reachability TLV (Type 135)
typedef struct CORD_PACKED cord_isis_extended_ip_reach_tlv {
    uint8_t type;               // TLV Type (135)
    uint8_t length;             // TLV Length
    // Followed by extended IP reachability entries
} cord_isis_extended_ip_reach_tlv_t;

// IS-IS Extended IP Reachability entry
typedef struct CORD_PACKED cord_isis_extended_ip_reach {
    uint32_t metric;            // Metric
    uint8_t control_info;       // Up/Down bit + Sub-TLV Length
    uint8_t prefix_length;      // Prefix Length
    // Followed by prefix (variable length) and sub-TLVs
} cord_isis_extended_ip_reach_t;

// IS-IS IPv6 Reachability TLV (Type 236)
typedef struct CORD_PACKED cord_isis_ipv6_reach_tlv {
    uint8_t type;               // TLV Type (236)
    uint8_t length;             // TLV Length
    // Followed by IPv6 reachability entries
} cord_isis_ipv6_reach_tlv_t;

// IS-IS IPv6 Reachability entry
typedef struct CORD_PACKED cord_isis_ipv6_reach {
    uint32_t metric;            // Metric
    uint8_t control_info;       // Up/Down bit + External bit + Sub-TLV Length
    uint8_t prefix_length;      // Prefix Length
    // Followed by prefix (variable length) and sub-TLVs
} cord_isis_ipv6_reach_t;

// EIGRP (Enhanced Interior Gateway Routing Protocol) header
typedef struct CORD_PACKED cord_eigrp_hdr {
    uint8_t version;            // Version
    uint8_t opcode;             // Opcode
    uint16_t checksum;          // Checksum
    uint32_t flags;             // Flags
    uint32_t sequence;          // Sequence Number
    uint32_t acknowledge;       // Acknowledge Number
    uint32_t as_number;         // Autonomous System Number
} cord_eigrp_hdr_t;

// EIGRP TLV header
typedef struct CORD_PACKED cord_eigrp_tlv {
    uint16_t type;              // Type
    uint16_t length;            // Length
    uint8_t data[];             // Variable length data
} cord_eigrp_tlv_t;

// PIM (Protocol Independent Multicast) header
typedef struct CORD_PACKED cord_pim_hdr {
    uint8_t ver_type;           // Version:4, Type:4
    uint8_t reserved;           // Reserved
    uint16_t checksum;          // Checksum
} cord_pim_hdr_t;

// IGMP (Internet Group Management Protocol) extended
typedef struct CORD_PACKED cord_igmpv3_query {
    uint8_t type;               // Type
    uint8_t code;               // Max Response Code
    uint16_t checksum;          // Checksum
    cord_ipv4_addr_t group_addr; // Group Address
    uint8_t resv_s_qrv;         // Resv:4, S:1, QRV:3
    uint8_t qqic;               // Querier's Query Interval Code
    uint16_t num_sources;       // Number of Sources
    // Followed by source addresses
} cord_igmpv3_query_t;

// HSRP (Hot Standby Router Protocol) header
typedef struct CORD_PACKED cord_hsrp_hdr {
    uint8_t version;            // Version
    uint8_t op_code;            // Op Code
    uint8_t state;              // State
    uint8_t hellotime;          // Hello Time
    uint8_t holdtime;           // Hold Time
    uint8_t priority;           // Priority
    uint8_t group;              // Group
    uint8_t reserved;           // Reserved
    uint8_t auth_data[8];       // Authentication Data
    cord_ipv4_addr_t virtual_ip; // Virtual IP Address
} cord_hsrp_hdr_t;

// VRRP (Virtual Router Redundancy Protocol) header
typedef struct CORD_PACKED cord_vrrp_hdr {
    uint8_t version_type;       // Version:4, Type:4
    uint8_t virtual_rtr_id;     // Virtual Router ID
    uint8_t priority;           // Priority
    uint8_t count_ip;           // Count IP Addresses
    uint8_t auth_type;          // Authentication Type
    uint8_t adver_int;          // Advertisement Interval
    uint16_t checksum;          // Checksum
    // Followed by IP addresses and authentication data
} cord_vrrp_hdr_t;

// LDP (Label Distribution Protocol) header
typedef struct CORD_PACKED cord_ldp_hdr {
    uint16_t version;           // Version
    uint16_t pdu_length;        // PDU Length
    cord_ipv4_addr_t lsr_id;    // LSR ID
    uint16_t label_space;       // Label Space ID
} cord_ldp_hdr_t;

// RSVP (Resource Reservation Protocol) common header
typedef struct CORD_PACKED cord_rsvp_hdr {
    uint8_t version_flags;      // Version:4, Flags:4
    uint8_t msg_type;           // Message Type
    uint16_t checksum;          // Checksum
    uint8_t send_ttl;           // Send TTL
    uint8_t reserved;           // Reserved
    uint16_t rsvp_length;       // RSVP Length
} cord_rsvp_hdr_t;

// BFD (Bidirectional Forwarding Detection) header
typedef struct CORD_PACKED cord_bfd_hdr {
    uint8_t vers_diag;          // Version:3, Diag:5
    uint8_t state_flags;        // State:2, Poll:1, Final:1, Control Plane Independent:1, Auth Present:1, Demand:1, Multipoint:1
    uint8_t detect_mult;        // Detect Mult
    uint8_t length;             // Length
    uint32_t my_discriminator;  // My Discriminator
    uint32_t your_discriminator; // Your Discriminator
    uint32_t desired_min_tx_interval;   // Desired Min TX Interval
    uint32_t required_min_rx_interval;  // Required Min RX Interval
    uint32_t required_min_echo_rx_interval; // Required Min Echo RX Interval
} cord_bfd_hdr_t;

// =============================================================================
// PROTOCOL CONSTANTS AND DEFINITIONS
// =============================================================================

// OSPF Protocol Constants
#define CORD_OSPF_VERSION           2
#define CORD_OSPF_TYPE_HELLO        1
#define CORD_OSPF_TYPE_DB_DESC      2
#define CORD_OSPF_TYPE_LS_REQ       3
#define CORD_OSPF_TYPE_LS_UPD       4
#define CORD_OSPF_TYPE_LS_ACK       5

// OSPF LSA Types
#define CORD_OSPF_LSA_ROUTER        1   // Router LSA
#define CORD_OSPF_LSA_NETWORK       2   // Network LSA
#define CORD_OSPF_LSA_SUMMARY_NET   3   // Network Summary LSA
#define CORD_OSPF_LSA_SUMMARY_ASBR  4   // ASBR Summary LSA
#define CORD_OSPF_LSA_EXTERNAL      5   // AS External LSA
#define CORD_OSPF_LSA_GROUP         6   // Group Membership LSA
#define CORD_OSPF_LSA_NSSA          7   // NSSA LSA
#define CORD_OSPF_LSA_LINK_LOCAL    9   // Link-Local Opaque LSA
#define CORD_OSPF_LSA_AREA_LOCAL    10  // Area-Local Opaque LSA
#define CORD_OSPF_LSA_AS_EXTERNAL   11  // AS-External Opaque LSA

// OSPF Router LSA Link Types
#define CORD_OSPF_LINK_PTP          1   // Point-to-Point
#define CORD_OSPF_LINK_TRANSIT      2   // Transit Network
#define CORD_OSPF_LINK_STUB         3   // Stub Network
#define CORD_OSPF_LINK_VIRTUAL      4   // Virtual Link

// OSPF Router LSA Flags
#define CORD_OSPF_ROUTER_FLAG_V     0x04    // Virtual Link Endpoint
#define CORD_OSPF_ROUTER_FLAG_E     0x02    // External
#define CORD_OSPF_ROUTER_FLAG_B     0x01    // Border

// OSPF Database Description Flags
#define CORD_OSPF_DD_FLAG_I         0x04    // Initial
#define CORD_OSPF_DD_FLAG_M         0x02    // More
#define CORD_OSPF_DD_FLAG_MS        0x01    // Master/Slave

// BGP Protocol Constants
#define CORD_BGP_VERSION            4
#define CORD_BGP_TYPE_OPEN          1
#define CORD_BGP_TYPE_UPDATE        2
#define CORD_BGP_TYPE_NOTIFICATION  3
#define CORD_BGP_TYPE_KEEPALIVE     4
#define CORD_BGP_TYPE_ROUTE_REFRESH 5

// BGP Path Attribute Types
#define CORD_BGP_ATTR_ORIGIN        1   // ORIGIN
#define CORD_BGP_ATTR_AS_PATH       2   // AS_PATH
#define CORD_BGP_ATTR_NEXT_HOP      3   // NEXT_HOP
#define CORD_BGP_ATTR_MED           4   // MULTI_EXIT_DISC
#define CORD_BGP_ATTR_LOCAL_PREF    5   // LOCAL_PREF
#define CORD_BGP_ATTR_ATOMIC_AGGR   6   // ATOMIC_AGGREGATE
#define CORD_BGP_ATTR_AGGREGATOR    7   // AGGREGATOR
#define CORD_BGP_ATTR_COMMUNITIES   8   // COMMUNITIES
#define CORD_BGP_ATTR_ORIGINATOR_ID 9   // ORIGINATOR_ID
#define CORD_BGP_ATTR_CLUSTER_LIST  10  // CLUSTER_LIST
#define CORD_BGP_ATTR_DPA           11  // DPA (deprecated)
#define CORD_BGP_ATTR_ADVERTISER    12  // ADVERTISER (deprecated)
#define CORD_BGP_ATTR_RCID_PATH     13  // RCID_PATH (deprecated)
#define CORD_BGP_ATTR_MP_REACH_NLRI 14  // MP_REACH_NLRI
#define CORD_BGP_ATTR_MP_UNREACH_NLRI 15 // MP_UNREACH_NLRI
#define CORD_BGP_ATTR_EXT_COMMUNITIES 16 // EXTENDED_COMMUNITIES
#define CORD_BGP_ATTR_AS4_PATH      17  // AS4_PATH
#define CORD_BGP_ATTR_AS4_AGGREGATOR 18 // AS4_AGGREGATOR
#define CORD_BGP_ATTR_PMSI_TUNNEL   22  // PMSI_TUNNEL
#define CORD_BGP_ATTR_TUNNEL_ENCAP  23  // TUNNEL_ENCAPSULATION
#define CORD_BGP_ATTR_LARGE_COMM    32  // LARGE_COMMUNITIES
#define CORD_BGP_ATTR_BGPSEC_PATH   33  // BGPsec_PATH

// BGP Path Attribute Flags
#define CORD_BGP_ATTR_FLAG_OPTIONAL     0x80    // Optional
#define CORD_BGP_ATTR_FLAG_TRANSITIVE   0x40    // Transitive
#define CORD_BGP_ATTR_FLAG_PARTIAL      0x20    // Partial
#define CORD_BGP_ATTR_FLAG_EXTENDED     0x10    // Extended Length

// BGP Origin Values
#define CORD_BGP_ORIGIN_IGP             0   // IGP
#define CORD_BGP_ORIGIN_EGP             1   // EGP
#define CORD_BGP_ORIGIN_INCOMPLETE      2   // Incomplete

// BGP AS Path Segment Types
#define CORD_BGP_AS_SET                 1   // AS_SET
#define CORD_BGP_AS_SEQUENCE            2   // AS_SEQUENCE
#define CORD_BGP_AS_CONFED_SEQUENCE     3   // AS_CONFED_SEQUENCE
#define CORD_BGP_AS_CONFED_SET          4   // AS_CONFED_SET

// BGP Error Codes
#define CORD_BGP_ERROR_MSG_HDR          1   // Message Header Error
#define CORD_BGP_ERROR_OPEN_MSG         2   // OPEN Message Error
#define CORD_BGP_ERROR_UPDATE_MSG       3   // UPDATE Message Error
#define CORD_BGP_ERROR_HOLD_TIMER       4   // Hold Timer Expired
#define CORD_BGP_ERROR_FSM              5   // Finite State Machine Error
#define CORD_BGP_ERROR_CEASE            6   // Cease

// BGP Well-known Communities
#define CORD_BGP_COMMUNITY_NO_EXPORT        0xFFFFFF01
#define CORD_BGP_COMMUNITY_NO_ADVERTISE     0xFFFFFF02
#define CORD_BGP_COMMUNITY_NO_EXPORT_SUBCONFED 0xFFFFFF03
#define CORD_BGP_COMMUNITY_NOPEER           0xFFFFFF04

// RIP Protocol Constants
#define CORD_RIP_COMMAND_REQUEST        1
#define CORD_RIP_COMMAND_RESPONSE       2
#define CORD_RIP_COMMAND_TRACE_ON       3   // Obsolete
#define CORD_RIP_COMMAND_TRACE_OFF      4   // Obsolete
#define CORD_RIP_COMMAND_RESERVED       5   // Sun Internal
#define CORD_RIP_VERSION_1              1
#define CORD_RIP_VERSION_2              2

// RIP Address Family Constants
#define CORD_RIP_AF_INET                2   // IPv4
#define CORD_RIP_AF_AUTH                0xFFFF  // Authentication

// RIP Authentication Types
#define CORD_RIP_AUTH_NONE              0   // No Authentication
#define CORD_RIP_AUTH_SIMPLE            2   // Simple Password
#define CORD_RIP_AUTH_MD5               3   // Keyed MD5

// RIP Metric Constants
#define CORD_RIP_METRIC_INFINITY        16  // Infinite metric

// RIPng Constants
#define CORD_RIPNG_VERSION              1
#define CORD_RIPNG_PORT                 521
#define CORD_RIPNG_METRIC_INFINITY      16

// IS-IS Protocol Constants
#define CORD_ISIS_PROTO_DISCRIMINATOR   0x83    // ISO 10589
#define CORD_ISIS_VERSION               1
#define CORD_ISIS_ID_LENGTH             6       // System ID Length

// IS-IS PDU Types
#define CORD_ISIS_PDU_L1_LAN_IIH        15      // Level-1 LAN IIH
#define CORD_ISIS_PDU_L2_LAN_IIH        16      // Level-2 LAN IIH
#define CORD_ISIS_PDU_PTP_IIH           17      // Point-to-Point IIH
#define CORD_ISIS_PDU_L1_LSP            18      // Level-1 LSP
#define CORD_ISIS_PDU_L2_LSP            20      // Level-2 LSP
#define CORD_ISIS_PDU_L1_CSNP           24      // Level-1 Complete SNP
#define CORD_ISIS_PDU_L2_CSNP           25      // Level-2 Complete SNP
#define CORD_ISIS_PDU_L1_PSNP           26      // Level-1 Partial SNP
#define CORD_ISIS_PDU_L2_PSNP           27      // Level-2 Partial SNP

// IS-IS Circuit Types
#define CORD_ISIS_CIRCUIT_L1_ONLY       1       // Level-1 only
#define CORD_ISIS_CIRCUIT_L2_ONLY       2       // Level-2 only
#define CORD_ISIS_CIRCUIT_L1L2          3       // Level-1-2

// IS-IS LSP Flags
#define CORD_ISIS_LSP_FLAG_PARTITION    0x80    // Partition Repair
#define CORD_ISIS_LSP_FLAG_ATT_ERROR    0x08    // Attached Error
#define CORD_ISIS_LSP_FLAG_ATT_EXPENSE  0x10    // Attached Expense
#define CORD_ISIS_LSP_FLAG_ATT_DELAY    0x20    // Attached Delay
#define CORD_ISIS_LSP_FLAG_ATT_DEFAULT  0x40    // Attached Default
#define CORD_ISIS_LSP_FLAG_OVERLOAD     0x04    // Overload
#define CORD_ISIS_LSP_FLAG_IS_TYPE      0x03    // IS Type mask

// IS-IS TLV Types
#define CORD_ISIS_TLV_AREA_ADDR         1       // Area Addresses
#define CORD_ISIS_TLV_IIS_NEIGHBORS     2       // IIS Neighbors
#define CORD_ISIS_TLV_ES_NEIGHBORS      3       // ES Neighbors
#define CORD_ISIS_TLV_PARTITION_DIS     4       // Partition Designated L2 IS
#define CORD_ISIS_TLV_PREFIX_NEIGHBORS  5       // Prefix Neighbors
#define CORD_ISIS_TLV_IIS_NEIGHBORS_VAR 6       // IIS Neighbors (Variable Length)
#define CORD_ISIS_TLV_PADDING           8       // Padding
#define CORD_ISIS_TLV_LSP_ENTRIES       9       // LSP Entries
#define CORD_ISIS_TLV_AUTHENTICATION    10      // Authentication
#define CORD_ISIS_TLV_LSP_BUFFERSIZE    14      // LSP Buffer Size
#define CORD_ISIS_TLV_EXTENDED_IS_REACH 22      // Extended IS Reachability
#define CORD_ISIS_TLV_IS_ALIAS          24      // IS Alias
#define CORD_ISIS_TLV_IP_INTERNAL_REACH 128     // IP Internal Reachability
#define CORD_ISIS_TLV_PROTOCOLS_SUPP    129     // Protocols Supported
#define CORD_ISIS_TLV_IP_EXTERNAL_REACH 130     // IP External Reachability
#define CORD_ISIS_TLV_IDRP_INFO         131     // IDRP Information
#define CORD_ISIS_TLV_IP_INTERFACE_ADDR 132     // IP Interface Address
#define CORD_ISIS_TLV_TE_ROUTER_ID      134     // TE Router ID
#define CORD_ISIS_TLV_EXTENDED_IP_REACH 135     // Extended IP Reachability
#define CORD_ISIS_TLV_HOSTNAME          137     // Dynamic Hostname
#define CORD_ISIS_TLV_SHARED_RISK_GROUP 138     // Shared Risk Link Group
#define CORD_ISIS_TLV_IPV6_INTERFACE_ADDR 232   // IPv6 Interface Address
#define CORD_ISIS_TLV_IPV6_REACH        236     // IPv6 Reachability
#define CORD_ISIS_TLV_MULTI_TOPOLOGY    229     // Multi-Topology
#define CORD_ISIS_TLV_RESTART           211     // Restart Signaling

// IS-IS Authentication Types
#define CORD_ISIS_AUTH_CLEARTEXT        1       // Cleartext Password
#define CORD_ISIS_AUTH_HMAC_MD5         54      // HMAC-MD5

// EIGRP Protocol Constants
#define CORD_EIGRP_VERSION              2
#define CORD_EIGRP_OPCODE_UPDATE        1
#define CORD_EIGRP_OPCODE_REQUEST       2
#define CORD_EIGRP_OPCODE_QUERY         3
#define CORD_EIGRP_OPCODE_REPLY         4
#define CORD_EIGRP_OPCODE_HELLO         5
#define CORD_EIGRP_OPCODE_IPXSAP        6
#define CORD_EIGRP_OPCODE_PROBE         7
#define CORD_EIGRP_OPCODE_ACK           8
#define CORD_EIGRP_OPCODE_STUB_PROBE    10
#define CORD_EIGRP_OPCODE_STUB_REPLY    11

// EIGRP Flags
#define CORD_EIGRP_FLAG_INIT            0x01    // Init Flag
#define CORD_EIGRP_FLAG_CR              0x02    // Conditional Receive
#define CORD_EIGRP_FLAG_RS              0x04    // Restart Flag
#define CORD_EIGRP_FLAG_EOT             0x08    // End of Table

// EIGRP TLV Types
#define CORD_EIGRP_TLV_PARAMETER        0x0001  // EIGRP Parameters
#define CORD_EIGRP_TLV_AUTH             0x0002  // Authentication
#define CORD_EIGRP_TLV_SEQUENCE         0x0003  // Sequence
#define CORD_EIGRP_TLV_SOFTWARE_VERSION 0x0004  // Software Version
#define CORD_EIGRP_TLV_MULTICAST_SEQ    0x0005  // Next Multicast Sequence
#define CORD_EIGRP_TLV_PEER_STUB_INFO   0x0006  // Peer Stub Information
#define CORD_EIGRP_TLV_PEER_TERMINATION 0x0007  // Peer Termination
#define CORD_EIGRP_TLV_IPV4_INTERNAL    0x0102  // IPv4 Internal Routes
#define CORD_EIGRP_TLV_IPV4_EXTERNAL    0x0103  // IPv4 External Routes
#define CORD_EIGRP_TLV_IPV6_INTERNAL    0x0402  // IPv6 Internal Routes
#define CORD_EIGRP_TLV_IPV6_EXTERNAL    0x0403  // IPv6 External Routes

// PIM Protocol Constants
#define CORD_PIM_VERSION                2
#define CORD_PIM_TYPE_HELLO             0
#define CORD_PIM_TYPE_REGISTER          1
#define CORD_PIM_TYPE_REGISTER_STOP     2
#define CORD_PIM_TYPE_JOIN_PRUNE        3
#define CORD_PIM_TYPE_BOOTSTRAP         4
#define CORD_PIM_TYPE_ASSERT            5
#define CORD_PIM_TYPE_GRAFT             6       // PIM-DM
#define CORD_PIM_TYPE_GRAFT_ACK         7       // PIM-DM
#define CORD_PIM_TYPE_CANDIDATE_RP_ADV  8       // PIM-SM
#define CORD_PIM_TYPE_STATE_REFRESH     9       // PIM-DM
#define CORD_PIM_TYPE_DF_ELECTION       10      // BIDIR-PIM

// PIM Hello Option Types
#define CORD_PIM_HELLO_HOLDTIME         1
#define CORD_PIM_HELLO_LAN_PRUNE_DELAY  2
#define CORD_PIM_HELLO_DR_PRIORITY      19
#define CORD_PIM_HELLO_GENERATION_ID    20
#define CORD_PIM_HELLO_STATE_REFRESH    21
#define CORD_PIM_HELLO_BIDIR_CAPABLE    22

// IGMP Protocol Constants
#define CORD_IGMP_TYPE_MEMBERSHIP_QUERY     0x11    // IGMPv1/v2/v3
#define CORD_IGMP_TYPE_V1_MEMBERSHIP_REPORT 0x12    // IGMPv1
#define CORD_IGMP_TYPE_V2_MEMBERSHIP_REPORT 0x16    // IGMPv2
#define CORD_IGMP_TYPE_V2_LEAVE_GROUP       0x17    // IGMPv2
#define CORD_IGMP_TYPE_V3_MEMBERSHIP_REPORT 0x22    // IGMPv3

// HSRP Protocol Constants
#define CORD_HSRP_VERSION_1             0
#define CORD_HSRP_VERSION_2             1
#define CORD_HSRP_OP_HELLO              0
#define CORD_HSRP_OP_COUP               1
#define CORD_HSRP_OP_RESIGN             2
#define CORD_HSRP_STATE_INITIAL         0
#define CORD_HSRP_STATE_LEARN           1
#define CORD_HSRP_STATE_LISTEN          2
#define CORD_HSRP_STATE_SPEAK           4
#define CORD_HSRP_STATE_STANDBY         8
#define CORD_HSRP_STATE_ACTIVE          16

// VRRP Protocol Constants
#define CORD_VRRP_VERSION_2             2
#define CORD_VRRP_VERSION_3             3
#define CORD_VRRP_TYPE_ADVERTISEMENT    1
#define CORD_VRRP_AUTH_NONE             0
#define CORD_VRRP_AUTH_SIMPLE           1
#define CORD_VRRP_AUTH_IP_AUTH_HDR      2

// VRRP States
#define CORD_VRRP_STATE_INITIALIZE      0
#define CORD_VRRP_STATE_BACKUP          1
#define CORD_VRRP_STATE_MASTER          2

// BFD Protocol Constants
#define CORD_BFD_VERSION                1
#define CORD_BFD_STATE_ADMINDOWN        0
#define CORD_BFD_STATE_DOWN             1
#define CORD_BFD_STATE_INIT             2
#define CORD_BFD_STATE_UP               3

// BFD Diagnostic Codes
#define CORD_BFD_DIAG_NO_DIAGNOSTIC             0
#define CORD_BFD_DIAG_CTRL_DETECT_EXPIRED       1
#define CORD_BFD_DIAG_ECHO_FUNC_FAILED          2
#define CORD_BFD_DIAG_NEIGHBOR_SIGNALED_DOWN    3
#define CORD_BFD_DIAG_FORWARDING_PLANE_RESET    4
#define CORD_BFD_DIAG_PATH_DOWN                 5
#define CORD_BFD_DIAG_CONCAT_PATH_DOWN          6
#define CORD_BFD_DIAG_ADMIN_DOWN                7
#define CORD_BFD_DIAG_REV_CONCAT_PATH_DOWN      8

// BFD Flags
#define CORD_BFD_FLAG_POLL              0x20    // Poll
#define CORD_BFD_FLAG_FINAL             0x10    // Final
#define CORD_BFD_FLAG_CTRL_PLANE_INDEP  0x08    // Control Plane Independent
#define CORD_BFD_FLAG_AUTH_PRESENT      0x04    // Authentication Present
#define CORD_BFD_FLAG_DEMAND            0x02    // Demand
#define CORD_BFD_FLAG_MULTIPOINT        0x01    // Multipoint

// LDP Protocol Constants
#define CORD_LDP_VERSION                1
#define CORD_LDP_MSG_NOTIFICATION       0x0001
#define CORD_LDP_MSG_HELLO              0x0100
#define CORD_LDP_MSG_INITIALIZATION     0x0200
#define CORD_LDP_MSG_KEEPALIVE          0x0201
#define CORD_LDP_MSG_ADDRESS            0x0300
#define CORD_LDP_MSG_ADDRESS_WITHDRAW   0x0301
#define CORD_LDP_MSG_LABEL_MAPPING      0x0400
#define CORD_LDP_MSG_LABEL_REQUEST      0x0401
#define CORD_LDP_MSG_LABEL_WITHDRAW     0x0402
#define CORD_LDP_MSG_LABEL_RELEASE      0x0403
#define CORD_LDP_MSG_LABEL_ABORT_REQ    0x0404

// RSVP Protocol Constants
#define CORD_RSVP_VERSION               1
#define CORD_RSVP_MSG_PATH              1
#define CORD_RSVP_MSG_RESV              2
#define CORD_RSVP_MSG_PATHERR           3
#define CORD_RSVP_MSG_RESVERR           4
#define CORD_RSVP_MSG_PATHTEAR          5
#define CORD_RSVP_MSG_RESVTEAR          6
#define CORD_RSVP_MSG_RESVCONF          7

// RSVP-TE Message Types
#define CORD_RSVP_MSG_BUNDLE            12
#define CORD_RSVP_MSG_ACK               13
#define CORD_RSVP_MSG_SREFRESH          15

// Common Port Numbers for Routing Protocols
#define CORD_PORT_RIP                   520     // RIP
#define CORD_PORT_RIPNG                 521     // RIPng
#define CORD_PORT_BGP                   179     // BGP
#define CORD_PORT_OSPF                  89      // OSPF (IP Protocol)
#define CORD_PORT_LDP                   646     // LDP
#define CORD_PORT_BFD_CONTROL           3784    // BFD Control
#define CORD_PORT_BFD_ECHO              3785    // BFD Echo
#define CORD_PORT_RSVP                  46      // RSVP (IP Protocol)
#define CORD_PORT_PIM                   103     // PIM (IP Protocol)
#define CORD_PORT_VRRP                  112     // VRRP (IP Protocol)
#define CORD_PORT_HSRP                  1985    // HSRP (UDP)

// =============================================================================
// HELPER MACROS FOR PROTOCOL FIELDS
// =============================================================================

// OSPF Helper Macros
#define CORD_OSPF_GET_LSA_AGE(lsa)          cord_ntohs((lsa)->ls_age)
#define CORD_OSPF_GET_LSA_TYPE(lsa)         ((lsa)->ls_type)
#define CORD_OSPF_GET_LSA_SEQNUM(lsa)       cord_ntohl((lsa)->ls_seqnum)
#define CORD_OSPF_GET_ROUTER_FLAG_V(rtr)    (((rtr)->flags & CORD_OSPF_ROUTER_FLAG_V) != 0)
#define CORD_OSPF_GET_ROUTER_FLAG_E(rtr)    (((rtr)->flags & CORD_OSPF_ROUTER_FLAG_E) != 0)
#define CORD_OSPF_GET_ROUTER_FLAG_B(rtr)    (((rtr)->flags & CORD_OSPF_ROUTER_FLAG_B) != 0)
#define CORD_OSPF_GET_DD_FLAG_I(dd)         (((dd)->flags & CORD_OSPF_DD_FLAG_I) != 0)
#define CORD_OSPF_GET_DD_FLAG_M(dd)         (((dd)->flags & CORD_OSPF_DD_FLAG_M) != 0)
#define CORD_OSPF_GET_DD_FLAG_MS(dd)        (((dd)->flags & CORD_OSPF_DD_FLAG_MS) != 0)

// BGP Helper Macros
#define CORD_BGP_GET_ATTR_FLAG_OPTIONAL(attr)   (((attr)->flags & CORD_BGP_ATTR_FLAG_OPTIONAL) != 0)
#define CORD_BGP_GET_ATTR_FLAG_TRANSITIVE(attr) (((attr)->flags & CORD_BGP_ATTR_FLAG_TRANSITIVE) != 0)
#define CORD_BGP_GET_ATTR_FLAG_PARTIAL(attr)    (((attr)->flags & CORD_BGP_ATTR_FLAG_PARTIAL) != 0)
#define CORD_BGP_GET_ATTR_FLAG_EXTENDED(attr)   (((attr)->flags & CORD_BGP_ATTR_FLAG_EXTENDED) != 0)
#define CORD_BGP_GET_AS_PATH_TYPE(seg)          ((seg)->type)
#define CORD_BGP_GET_AS_PATH_LENGTH(seg)        ((seg)->length)

// IS-IS Helper Macros
#define CORD_ISIS_GET_PDU_TYPE(isis)        ((isis)->pdu_type)
#define CORD_ISIS_GET_CIRCUIT_TYPE(hello)   ((hello)->circuit_type)
#define CORD_ISIS_GET_LSP_FLAG_P(lsp)       (((lsp)->p_att_ol_is_type & CORD_ISIS_LSP_FLAG_PARTITION) != 0)
#define CORD_ISIS_GET_LSP_FLAG_ATT(lsp)     (((lsp)->p_att_ol_is_type & 0x78) >> 3)
#define CORD_ISIS_GET_LSP_FLAG_OL(lsp)      (((lsp)->p_att_ol_is_type & CORD_ISIS_LSP_FLAG_OVERLOAD) != 0)
#define CORD_ISIS_GET_LSP_IS_TYPE(lsp)      ((lsp)->p_att_ol_is_type & CORD_ISIS_LSP_FLAG_IS_TYPE)
#define CORD_ISIS_GET_TLV_TYPE(tlv)         ((tlv)->type)
#define CORD_ISIS_GET_TLV_LENGTH(tlv)       ((tlv)->length)

// RIP Helper Macros
#define CORD_RIP_GET_COMMAND(rip)           ((rip)->command)
#define CORD_RIP_GET_VERSION(rip)           ((rip)->version)
#define CORD_RIP_GET_AF(entry)              cord_ntohs((entry)->address_family)
#define CORD_RIP_GET_METRIC(entry)          cord_ntohl((entry)->metric)
#define CORD_RIP_IS_AUTH_ENTRY(entry)       (cord_ntohs((entry)->address_family) == CORD_RIP_AF_AUTH)

// EIGRP Helper Macros
#define CORD_EIGRP_GET_VERSION(eigrp)       ((eigrp)->version)
#define CORD_EIGRP_GET_OPCODE(eigrp)        ((eigrp)->opcode)
#define CORD_EIGRP_GET_FLAGS(eigrp)         cord_ntohl((eigrp)->flags)
#define CORD_EIGRP_GET_SEQ(eigrp)           cord_ntohl((eigrp)->sequence)
#define CORD_EIGRP_GET_ACK(eigrp)           cord_ntohl((eigrp)->acknowledge)
#define CORD_EIGRP_GET_AS(eigrp)            cord_ntohl((eigrp)->as_number)
#define CORD_EIGRP_GET_TLV_TYPE(tlv)        cord_ntohs((tlv)->type)
#define CORD_EIGRP_GET_TLV_LENGTH(tlv)      cord_ntohs((tlv)->length)

// PIM Helper Macros
#define CORD_PIM_GET_VERSION(pim)           (((pim)->ver_type & 0xF0) >> 4)
#define CORD_PIM_GET_TYPE(pim)              ((pim)->ver_type & 0x0F)

// HSRP Helper Macros
#define CORD_HSRP_GET_VERSION(hsrp)         ((hsrp)->version)
#define CORD_HSRP_GET_OPCODE(hsrp)          ((hsrp)->op_code)
#define CORD_HSRP_GET_STATE(hsrp)           ((hsrp)->state)

// VRRP Helper Macros
#define CORD_VRRP_GET_VERSION(vrrp)         (((vrrp)->version_type & 0xF0) >> 4)
#define CORD_VRRP_GET_TYPE(vrrp)            ((vrrp)->version_type & 0x0F)
#define CORD_VRRP_GET_VRID(vrrp)            ((vrrp)->virtual_rtr_id)
#define CORD_VRRP_GET_PRIORITY(vrrp)        ((vrrp)->priority)

// BFD Helper Macros
#define CORD_BFD_GET_VERSION(bfd)           (((bfd)->vers_diag & 0xE0) >> 5)
#define CORD_BFD_GET_DIAG(bfd)              ((bfd)->vers_diag & 0x1F)
#define CORD_BFD_GET_STATE(bfd)             (((bfd)->state_flags & 0xC0) >> 6)
#define CORD_BFD_GET_FLAGS(bfd)             ((bfd)->state_flags & 0x3F)
#define CORD_BFD_GET_DETECT_MULT(bfd)       ((bfd)->detect_mult)
#define CORD_BFD_GET_LENGTH(bfd)            ((bfd)->length)
#define CORD_BFD_GET_MY_DISC(bfd)           cord_ntohl((bfd)->my_discriminator)
#define CORD_BFD_GET_YOUR_DISC(bfd)         cord_ntohl((bfd)->your_discriminator)

// LDP Helper Macros
#define CORD_LDP_GET_VERSION(ldp)           cord_ntohs((ldp)->version)
#define CORD_LDP_GET_PDU_LENGTH(ldp)        cord_ntohs((ldp)->pdu_length)
#define CORD_LDP_GET_LSR_ID(ldp)            cord_ntohl((ldp)->lsr_id.addr)
#define CORD_LDP_GET_LABEL_SPACE(ldp)       cord_ntohs((ldp)->label_space)

// RSVP Helper Macros
#define CORD_RSVP_GET_VERSION(rsvp)         (((rsvp)->version_flags & 0xF0) >> 4)
#define CORD_RSVP_GET_FLAGS(rsvp)           ((rsvp)->version_flags & 0x0F)
#define CORD_RSVP_GET_MSG_TYPE(rsvp)        ((rsvp)->msg_type)
#define CORD_RSVP_GET_LENGTH(rsvp)          cord_ntohs((rsvp)->rsvp_length)

// =============================================================================
// PROTOCOL VALIDATION FUNCTIONS
// =============================================================================

// Protocol version validation
bool cord_ospf_version_valid(uint8_t version);
bool cord_bgp_version_valid(uint8_t version);
bool cord_rip_version_valid(uint8_t version);
bool cord_isis_version_valid(uint8_t version);
bool cord_eigrp_version_valid(uint8_t version);
bool cord_bfd_version_valid(uint8_t version);

// Protocol type/opcode validation
bool cord_ospf_type_valid(uint8_t type);
bool cord_bgp_type_valid(uint8_t type);
bool cord_isis_pdu_type_valid(uint8_t pdu_type);
bool cord_eigrp_opcode_valid(uint8_t opcode);
bool cord_rip_command_valid(uint8_t command);

// LSA type validation
bool cord_ospf_lsa_type_valid(uint8_t lsa_type);

// BGP attribute type validation
bool cord_bgp_attr_type_valid(uint8_t attr_type);

// IS-IS TLV type validation
bool cord_isis_tlv_type_valid(uint8_t tlv_type);

// Metric validation
bool cord_rip_metric_valid(uint32_t metric);
bool cord_ospf_metric_valid(uint32_t metric);

#endif // CORD_ROUTING_PROTOCOLS_H