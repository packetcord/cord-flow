#ifndef CORD_LPM_H
#define CORD_LPM_H

#include <cord_type.h>
#include <protocol_headers/cord_protocol_headers.h>

//
// CORD LPM - Longest Prefix Match Implementation
// Based on DPDK's DIR-24-8 algorithm for IPv4 and multi-level trie for IPv6
//
// References:
// - DPDK LPM: https://doc.dpdk.org/guides/prog_guide/lpm_lib.html
// - DPDK LPM6: https://doc.dpdk.org/guides/prog_guide/lpm6_lib.html
//

//
// IPv4 LPM (32-bit) - DIR-24-8 Algorithm
//
//
// Algorithm: Two-level table structure
// - TBL24: 2^24 entries indexed by first 24 bits (16M entries, 128MB)
// - TBL8:  Multiple groups of 2^8 entries for depth > 24
//
// Performance: 1-2 memory accesses (1 for depth <= 24, 2 for depth > 24)
// Memory: ~128MB base + ~2KB per TBL8 group
//

#define CORD_IPV4_LPM_TBL24_SIZE        (1 << 24)  // 16,777,216 entries
#define CORD_IPV4_LPM_TBL8_SIZE         256        // 256 entries per group
#define CORD_IPV4_LPM_TBL8_MAX_GROUPS   256        // Default: 256 groups (configurable)
#define CORD_IPV4_LPM_MAX_DEPTH         32
#define CORD_IPV4_LPM_INVALID_NEXT_HOP  0xFFFFFFFF

// IPv4 LPM entry structure (8 bytes - cache line friendly)
typedef struct
{
    uint32_t next_hop;     // Next hop identifier or output interface
    uint8_t depth;         // Prefix length (0-32)
    uint8_t valid : 1;     // Entry contains a valid route
    uint8_t ext_entry : 1; // Entry points to TBL8 group (vs. direct next hop)
    uint8_t reserved : 6;  // Reserved for future use
    uint16_t group_idx;    // TBL8 group index (if ext_entry == 1)
} CORD_PACKED cord_ipv4_lpm_entry_t;

// IPv4 LPM table structure
typedef struct
{
    // Tables
    cord_ipv4_lpm_entry_t *tbl24;                                  // Primary table (16M entries)
    cord_ipv4_lpm_entry_t **tbl8_groups;                           // Secondary tables (on-demand)

    // TBL8 management
    uint16_t tbl8_free_list[CORD_IPV4_LPM_TBL8_MAX_GROUPS];        // Free group indices
    uint32_t tbl8_free_count;                                      // Number of free groups
    uint32_t tbl8_used_count;                                      // Number of used groups

    // Statistics
    uint64_t lookup_count;                                         // Total lookups performed
    uint32_t routes_count;                                         // Number of installed routes
    uint32_t max_routes;                                           // Maximum routes capacity
} cord_ipv4_lpm_t;

// IPv4 LPM API
cord_ipv4_lpm_t *cord_ipv4_lpm_create(uint32_t max_routes);
void cord_ipv4_lpm_destroy(cord_ipv4_lpm_t *lpm);

int cord_ipv4_lpm_add(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth, uint32_t next_hop);
int cord_ipv4_lpm_delete(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth);
int cord_ipv4_lpm_delete_all(cord_ipv4_lpm_t *lpm);

// Fast inline lookup (optimized for hot path)
static inline uint32_t cord_ipv4_lpm_lookup(const cord_ipv4_lpm_t *lpm, uint32_t ip)
{
    // First lookup: TBL24 indexed by upper 24 bits
    uint32_t tbl24_idx = ip >> 8;
    cord_ipv4_lpm_entry_t entry = lpm->tbl24[tbl24_idx];

    if (cord_unlikely(!entry.valid))
    {
        return CORD_IPV4_LPM_INVALID_NEXT_HOP;
    }

    // Fast path: Direct next hop (depth <= 24)
    if (cord_likely(!entry.ext_entry))
    {
        return entry.next_hop;
    }

    // Slow path: TBL8 lookup (depth > 24)
    uint8_t tbl8_idx = ip & 0xFF;
    entry = lpm->tbl8_groups[entry.group_idx][tbl8_idx];

    return entry.valid ? entry.next_hop : CORD_IPV4_LPM_INVALID_NEXT_HOP;
}

// Batch lookup for vectorization opportunities
void cord_ipv4_lpm_lookup_batch(const cord_ipv4_lpm_t *lpm, const uint32_t *ips,
                                 uint32_t *next_hops, uint32_t count);

