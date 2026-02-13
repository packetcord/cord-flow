#ifndef CORD_TABLE_H
#define CORD_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <cord_type.h>
#include <protocol_headers/cord_protocol_common.h>

//
// Unified LPM for 32-bit (IPv4), 48-bit (MAC), 128-bit (IPv6)
//

// ================================================================
// IPv4 LPM (32-bit) - DIR-24-8 Algorithm
// ================================================================

#define CORD_IPV4_LPM_TBL24_SIZE        (1 << 24)  // 16M entries
#define CORD_IPV4_LPM_TBL8_SIZE         256        // 256 entries per group
#define CORD_IPV4_LPM_TBL8_MAX_GROUPS   8192       // Max TBL8 groups
#define CORD_IPV4_LPM_MAX_DEPTH         32
#define CORD_IPV4_LPM_INVALID_NEXT_HOP  0xFFFFFFFF

// IPv4 LPM entry (8 bytes - cache friendly)
typedef struct {
    uint32_t next_hop;               // Next hop identifier or output port
    uint8_t depth;                   // Prefix length (0-32)
    uint8_t valid:1;                 // Entry is valid
    uint8_t ext_entry:1;             // Points to TBL8
    uint8_t reserved:6;
    uint16_t group_idx;              // TBL8 group index (if ext_entry=1)
} CORD_PACKED cord_ipv4_lpm_entry_t;

typedef struct {
    cord_ipv4_lpm_entry_t *tbl24;
    cord_ipv4_lpm_entry_t **tbl8_groups;

    uint32_t tbl8_free_list[CORD_IPV4_LPM_TBL8_MAX_GROUPS];
    uint32_t tbl8_free_count;
    uint32_t tbl8_used_count;

    uint64_t lookup_count;
    uint32_t routes_count;
    uint32_t max_routes;
} cord_ipv4_lpm_t;

// IPv4 LPM API
cord_ipv4_lpm_t *cord_ipv4_lpm_create(uint32_t max_routes);
void cord_ipv4_lpm_destroy(cord_ipv4_lpm_t *lpm);

int cord_ipv4_lpm_add(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth, uint32_t next_hop);
int cord_ipv4_lpm_delete(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth);

// Fast inline lookup
static inline uint32_t cord_ipv4_lpm_lookup(const cord_ipv4_lpm_t *lpm, uint32_t ip)
{
    uint32_t tbl24_idx = ip >> 8;
    cord_ipv4_lpm_entry_t entry = lpm->tbl24[tbl24_idx];

    if (!entry.valid) {
        return CORD_IPV4_LPM_INVALID_NEXT_HOP;
    }

    if (cord_likely(!entry.ext_entry)) {
        return entry.next_hop;
    }

    uint8_t tbl8_idx = ip & 0xFF;
    entry = lpm->tbl8_groups[entry.group_idx][tbl8_idx];

    return entry.valid ? entry.next_hop : CORD_IPV4_LPM_INVALID_NEXT_HOP;
}

void cord_ipv4_lpm_lookup_batch(const cord_ipv4_lpm_t *lpm, const uint32_t *ips,
                                uint32_t *next_hops, uint32_t count);

// ================================================================
// MAC LPM (48-bit) - DIR-24-24 Algorithm
// ================================================================

#define CORD_MAC_LPM_TBL24_SIZE         (1 << 24)  // 16M entries
#define CORD_MAC_LPM_TBL24_MAX_GROUPS   4096       // Secondary table groups
#define CORD_MAC_LPM_MAX_DEPTH          48
#define CORD_MAC_LPM_INVALID_NEXT_HOP   0xFFFFFFFF

// MAC LPM entry (8 bytes)
typedef struct {
    uint32_t next_hop;
    uint8_t depth;
    uint8_t valid:1;
    uint8_t ext_entry:1;
    uint8_t reserved:6;
    uint16_t group_idx;
} CORD_PACKED cord_mac_lpm_entry_t;

