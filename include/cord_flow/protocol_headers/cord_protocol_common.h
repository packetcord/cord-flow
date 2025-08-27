#ifndef CORD_PROTOCOL_COMMON_H
#define CORD_PROTOCOL_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Portable byte order macros
#ifndef __BYTE_ORDER
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#ifdef __ARMEL__
#define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined(__ARMEB__)
#define __BYTE_ORDER __BIG_ENDIAN
#elif defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif

// Portable network byte order conversion
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cord_ntohs(x) ((uint16_t)((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define cord_htons(x) cord_ntohs(x)
#define cord_ntohl(x) ((uint32_t)((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff)))
#define cord_htonl(x) cord_ntohl(x)
#define cord_ntohll(x) ((uint64_t)((((uint64_t)cord_ntohl((x) & 0xffffffff)) << 32) | cord_ntohl(((x) >> 32) & 0xffffffff)))
#define cord_htonll(x) cord_ntohll(x)
#else
#define cord_ntohs(x) (x)
#define cord_htons(x) (x)
#define cord_ntohl(x) (x)
#define cord_htonl(x) (x)
#define cord_ntohll(x) (x)
#define cord_htonll(x) (x)
#endif

// Portable address structures
typedef struct {
    uint8_t addr[6];
} cord_mac_addr_t;

typedef struct {
    uint32_t addr;
} cord_ipv4_addr_t;

typedef struct {
    uint8_t addr[16];
} cord_ipv6_addr_t;

// Protocol constants
#define CORD_ETH_ALEN       6       // Ethernet address length
#define CORD_ETH_HLEN       14      // Ethernet header length
#define CORD_ETH_ZLEN       60      // Minimum frame size
#define CORD_ETH_FRAME_LEN  1514    // Maximum frame size
#define CORD_ETH_FCS_LEN    4       // Frame check sequence length

// Common protocol numbers
#define CORD_IPPROTO_ICMP   1
#define CORD_IPPROTO_IGMP   2
#define CORD_IPPROTO_TCP    6
#define CORD_IPPROTO_UDP    17
#define CORD_IPPROTO_IPV6   41
#define CORD_IPPROTO_GRE    47
#define CORD_IPPROTO_ESP    50
#define CORD_IPPROTO_AH     51
#define CORD_IPPROTO_ICMPV6 58
#define CORD_IPPROTO_EIGRP  88
#define CORD_IPPROTO_OSPF   89
#define CORD_IPPROTO_SCTP   132

// EtherType constants
#define CORD_ETH_P_IP       0x0800  // IPv4
#define CORD_ETH_P_ARP      0x0806  // ARP
#define CORD_ETH_P_RARP     0x8035  // RARP
#define CORD_ETH_P_IPV6     0x86DD  // IPv6
#define CORD_ETH_P_8021Q    0x8100  // 802.1Q VLAN
#define CORD_ETH_P_8021AD   0x88A8  // 802.1ad Service VLAN
#define CORD_ETH_P_MPLS_UC  0x8847  // MPLS unicast
#define CORD_ETH_P_MPLS_MC  0x8848  // MPLS multicast
#define CORD_ETH_P_PPP_DISC 0x8863  // PPPoE discovery
#define CORD_ETH_P_PPP_SES  0x8864  // PPPoE session

// Packing attribute for structs
#ifdef __GNUC__
#define CORD_PACKED __attribute__((packed))
#elif defined(_MSC_VER)
#define CORD_PACKED
#pragma pack(push, 1)
#else
#define CORD_PACKED
#endif

#endif // CORD_PROTOCOL_COMMON_H