//
// IPv6 LPM (128-bit) - Multi-level Trie
//
//
// Algorithm: Multi-level trie with varying strides
// - Level 0: TBL24 indexed by first 24 bits (16M entries, 128MB)
// - Levels 1-13: TBL8 groups indexed by 8-bit chunks (104 bits total)
//
// Performance: 1-14 memory accesses (avg 5 for common /48 prefixes)
// Memory: ~128MB base + ~1KB per TBL8 group
//

#define CORD_IPV6_LPM_TBL24_SIZE        (1 << 24)  // 16,777,216 entries
#define CORD_IPV6_LPM_TBL8_SIZE         256        // 256 entries per group
#define CORD_IPV6_LPM_TBL8_MAX_GROUPS   65536      // Max groups (65K)
#define CORD_IPV6_LPM_MAX_DEPTH         128
#define CORD_IPV6_LPM_MAX_LEVELS        14         // 1 TBL24 + 13 TBL8 levels
#define CORD_IPV6_LPM_INVALID_NEXT_HOP  0xFFFFFFFF

// IPv6 LPM entry structure (8 bytes)
typedef struct
{
    uint32_t next_hop;     // Next hop identifier
    uint8_t depth;         // Prefix length (0-128)
    uint8_t valid : 1;     // Entry contains a valid route
    uint8_t ext_entry : 1; // Entry points to next level TBL8
    uint8_t reserved : 6;
    uint16_t group_idx;    // TBL8 group index (if ext_entry == 1)
} CORD_PACKED cord_ipv6_lpm_entry_t;

// IPv6 LPM table structure
typedef struct
{
    // Tables
    cord_ipv6_lpm_entry_t *tbl24;                                  // Root table (16M entries)
    cord_ipv6_lpm_entry_t **tbl8_groups;                           // Multi-level TBL8 groups

    // TBL8 management
    uint16_t tbl8_free_list[CORD_IPV6_LPM_TBL8_MAX_GROUPS];
    uint32_t tbl8_free_count;
    uint32_t tbl8_used_count;

    // Statistics
    uint64_t lookup_count;
    uint32_t routes_count;
    uint32_t max_routes;
    uint32_t max_depth_reached;                                    // Deepest trie level used
} cord_ipv6_lpm_t;

// IPv6 LPM API
cord_ipv6_lpm_t *cord_ipv6_lpm_create(uint32_t max_routes);
void cord_ipv6_lpm_destroy(cord_ipv6_lpm_t *lpm);

int cord_ipv6_lpm_add(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth, uint32_t next_hop);
int cord_ipv6_lpm_delete(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth);
int cord_ipv6_lpm_delete_all(cord_ipv6_lpm_t *lpm);

uint32_t cord_ipv6_lpm_lookup(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip);

void cord_ipv6_lpm_lookup_batch(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ips,
                                 uint32_t *next_hops, uint32_t count);

//
// Helper Functions
//

// IPv4 helpers
int cord_ipv4_str_to_binary(const char *ip_str, uint32_t *ip_bin);
int cord_ipv4_parse_cidr(const char *cidr, uint32_t *ip, uint8_t *depth);
void cord_ipv4_to_str(uint32_t ip, char *buf, size_t len);

// IPv6 helpers
int cord_ipv6_str_to_binary(const char *ip_str, cord_ipv6_addr_t *ip);
int cord_ipv6_parse_cidr(const char *cidr, cord_ipv6_addr_t *ip, uint8_t *depth);
void cord_ipv6_to_str(const cord_ipv6_addr_t *ip, char *buf, size_t len);

// MAC address helpers (for MAC-based LPM research/subnetting)
void cord_mac_to_u64(const cord_mac_addr_t *mac, uint64_t *mac64);
void cord_u64_to_mac(uint64_t mac64, cord_mac_addr_t *mac);
int cord_mac_str_to_binary(const char *mac_str, cord_mac_addr_t *mac);
int cord_mac_parse_cidr(const char *cidr, cord_mac_addr_t *mac, uint8_t *depth);
void cord_mac_to_str(const cord_mac_addr_t *mac, char *buf, size_t len);

// Statistics
void cord_ipv4_lpm_print_stats(const cord_ipv4_lpm_t *lpm);
void cord_ipv6_lpm_print_stats(const cord_ipv6_lpm_t *lpm);

#endif // CORD_LPM_H