typedef struct {
    cord_mac_lpm_entry_t *tbl24_hi;  // First 24 bits
    cord_mac_lpm_entry_t **tbl24_lo_groups;  // Remaining 24 bits (on demand)

    uint32_t group_free_list[CORD_MAC_LPM_TBL24_MAX_GROUPS];
    uint32_t group_free_count;
    uint32_t groups_used;

    uint64_t lookup_count;
    uint32_t routes_count;
    uint32_t max_routes;
} cord_mac_lpm_t;

// MAC LPM API
cord_mac_lpm_t *cord_mac_lpm_create(uint32_t max_routes);
void cord_mac_lpm_destroy(cord_mac_lpm_t *lpm);

int cord_mac_lpm_add(cord_mac_lpm_t *lpm, const cord_mac_addr_t *mac, uint8_t depth, uint32_t next_hop);
int cord_mac_lpm_add_u64(cord_mac_lpm_t *lpm, uint64_t mac48, uint8_t depth, uint32_t next_hop);
int cord_mac_lpm_delete(cord_mac_lpm_t *lpm, const cord_mac_addr_t *mac, uint8_t depth);

// Fast inline lookup
static inline uint32_t cord_mac_lpm_lookup(const cord_mac_lpm_t *lpm, const cord_mac_addr_t *mac)
{
    // Convert MAC to 64-bit
    uint64_t mac64 = ((uint64_t)mac->addr[0] << 40) |
                     ((uint64_t)mac->addr[1] << 32) |
                     ((uint64_t)mac->addr[2] << 24) |
                     ((uint64_t)mac->addr[3] << 16) |
                     ((uint64_t)mac->addr[4] << 8)  |
                     ((uint64_t)mac->addr[5]);

    // First 24-bit lookup
    uint32_t hi_idx = mac64 >> 24;
    cord_mac_lpm_entry_t entry = lpm->tbl24_hi[hi_idx];

    if (!entry.valid) {
        return CORD_MAC_LPM_INVALID_NEXT_HOP;
    }

    if (cord_likely(!entry.ext_entry)) {
        return entry.next_hop;
    }

    // Second 24-bit lookup
    uint32_t lo_idx = mac64 & 0xFFFFFF;
    entry = lpm->tbl24_lo_groups[entry.group_idx][lo_idx];

    return entry.valid ? entry.next_hop : CORD_MAC_LPM_INVALID_NEXT_HOP;
}

static inline uint32_t cord_mac_lpm_lookup_u64(const cord_mac_lpm_t *lpm, uint64_t mac48)
{
    cord_mac_addr_t mac;
    mac.addr[0] = (mac48 >> 40) & 0xFF;
    mac.addr[1] = (mac48 >> 32) & 0xFF;
    mac.addr[2] = (mac48 >> 24) & 0xFF;
    mac.addr[3] = (mac48 >> 16) & 0xFF;
    mac.addr[4] = (mac48 >> 8) & 0xFF;
    mac.addr[5] = mac48 & 0xFF;
    return cord_mac_lpm_lookup(lpm, &mac);
}

// ================================================================
// IPv6 LPM (128-bit) - Compressed Multi-bit Trie
// ================================================================

#define CORD_IPV6_LPM_TBL16_SIZE        (1 << 16)  // Fast path: 64K entries
#define CORD_IPV6_LPM_MAX_DEPTH         128
#define CORD_IPV6_LPM_STRIDE_SIZE       16
#define CORD_IPV6_LPM_INVALID_NEXT_HOP  0xFFFFFFFF

typedef struct cord_ipv6_lpm_node {
    uint32_t next_hop;
    uint8_t depth;
    uint8_t is_leaf:1;
    uint8_t has_children:1;
    uint8_t reserved:6;
    uint16_t child_count;

    // Compressed children (bitmap + compact array)
    uint64_t *child_bitmap;          // 1024 × 64-bit if allocated
    struct cord_ipv6_lpm_node **children;
} cord_ipv6_lpm_node_t;

