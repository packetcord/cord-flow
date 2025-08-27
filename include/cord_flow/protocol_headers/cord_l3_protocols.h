#ifndef CORD_L3_PROTOCOLS_H
#define CORD_L3_PROTOCOLS_H

#include "cord_protocol_common.h"

// IPv4 header - RFC 791 compliant
typedef struct CORD_PACKED cord_ipv4_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t ihl:4,              // Internet Header Length
            version:4;          // Version
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t version:4,          // Version
            ihl:4;              // Internet Header Length  
#else
#error "Please define __BYTE_ORDER"
#endif
    uint8_t tos;                // Type of Service
    uint16_t tot_len;           // Total Length
    uint16_t id;                // Identification
    uint16_t frag_off;          // Fragment Offset and Flags
    uint8_t ttl;                // Time to Live
    uint8_t protocol;           // Protocol
    uint16_t check;             // Header Checksum
    cord_ipv4_addr_t saddr;     // Source Address
    cord_ipv4_addr_t daddr;     // Destination Address
} cord_ipv4_hdr_t;

// IPv6 header - RFC 8200 compliant
typedef struct CORD_PACKED cord_ipv6_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t flow_label:20,         // Flow Label
             traffic_class:8,       // Traffic Class
             version:4;             // Version
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint32_t version:4,             // Version  
             traffic_class:8,       // Traffic Class
             flow_label:20;         // Flow Label
#else
#error "Please define __BYTE_ORDER"
#endif
    uint16_t payload_len;           // Payload Length
    uint8_t nexthdr;                // Next Header
    uint8_t hop_limit;              // Hop Limit
    cord_ipv6_addr_t saddr;         // Source Address
    cord_ipv6_addr_t daddr;         // Destination Address
} cord_ipv6_hdr_t;

// ICMPv4 header
typedef struct CORD_PACKED cord_icmp_hdr {
    uint8_t type;               // ICMP Type
    uint8_t code;               // ICMP Code
    uint16_t checksum;          // Checksum
    union {
        struct {
            uint16_t id;        // Identifier
            uint16_t sequence;  // Sequence Number
        } echo;
        uint32_t gateway;       // Gateway Address (for redirect)
        struct {
            uint16_t unused;    // Unused
            uint16_t mtu;       // MTU (for dest unreachable)
        } frag;
    } un;
} cord_icmp_hdr_t;

// ICMPv6 header
typedef struct CORD_PACKED cord_icmpv6_hdr {
    uint8_t type;               // ICMPv6 Type
    uint8_t code;               // ICMPv6 Code
    uint16_t checksum;          // Checksum
    uint32_t data;              // Type-specific data
} cord_icmpv6_hdr_t;

// IGMP header
typedef struct CORD_PACKED cord_igmp_hdr {
    uint8_t type;               // IGMP Type
    uint8_t code;               // Max Response Time / Reserved
    uint16_t csum;              // Checksum
    cord_ipv4_addr_t group;     // Group Address
} cord_igmp_hdr_t;

// IPv6 Extension Headers

// IPv6 Fragment header
typedef struct CORD_PACKED cord_ipv6_frag_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t reserved;           // Reserved
    uint16_t frag_off_res_m;    // Fragment Offset:13, Reserved:2, M Flag:1
    uint32_t identification;    // Identification
} cord_ipv6_frag_hdr_t;

// IPv6 Hop-by-Hop/Destination Options header
typedef struct CORD_PACKED cord_ipv6_opt_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t hdrlen;             // Header Extension Length
    uint8_t data[];             // Variable length options
} cord_ipv6_opt_hdr_t;

// IPv6 Routing header
typedef struct CORD_PACKED cord_ipv6_rt_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t hdrlen;             // Header Extension Length
    uint8_t type;               // Routing Type
    uint8_t segleft;            // Segments Left
    uint32_t reserved;          // Type-specific data
    // Followed by type-specific routing data
} cord_ipv6_rt_hdr_t;

// IPv6 Authentication header
typedef struct CORD_PACKED cord_ipv6_auth_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t hdrlen;             // Payload Length
    uint16_t reserved;          // Reserved
    uint32_t spi;               // Security Parameters Index
    uint32_t seq_no;            // Sequence Number
    uint8_t auth_data[];        // Variable length Authentication Data
} cord_ipv6_auth_hdr_t;

// ESP header
typedef struct CORD_PACKED cord_esp_hdr {
    uint32_t spi;               // Security Parameters Index
    uint32_t seq;               // Sequence Number
    // Followed by encrypted payload and ESP trailer
} cord_esp_hdr_t;

// ESP trailer
typedef struct CORD_PACKED cord_esp_trailer {
    uint8_t pad_length;         // Padding Length
    uint8_t next_header;        // Next Header
    // Followed by Authentication Data
} cord_esp_trailer_t;

// AH header (Authentication Header)
typedef struct CORD_PACKED cord_ah_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t hdrlen;             // Payload Length
    uint16_t reserved;          // Reserved
    uint32_t spi;               // Security Parameters Index
    uint32_t seq_no;            // Sequence Number
    uint8_t auth_data[];        // Variable length Authentication Data
} cord_ah_hdr_t;

// Mobile IPv6 header
typedef struct CORD_PACKED cord_mipv6_hdr {
    uint8_t nexthdr;            // Next Header
    uint8_t hdrlen;             // Header Extension Length
    uint8_t mh_type;            // Mobility Header Type
    uint8_t reserved;           // Reserved
    uint16_t checksum;          // Checksum
    uint8_t data[];             // Type-specific data
} cord_mipv6_hdr_t;

// IPv4 constants
#define CORD_IPV4_VERSION       4
#define CORD_IPV4_MIN_IHL       5
#define CORD_IPV4_MAX_IHL       15
#define CORD_IPV4_IHL_MASK      0x0F
#define CORD_IPV4_VERSION_MASK  0xF0

#define CORD_IPV4_DF            0x4000      // Don't Fragment
#define CORD_IPV4_MF            0x2000      // More Fragments
#define CORD_IPV4_OFFSET_MASK   0x1FFF      // Fragment Offset

// IPv4 ToS/DSCP constants
#define CORD_IPTOS_TOS_MASK     0x1E
#define CORD_IPTOS_PREC_MASK    0xE0
#define CORD_IPTOS_DSCP_MASK    0xFC
#define CORD_IPTOS_ECN_MASK     0x03

// IPv6 constants
#define CORD_IPV6_VERSION       6
#define CORD_IPV6_VERSION_MASK  0xF0000000
#define CORD_IPV6_TCLASS_MASK   0x0FF00000
#define CORD_IPV6_FLOWLABEL_MASK 0x000FFFFF

// ICMPv4 types
#define CORD_ICMP_ECHOREPLY     0
#define CORD_ICMP_DEST_UNREACH  3
#define CORD_ICMP_SOURCE_QUENCH 4
#define CORD_ICMP_REDIRECT      5
#define CORD_ICMP_ECHO          8
#define CORD_ICMP_TIME_EXCEEDED 11
#define CORD_ICMP_PARAMETERPROB 12
#define CORD_ICMP_TIMESTAMP     13
#define CORD_ICMP_TIMESTAMPREPLY 14
#define CORD_ICMP_INFO_REQUEST  15
#define CORD_ICMP_INFO_REPLY    16

// ICMPv6 types
#define CORD_ICMPV6_DEST_UNREACH    1
#define CORD_ICMPV6_PKT_TOOBIG      2
#define CORD_ICMPV6_TIME_EXCEED     3
#define CORD_ICMPV6_PARAMPROB       4
#define CORD_ICMPV6_ECHO_REQUEST    128
#define CORD_ICMPV6_ECHO_REPLY      129
#define CORD_ICMPV6_MGM_QUERY       130
#define CORD_ICMPV6_MGM_REPORT      131
#define CORD_ICMPV6_MGM_REDUCTION   132
#define CORD_ICMPV6_NI_QUERY        139
#define CORD_ICMPV6_NI_REPLY        140
#define CORD_ICMPV6_ND_ROUTER_SOLICIT   133
#define CORD_ICMPV6_ND_ROUTER_ADVERT    134
#define CORD_ICMPV6_ND_NEIGHBOR_SOLICIT 135
#define CORD_ICMPV6_ND_NEIGHBOR_ADVERT  136
#define CORD_ICMPV6_ND_REDIRECT         137

// IPv6 Next Header values
#define CORD_IPPROTO_HOPOPTS    0       // IPv6 hop-by-hop options
#define CORD_IPPROTO_ROUTING    43      // IPv6 routing header
#define CORD_IPPROTO_FRAGMENT   44      // IPv6 fragmentation header
#define CORD_IPPROTO_DSTOPTS    60      // IPv6 destination options
#define CORD_IPPROTO_MH         135     // Mobile IPv6