typedef struct {
    cord_ipv6_lpm_node_t *root;

    // Fast path: direct lookup for first 16 bits
    uint32_t *tbl16;
    uint8_t *tbl16_valid;

    // Memory pool
    cord_ipv6_lpm_node_t *node_pool;
    uint32_t node_pool_capacity;
    uint32_t nodes_allocated;

    uint64_t lookup_count;
    uint32_t routes_count;
    uint32_t max_tree_depth;
    uint32_t max_routes;
} cord_ipv6_lpm_t;

// IPv6 LPM API
cord_ipv6_lpm_t *cord_ipv6_lpm_create(uint32_t max_routes);
void cord_ipv6_lpm_destroy(cord_ipv6_lpm_t *lpm);

int cord_ipv6_lpm_add(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth, uint32_t next_hop);
int cord_ipv6_lpm_delete(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth);

uint32_t cord_ipv6_lpm_lookup(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip);

void cord_ipv6_lpm_lookup_batch(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ips,
                                uint32_t *next_hops, uint32_t count);

// ================================================================
// Generic LPM Interface (Type-Agnostic Wrapper)
// ================================================================

typedef enum {
    CORD_LPM_TYPE_IPV4,      // 32-bit
    CORD_LPM_TYPE_MAC,       // 48-bit
    CORD_LPM_TYPE_IPV6,      // 128-bit
} cord_lpm_type_t;

typedef union {
    uint32_t ipv4;
    uint64_t mac48;
    cord_mac_addr_t mac;
    cord_ipv6_addr_t ipv6;
    uint8_t bytes[16];
} cord_lpm_addr_t;

typedef struct cord_lpm_table {
    cord_lpm_type_t type;
    union {
        cord_ipv4_lpm_t *ipv4;
        cord_mac_lpm_t *mac;
        cord_ipv6_lpm_t *ipv6;
    } impl;
} cord_lpm_table_t;

// Generic API
cord_lpm_table_t *cord_lpm_create(cord_lpm_type_t type, uint32_t max_routes);
void cord_lpm_destroy(cord_lpm_table_t *table);

int cord_lpm_add(cord_lpm_table_t *table, const cord_lpm_addr_t *addr, uint8_t depth, uint32_t next_hop);
int cord_lpm_delete(cord_lpm_table_t *table, const cord_lpm_addr_t *addr, uint8_t depth);
uint32_t cord_lpm_lookup(const cord_lpm_table_t *table, const cord_lpm_addr_t *addr);

// ================================================================
// Helper Functions
// ================================================================

// IPv4 helpers
int cord_ipv4_str_to_binary(const char *ip_str, uint32_t *ip_bin);
int cord_parse_cidr_ipv4(const char *cidr, uint32_t *ip, uint8_t *depth);
void cord_ipv4_to_str(uint32_t ip, char *buf, size_t len);

// MAC helpers
void cord_mac_to_u64(const cord_mac_addr_t *mac, uint64_t *mac64);
void cord_u64_to_mac(uint64_t mac64, cord_mac_addr_t *mac);
int cord_mac_str_to_binary(const char *mac_str, cord_mac_addr_t *mac);
int cord_parse_cidr_mac(const char *cidr, cord_mac_addr_t *mac, uint8_t *depth);
void cord_mac_to_str(const cord_mac_addr_t *mac, char *buf, size_t len);

// IPv6 helpers
int cord_ipv6_str_to_binary(const char *ip_str, cord_ipv6_addr_t *ip);
int cord_parse_cidr_ipv6(const char *cidr, cord_ipv6_addr_t *ip, uint8_t *depth);
void cord_ipv6_to_str(const cord_ipv6_addr_t *ip, char *buf, size_t len);

// Statistics
void cord_ipv4_lpm_print_stats(const cord_ipv4_lpm_t *lpm);
void cord_mac_lpm_print_stats(const cord_mac_lpm_t *lpm);
void cord_ipv6_lpm_print_stats(const cord_ipv6_lpm_t *lpm);
void cord_lpm_print_stats(const cord_lpm_table_t *table);

#endif // CORD_TABLE_H