// Helper macros for IPv4
#define CORD_IPV4_GET_VERSION(iph)  ((iph)->version)
#define CORD_IPV4_GET_IHL(iph)      ((iph)->ihl)
#define CORD_IPV4_SET_VERSION(iph, ver) ((iph)->version = (ver))
#define CORD_IPV4_SET_IHL(iph, ihl_val) ((iph)->ihl = (ihl_val))
#define CORD_IPV4_GET_DSCP(iph)     (((iph)->tos & CORD_IPTOS_DSCP_MASK) >> 2)
#define CORD_IPV4_GET_ECN(iph)      ((iph)->tos & CORD_IPTOS_ECN_MASK)

// =============================================================================
// DHCP (Dynamic Host Configuration Protocol) - IPv4
// =============================================================================

// DHCP message header (RFC 2131)
typedef struct CORD_PACKED cord_dhcp_hdr {
    uint8_t op;                 // Operation (1=BOOTREQUEST, 2=BOOTREPLY)
    uint8_t htype;              // Hardware type (1=Ethernet)
    uint8_t hlen;               // Hardware address length
    uint8_t hops;               // Hops
    uint32_t xid;               // Transaction ID
    uint16_t secs;              // Seconds elapsed
    uint16_t flags;             // Flags (Broadcast, etc.)
    cord_ipv4_addr_t ciaddr;    // Client IP address
    cord_ipv4_addr_t yiaddr;    // Your (client) IP address
    cord_ipv4_addr_t siaddr;    // Next server IP address
    cord_ipv4_addr_t giaddr;    // Relay agent IP address
    uint8_t chaddr[16];         // Client hardware address
    uint8_t sname[64];          // Optional server host name
    uint8_t file[128];          // Boot file name
    uint32_t magic;             // Magic cookie (0x63825363)
    uint8_t options[];          // Options (variable length)
} cord_dhcp_hdr_t;

// DHCP option header
typedef struct CORD_PACKED cord_dhcp_option {
    uint8_t code;               // Option code
    uint8_t length;             // Option length
    uint8_t data[];             // Option data
} cord_dhcp_option_t;

// DHCP constants
#define CORD_DHCP_OP_BOOTREQUEST    1
#define CORD_DHCP_OP_BOOTREPLY      2
#define CORD_DHCP_MAGIC_COOKIE      0x63825363
#define CORD_DHCP_FLAG_BROADCAST    0x8000

// DHCP Hardware Types
#define CORD_DHCP_HTYPE_ETHERNET    1
#define CORD_DHCP_HTYPE_IEEE802     6
#define CORD_DHCP_HTYPE_FDDI        8

// DHCP Message Types (Option 53)
#define CORD_DHCP_DISCOVER          1
#define CORD_DHCP_OFFER             2
#define CORD_DHCP_REQUEST           3
#define CORD_DHCP_DECLINE           4
#define CORD_DHCP_ACK               5
#define CORD_DHCP_NAK               6
#define CORD_DHCP_RELEASE           7
#define CORD_DHCP_INFORM            8
#define CORD_DHCP_FORCERENEW        9
#define CORD_DHCP_LEASEQUERY        10
#define CORD_DHCP_LEASEUNASSIGNED   11
#define CORD_DHCP_LEASEUNKNOWN      12
#define CORD_DHCP_LEASEACTIVE       13

// Key DHCP Options (RFC 2132)
#define CORD_DHCP_OPT_PAD                   0
#define CORD_DHCP_OPT_SUBNET_MASK           1
#define CORD_DHCP_OPT_ROUTER                3
#define CORD_DHCP_OPT_DOMAIN_NAME_SERVER    6
#define CORD_DHCP_OPT_HOST_NAME             12
#define CORD_DHCP_OPT_DOMAIN_NAME           15
#define CORD_DHCP_OPT_BROADCAST_ADDRESS     28
#define CORD_DHCP_OPT_NTP_SERVERS           42
#define CORD_DHCP_OPT_REQUESTED_IP          50
#define CORD_DHCP_OPT_LEASE_TIME            51
#define CORD_DHCP_OPT_MESSAGE_TYPE          53
#define CORD_DHCP_OPT_SERVER_IDENTIFIER     54
#define CORD_DHCP_OPT_PARAMETER_REQUEST_LIST 55
#define CORD_DHCP_OPT_MESSAGE               56
#define CORD_DHCP_OPT_MAX_MESSAGE_SIZE      57
#define CORD_DHCP_OPT_RENEWAL_TIME          58
#define CORD_DHCP_OPT_REBINDING_TIME        59
#define CORD_DHCP_OPT_VENDOR_CLASS_ID       60
#define CORD_DHCP_OPT_CLIENT_IDENTIFIER     61
#define CORD_DHCP_OPT_TFTP_SERVER_NAME      66
#define CORD_DHCP_OPT_BOOTFILE_NAME         67
#define CORD_DHCP_OPT_CLIENT_FQDN           81
#define CORD_DHCP_OPT_CLASSLESS_STATIC_ROUTE 121
#define CORD_DHCP_OPT_END                   255

// =============================================================================
// DHCPv6 (Dynamic Host Configuration Protocol for IPv6) - RFC 8415
// =============================================================================

// DHCPv6 message header
typedef struct CORD_PACKED cord_dhcpv6_hdr {
    uint8_t msg_type;           // Message type
    uint8_t transaction_id[3];  // Transaction ID (24 bits)
    uint8_t options[];          // Options (variable length)
} cord_dhcpv6_hdr_t;

// DHCPv6 relay message header
typedef struct CORD_PACKED cord_dhcpv6_relay_hdr {
    uint8_t msg_type;           // Message type (12 or 13)
    uint8_t hop_count;          // Hop count
    cord_ipv6_addr_t link_addr; // Link address
    cord_ipv6_addr_t peer_addr; // Peer address
    uint8_t options[];          // Options (variable length)
} cord_dhcpv6_relay_hdr_t;

// DHCPv6 option header
typedef struct CORD_PACKED cord_dhcpv6_option {
    uint16_t code;              // Option code
    uint16_t length;            // Option length
    uint8_t data[];             // Option data
} cord_dhcpv6_option_t;

// DHCPv6 Message Types
#define CORD_DHCPV6_SOLICIT         1
#define CORD_DHCPV6_ADVERTISE       2
#define CORD_DHCPV6_REQUEST         3
#define CORD_DHCPV6_CONFIRM         4
#define CORD_DHCPV6_RENEW           5
#define CORD_DHCPV6_REBIND          6
#define CORD_DHCPV6_REPLY           7
#define CORD_DHCPV6_RELEASE         8
#define CORD_DHCPV6_DECLINE         9
#define CORD_DHCPV6_RECONFIGURE     10
#define CORD_DHCPV6_INFORMATION_REQUEST 11
#define CORD_DHCPV6_RELAY_FORW      12
#define CORD_DHCPV6_RELAY_REPL      13

// Key DHCPv6 Option Codes
#define CORD_DHCPV6_OPT_CLIENTID    1
#define CORD_DHCPV6_OPT_SERVERID    2
#define CORD_DHCPV6_OPT_IA_NA       3
#define CORD_DHCPV6_OPT_IA_TA       4
#define CORD_DHCPV6_OPT_IAADDR      5
#define CORD_DHCPV6_OPT_ORO         6
#define CORD_DHCPV6_OPT_PREFERENCE  7
#define CORD_DHCPV6_OPT_ELAPSED_TIME 8
#define CORD_DHCPV6_OPT_RELAY_MSG   9
#define CORD_DHCPV6_OPT_AUTH        11
#define CORD_DHCPV6_OPT_UNICAST     12
#define CORD_DHCPV6_OPT_STATUS_CODE 13
#define CORD_DHCPV6_OPT_RAPID_COMMIT 14
#define CORD_DHCPV6_OPT_USER_CLASS  15
#define CORD_DHCPV6_OPT_VENDOR_CLASS 16
#define CORD_DHCPV6_OPT_VENDOR_OPTS 17
#define CORD_DHCPV6_OPT_INTERFACE_ID 18
#define CORD_DHCPV6_OPT_RECONF_MSG  19
#define CORD_DHCPV6_OPT_RECONF_ACCEPT 20
#define CORD_DHCPV6_OPT_DNS_SERVERS 23
#define CORD_DHCPV6_OPT_DOMAIN_LIST 24
#define CORD_DHCPV6_OPT_IA_PD       25
#define CORD_DHCPV6_OPT_IAPREFIX    26
#define CORD_DHCPV6_OPT_SNTP_SERVERS 31
#define CORD_DHCPV6_OPT_INFORMATION_REFRESH_TIME 32
#define CORD_DHCPV6_OPT_CLIENT_FQDN 39
#define CORD_DHCPV6_OPT_NTP_SERVER  56
#define CORD_DHCPV6_OPT_BOOTFILE_URL 59

// =============================================================================
// IPv6 Neighbor Discovery Protocol (NDP) - RFC 4861
// =============================================================================

// IPv6 Neighbor Discovery option header
typedef struct CORD_PACKED cord_ipv6_nd_opt {
    uint8_t type;               // Option type
    uint8_t length;             // Option length (in units of 8 octets)
    uint8_t data[];             // Option data
} cord_ipv6_nd_opt_t;

// Router Solicitation (ICMPv6 type 133)
typedef struct CORD_PACKED cord_ipv6_nd_router_solicit {
    cord_icmpv6_hdr_t icmp6;    // ICMPv6 header
    uint32_t reserved;          // Reserved (must be zero)
    // Followed by possible options
} cord_ipv6_nd_router_solicit_t;

// Router Advertisement (ICMPv6 type 134)
typedef struct CORD_PACKED cord_ipv6_nd_router_advert {
    cord_icmpv6_hdr_t icmp6;    // ICMPv6 header
    uint8_t cur_hop_limit;      // Current hop limit
    uint8_t flags;              // Flags (M, O, H, Prf, P, R)
    uint16_t router_lifetime;   // Router lifetime
    uint32_t reachable_time;    // Reachable time
    uint32_t retrans_timer;     // Retrans timer
    // Followed by options
} cord_ipv6_nd_router_advert_t;

// Neighbor Solicitation (ICMPv6 type 135)
typedef struct CORD_PACKED cord_ipv6_nd_neighbor_solicit {
    cord_icmpv6_hdr_t icmp6;    // ICMPv6 header
    uint32_t reserved;          // Reserved (must be zero)
    cord_ipv6_addr_t target;    // Target address
    // Followed by possible options
} cord_ipv6_nd_neighbor_solicit_t;

// Neighbor Advertisement (ICMPv6 type 136)
typedef struct CORD_PACKED cord_ipv6_nd_neighbor_advert {
    cord_icmpv6_hdr_t icmp6;    // ICMPv6 header
    uint8_t flags;              // Flags (R, S, O)
    uint8_t reserved[3];        // Reserved (must be zero)
    cord_ipv6_addr_t target;    // Target address
    // Followed by possible options
} cord_ipv6_nd_neighbor_advert_t;

// Redirect Message (ICMPv6 type 137)
typedef struct CORD_PACKED cord_ipv6_nd_redirect {
    cord_icmpv6_hdr_t icmp6;    // ICMPv6 header
    uint32_t reserved;          // Reserved (must be zero)
    cord_ipv6_addr_t target;    // Target address
    cord_ipv6_addr_t destination; // Destination address
    // Followed by possible options
} cord_ipv6_nd_redirect_t;

// Source/Target Link-layer Address Option (Type 1/2)
typedef struct CORD_PACKED cord_ipv6_nd_opt_lladdr {
    uint8_t type;               // Option type (1 or 2)
    uint8_t length;             // Length (1 for Ethernet)
    uint8_t lladdr[6];          // Link-layer address (e.g., MAC address)
} cord_ipv6_nd_opt_lladdr_t;

// Prefix Information Option (Type 3)
typedef struct CORD_PACKED cord_ipv6_nd_opt_prefix_info {
    uint8_t type;               // Option type (3)
    uint8_t length;             // Length (4)
    uint8_t prefix_length;      // Prefix length
    uint8_t flags;              // Flags (L, A)
    uint32_t valid_lifetime;    // Valid lifetime
    uint32_t preferred_lifetime; // Preferred lifetime
    uint32_t reserved2;         // Reserved
    cord_ipv6_addr_t prefix;    // Prefix
} cord_ipv6_nd_opt_prefix_info_t;

// Redirected Header Option (Type 4)
typedef struct CORD_PACKED cord_ipv6_nd_opt_rh {
    uint8_t type;               // Option type (4)
    uint8_t length;             // Length
    uint16_t reserved1;         // Reserved
    uint32_t reserved2;         // Reserved
    uint8_t data[];             // IP header + data
} cord_ipv6_nd_opt_rh_t;

// MTU Option (Type 5)
typedef struct CORD_PACKED cord_ipv6_nd_opt_mtu {
    uint8_t type;               // Option type (5)
    uint8_t length;             // Length (1)
    uint16_t reserved;          // Reserved
    uint32_t mtu;               // MTU
} cord_ipv6_nd_opt_mtu_t;

// Route Information Option (Type 24) - RFC 4191
typedef struct CORD_PACKED cord_ipv6_nd_opt_route_info {
    uint8_t type;               // Option type (24)
    uint8_t length;             // Length
    uint8_t prefix_length;      // Prefix length
    uint8_t flags;              // Flags (Prf, Reserved)
    uint32_t route_lifetime;    // Route lifetime
    uint8_t prefix[];           // Prefix (variable length)
} cord_ipv6_nd_opt_route_info_t;

// Recursive DNS Server Option (Type 25) - RFC 8106
typedef struct CORD_PACKED cord_ipv6_nd_opt_rdnss {
    uint8_t type;               // Option type (25)
    uint8_t length;             // Length
    uint16_t reserved;          // Reserved
    uint32_t lifetime;          // Lifetime
    // Followed by list of IPv6 addresses
} cord_ipv6_nd_opt_rdnss_t;

// DNS Search List Option (Type 31) - RFC 8106
typedef struct CORD_PACKED cord_ipv6_nd_opt_dnssl {
    uint8_t type;               // Option type (31)
    uint8_t length;             // Length
    uint16_t reserved;          // Reserved
    uint32_t lifetime;          // Lifetime
    uint8_t domains[];          // Domain names (variable length)
} cord_ipv6_nd_opt_dnssl_t;

// IPv6 Neighbor Discovery Option Types
#define CORD_IPV6_ND_OPT_SOURCE_LLADDR      1   // Source Link-layer Address
#define CORD_IPV6_ND_OPT_TARGET_LLADDR      2   // Target Link-layer Address
#define CORD_IPV6_ND_OPT_PREFIX_INFO        3   // Prefix Information
#define CORD_IPV6_ND_OPT_REDIRECTED_HEADER  4   // Redirected Header
#define CORD_IPV6_ND_OPT_MTU                5   // MTU
#define CORD_IPV6_ND_OPT_ROUTE_INFO         24  // Route Information (RFC 4191)
#define CORD_IPV6_ND_OPT_RDNSS              25  // Recursive DNS Server (RFC 8106)
#define CORD_IPV6_ND_OPT_DNSSL              31  // DNS Search List (RFC 8106)

// Router Advertisement Flags
#define CORD_IPV6_ND_RA_FLAG_MANAGED        0x80    // M - Managed address config
#define CORD_IPV6_ND_RA_FLAG_OTHER          0x40    // O - Other config
#define CORD_IPV6_ND_RA_FLAG_HOME_AGENT     0x20    // H - Home Agent (RFC 3775)
#define CORD_IPV6_ND_RA_FLAG_PREFERENCE     0x18    // Prf - Router preference (RFC 4191)
#define CORD_IPV6_ND_RA_FLAG_PROXY          0x04    // P - Proxy (RFC 4389)

// Router Preference Values (RFC 4191)
#define CORD_IPV6_ND_RA_PRF_HIGH            0x08    // High preference
#define CORD_IPV6_ND_RA_PRF_MEDIUM          0x00    // Medium preference (default)
#define CORD_IPV6_ND_RA_PRF_LOW             0x18    // Low preference

// Neighbor Advertisement Flags
#define CORD_IPV6_ND_NA_FLAG_ROUTER         0x80    // R - Router
#define CORD_IPV6_ND_NA_FLAG_SOLICITED      0x40    // S - Solicited
#define CORD_IPV6_ND_NA_FLAG_OVERRIDE       0x20    // O - Override

// Prefix Information Flags
#define CORD_IPV6_ND_PI_FLAG_ONLINK         0x80    // L - On-link
#define CORD_IPV6_ND_PI_FLAG_AUTO           0x40    // A - Autonomous address config

// Common Port Numbers
#define CORD_PORT_DHCP_SERVER               67      // DHCP Server
#define CORD_PORT_DHCP_CLIENT               68      // DHCP Client
#define CORD_PORT_DHCPV6_SERVER             547     // DHCPv6 Server
#define CORD_PORT_DHCPV6_CLIENT             546     // DHCPv6 Client

// Helper macros for IPv6
#define CORD_IPV6_GET_VERSION(ip6h)     ((ip6h)->version)
#define CORD_IPV6_GET_TCLASS(ip6h)      ((ip6h)->traffic_class)
#define CORD_IPV6_GET_FLOWLABEL(ip6h)   ((ip6h)->flow_label)
#define CORD_IPV6_SET_VERSION(ip6h, ver) ((ip6h)->version = (ver))
#define CORD_IPV6_SET_TCLASS(ip6h, tc)   ((ip6h)->traffic_class = (tc))
#define CORD_IPV6_SET_FLOWLABEL(ip6h, fl) ((ip6h)->flow_label = (fl))

#endif // CORD_L3_PROTOCOLS_H