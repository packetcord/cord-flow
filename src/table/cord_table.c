#include <table/cord_table.h>
#include <memory/cord_memory.h>
#include <action/cord_action.h>
#include <match/cord_match.h>
//
// IPv4 LPM Implementation (DIR-24-8 Algorithm)
//

cord_ipv4_lpm_t *cord_ipv4_lpm_create(uint32_t max_routes)
{
    cord_ipv4_lpm_t *lpm = calloc(1, sizeof(cord_ipv4_lpm_t));
    if (!lpm)
    {
        return NULL;
    }

    lpm->max_routes = max_routes;

    // Allocate TBL24 (128MB)
    size_t tbl24_size = CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t);
    lpm->tbl24 = cord_alloc_hugepage(tbl24_size);
    if (!lpm->tbl24)
    {
        free(lpm);
        return NULL;
    }

    // Allocate TBL8 group pointer array
    lpm->tbl8_groups = calloc(CORD_IPV4_LPM_TBL8_MAX_GROUPS, sizeof(cord_ipv4_lpm_entry_t *));
    if (!lpm->tbl8_groups)
    {
        cord_free_hugepage(lpm->tbl24, tbl24_size);
        free(lpm);
        return NULL;
    }

    // Initialize free list
    for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_MAX_GROUPS; i++)
    {
        lpm->tbl8_free_list[i] = i;
    }
    lpm->tbl8_free_count = CORD_IPV4_LPM_TBL8_MAX_GROUPS;
    lpm->tbl8_used_count = 0;

    lpm->lookup_count = 0;
    lpm->routes_count = 0;

    return lpm;
}

void cord_ipv4_lpm_destroy(cord_ipv4_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    // Free all TBL8 groups
    for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_MAX_GROUPS; i++)
    {
        if (lpm->tbl8_groups[i])
        {
            free(lpm->tbl8_groups[i]);
        }
    }

    free(lpm->tbl8_groups);

    size_t tbl24_size = CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t);
    cord_free_hugepage(lpm->tbl24, tbl24_size);

    free(lpm);
}

// Allocate a new TBL8 group
static int tbl8_alloc(cord_ipv4_lpm_t *lpm, uint32_t *group_idx)
{
    if (lpm->tbl8_free_count == 0)
    {
        return -1; // No free groups
    }

    // Get index from free list
    *group_idx = lpm->tbl8_free_list[--lpm->tbl8_free_count];

    // Allocate the group
    lpm->tbl8_groups[*group_idx] = calloc(CORD_IPV4_LPM_TBL8_SIZE, sizeof(cord_ipv4_lpm_entry_t));
    if (!lpm->tbl8_groups[*group_idx])
    {
        lpm->tbl8_free_list[lpm->tbl8_free_count++] = *group_idx;
        return -1;
    }

    lpm->tbl8_used_count++;
    return 0;
}

// Free a TBL8 group
static void tbl8_free(cord_ipv4_lpm_t *lpm, uint32_t group_idx)
{
    if (group_idx >= CORD_IPV4_LPM_TBL8_MAX_GROUPS)
    {
        return;
    }

    if (lpm->tbl8_groups[group_idx])
    {
        free(lpm->tbl8_groups[group_idx]);
        lpm->tbl8_groups[group_idx] = NULL;
    }

    lpm->tbl8_free_list[lpm->tbl8_free_count++] = group_idx;
    lpm->tbl8_used_count--;
}

int cord_ipv4_lpm_add(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth, uint32_t next_hop)
{
    if (!lpm || depth > 32)
    {
        return -1;
    }

    // Special case: depth 0 is default route
    if (depth == 0)
    {
        // Fill entire TBL24 with default route
        for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL24_SIZE; i++)
        {
            if (!lpm->tbl24[i].valid || lpm->tbl24[i].depth < depth)
            {
                lpm->tbl24[i].next_hop = next_hop;
                lpm->tbl24[i].depth = depth;
                lpm->tbl24[i].valid = 1;
                lpm->tbl24[i].ext_entry = 0;
            }
        }
        lpm->routes_count++;
        return 0;
    }

    // Normalize IP to network address
    uint32_t mask = (depth == 32) ? 0xFFFFFFFF : ~((1U << (32 - depth)) - 1);
    ip = ip & mask;

    if (depth <= 24)
    {
        // Route fits in TBL24
        uint32_t tbl24_start = ip >> 8;
        uint32_t num_entries = 1 << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_start + i;

            // Only update if no entry or new route is more specific
            if (!lpm->tbl24[idx].valid || depth >= lpm->tbl24[idx].depth)
            {
                lpm->tbl24[idx].next_hop = next_hop;
                lpm->tbl24[idx].depth = depth;
                lpm->tbl24[idx].valid = 1;
                // Don't modify ext_entry if it was already set for longer prefixes
                if (depth >= lpm->tbl24[idx].depth)
                {
                    lpm->tbl24[idx].ext_entry = 0;
                }
            }
        }
    }
    else
    {
        // Route needs TBL8 (depth > 24)
        uint32_t tbl24_idx = ip >> 8;
        uint32_t group_idx;

        // Check if TBL8 group exists
        if (!lpm->tbl24[tbl24_idx].ext_entry)
        {
            // Need to allocate TBL8 group
            if (tbl8_alloc(lpm, &group_idx) != 0)
            {
                return -1; // Allocation failed
            }

            // Copy TBL24 entry to all TBL8 entries
            cord_ipv4_lpm_entry_t tbl24_entry = lpm->tbl24[tbl24_idx];
            for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_SIZE; i++)
            {
                lpm->tbl8_groups[group_idx][i] = tbl24_entry;
                lpm->tbl8_groups[group_idx][i].ext_entry = 0;
            }

            // Update TBL24 to point to TBL8
            lpm->tbl24[tbl24_idx].ext_entry = 1;
            lpm->tbl24[tbl24_idx].group_idx = group_idx;
        }
        else
        {
            group_idx = lpm->tbl24[tbl24_idx].group_idx;
        }

        // Update TBL8 entries
        uint32_t tbl8_start = (ip & 0xFF);
        uint32_t num_entries = 1 << (32 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl8_start + i;
            if (idx >= CORD_IPV4_LPM_TBL8_SIZE)
            {
                break;
            }

            if (!lpm->tbl8_groups[group_idx][idx].valid || depth >= lpm->tbl8_groups[group_idx][idx].depth)
            {
                lpm->tbl8_groups[group_idx][idx].next_hop = next_hop;
                lpm->tbl8_groups[group_idx][idx].depth = depth;
                lpm->tbl8_groups[group_idx][idx].valid = 1;
                lpm->tbl8_groups[group_idx][idx].ext_entry = 0;
            }
        }
    }

    lpm->routes_count++;
    return 0;
}

int cord_ipv4_lpm_delete(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth)
{
    if (!lpm || depth > 32)
    {
        return -1;
    }

    // Normalize IP to network address
    uint32_t mask = (depth == 32) ? 0xFFFFFFFF : ~((1U << (32 - depth)) - 1);
    ip = ip & mask;

    if (depth <= 24)
    {
        uint32_t tbl24_start = ip >> 8;
        uint32_t num_entries = 1 << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_start + i;
            if (lpm->tbl24[idx].depth == depth)
            {
                lpm->tbl24[idx].valid = 0;
                lpm->tbl24[idx].next_hop = 0;
                lpm->tbl24[idx].depth = 0;
            }
        }
    }
    else
    {
        uint32_t tbl24_idx = ip >> 8;

        if (lpm->tbl24[tbl24_idx].ext_entry)
        {
            uint32_t group_idx = lpm->tbl24[tbl24_idx].group_idx;
            uint32_t tbl8_start = (ip & 0xFF);
            uint32_t num_entries = 1 << (32 - depth);

            for (uint32_t i = 0; i < num_entries; i++)
            {
                uint32_t idx = tbl8_start + i;
                if (idx >= CORD_IPV4_LPM_TBL8_SIZE)
                {
                    break;
                }

                if (lpm->tbl8_groups[group_idx][idx].depth == depth)
                {
                    lpm->tbl8_groups[group_idx][idx].valid = 0;
                    lpm->tbl8_groups[group_idx][idx].next_hop = 0;
                    lpm->tbl8_groups[group_idx][idx].depth = 0;
                }
            }
        }
    }

    lpm->routes_count--;
    return 0;
}

// Batch lookup
void cord_ipv4_lpm_lookup_batch(const cord_ipv4_lpm_t *lpm, const uint32_t *ips, uint32_t *next_hops, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        next_hops[i] = cord_ipv4_lpm_lookup(lpm, ips[i]);
    }
}

// Helper: Convert IP string to binary
int cord_ipv4_str_to_binary(const char *ip_str, uint32_t *ip_bin)
{
    struct in_addr addr;
    if (inet_pton(AF_INET, ip_str, &addr) != 1)
    {
        return -1;
    }
    *ip_bin = ntohl(addr.s_addr);
    return 0;
}

// Helper: Parse CIDR notation
int cord_parse_cidr_ipv4(const char *cidr, uint32_t *ip, uint8_t *depth)
{
    char ip_str[16];
    int prefix_len;

    if (sscanf(cidr, "%15[^/]/%d", ip_str, &prefix_len) != 2)
    {
        return -1;
    }

    if (prefix_len < 0 || prefix_len > 32)
    {
        return -1;
    }

    if (cord_ipv4_str_to_binary(ip_str, ip) != 0)
    {
        return -1;
    }

    *depth = (uint8_t) prefix_len;
    return 0;
}

// Helper: Convert binary IP to string
void cord_ipv4_to_str(uint32_t ip, char *buf, size_t len)
{
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, buf, len);
}

// Statistics
void cord_ipv4_lpm_print_stats(const cord_ipv4_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    CORD_LOG("=== IPv4 LPM Statistics ===\n");
    CORD_LOG("Routes:           %u\n", lpm->routes_count);
    CORD_LOG("Max routes:       %u\n", lpm->max_routes);
    CORD_LOG("TBL8 groups used: %u / %u\n", lpm->tbl8_used_count, CORD_IPV4_LPM_TBL8_MAX_GROUPS);
    CORD_LOG("Lookup count:     %lu\n", lpm->lookup_count);
    CORD_LOG("TBL24 size:       %zu MB\n", (CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t)) / (1024 * 1024));
    CORD_LOG("TBL8 size:        %zu KB\n",
           (lpm->tbl8_used_count * CORD_IPV4_LPM_TBL8_SIZE * sizeof(cord_ipv4_lpm_entry_t)) / 1024);
    CORD_LOG("===========================\n");
}

//
// MAC LPM Implementation (DIR-24-24 Algorithm for 48-bit)
//

cord_mac_lpm_t *cord_mac_lpm_create(uint32_t max_routes)
{
    cord_mac_lpm_t *lpm = calloc(1, sizeof(cord_mac_lpm_t));
    if (!lpm)
    {
        return NULL;
    }

    lpm->max_routes = max_routes;

    // Allocate TBL24_HI (first 24 bits - 128MB)
    // Allocate TBL24_HI (huge allocation, may use huge pages)
    size_t tbl24_size = CORD_MAC_LPM_TBL24_SIZE * sizeof(cord_mac_lpm_entry_t);
    lpm->tbl24_hi = cord_alloc_hugepage(tbl24_size);
    if (!lpm->tbl24_hi)
    {
        free(lpm);
        return NULL;
    }

    // Allocate TBL24_LO group pointer array
    lpm->tbl24_lo_groups = calloc(CORD_MAC_LPM_TBL24_MAX_GROUPS, sizeof(cord_mac_lpm_entry_t *));
    if (!lpm->tbl24_lo_groups)
    {
        cord_free_hugepage(lpm->tbl24_hi, tbl24_size);
        free(lpm);
        return NULL;
    }

    // Initialize free list
    for (uint32_t i = 0; i < CORD_MAC_LPM_TBL24_MAX_GROUPS; i++)
    {
        lpm->group_free_list[i] = i;
    }
    lpm->group_free_count = CORD_MAC_LPM_TBL24_MAX_GROUPS;
    lpm->groups_used = 0;

    lpm->lookup_count = 0;
    lpm->routes_count = 0;

    return lpm;
}

void cord_mac_lpm_destroy(cord_mac_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    // Free all TBL24_LO groups
    for (uint32_t i = 0; i < CORD_MAC_LPM_TBL24_MAX_GROUPS; i++)
    {
        if (lpm->tbl24_lo_groups[i])
        {
            free(lpm->tbl24_lo_groups[i]);
        }
    }

    free(lpm->tbl24_lo_groups);

    size_t tbl24_size = CORD_MAC_LPM_TBL24_SIZE * sizeof(cord_mac_lpm_entry_t);
    cord_free_hugepage(lpm->tbl24_hi, tbl24_size);

    free(lpm);
}

int cord_mac_lpm_add(cord_mac_lpm_t *lpm, const cord_mac_addr_t *mac, uint8_t depth, uint32_t next_hop)
{
    uint64_t mac64;
    cord_mac_to_u64(mac, &mac64);
    return cord_mac_lpm_add_u64(lpm, mac64, depth, next_hop);
}

int cord_mac_lpm_add_u64(cord_mac_lpm_t *lpm, uint64_t mac48, uint8_t depth, uint32_t next_hop)
{
    if (!lpm || depth > 48)
    {
        return -1;
    }

    // Normalize MAC to prefix
    uint64_t mask = (depth == 48) ? 0xFFFFFFFFFFFFULL : ~((1ULL << (48 - depth)) - 1);
    mac48 = mac48 & mask;

    if (depth <= 24)
    {
        // Route fits in TBL24_HI
        uint32_t hi_start = mac48 >> 24;
        uint32_t num_entries = 1 << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = hi_start + i;
            if (!lpm->tbl24_hi[idx].valid || depth >= lpm->tbl24_hi[idx].depth)
            {
                lpm->tbl24_hi[idx].next_hop = next_hop;
                lpm->tbl24_hi[idx].depth = depth;
                lpm->tbl24_hi[idx].valid = 1;
                lpm->tbl24_hi[idx].ext_entry = 0;
            }
        }
    }
    else
    {
        // Route needs TBL24_LO (depth > 24)
        uint32_t hi_idx = mac48 >> 24;
        uint32_t group_idx;

        // Check if group exists
        if (!lpm->tbl24_hi[hi_idx].ext_entry)
        {
            // Allocate new group
            if (lpm->group_free_count == 0)
            {
                return -1;
            }
            group_idx = lpm->group_free_list[--lpm->group_free_count];

            lpm->tbl24_lo_groups[group_idx] = calloc(CORD_MAC_LPM_TBL24_SIZE, sizeof(cord_mac_lpm_entry_t));
            if (!lpm->tbl24_lo_groups[group_idx])
            {
                lpm->group_free_list[lpm->group_free_count++] = group_idx;
                return -1;
            }

            // Copy TBL24_HI entry to all TBL24_LO entries
            cord_mac_lpm_entry_t hi_entry = lpm->tbl24_hi[hi_idx];
            for (uint32_t i = 0; i < CORD_MAC_LPM_TBL24_SIZE; i++)
            {
                lpm->tbl24_lo_groups[group_idx][i] = hi_entry;
                lpm->tbl24_lo_groups[group_idx][i].ext_entry = 0;
            }

            lpm->tbl24_hi[hi_idx].ext_entry = 1;
            lpm->tbl24_hi[hi_idx].group_idx = group_idx;
            lpm->groups_used++;
        }
        else
        {
            group_idx = lpm->tbl24_hi[hi_idx].group_idx;
        }

        // Update TBL24_LO entries
        uint32_t lo_start = mac48 & 0xFFFFFF;
        uint32_t num_entries = 1 << (48 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = lo_start + i;
            if (idx >= CORD_MAC_LPM_TBL24_SIZE)
            {
                break;
            }

            if (!lpm->tbl24_lo_groups[group_idx][idx].valid || depth >= lpm->tbl24_lo_groups[group_idx][idx].depth)
            {
                lpm->tbl24_lo_groups[group_idx][idx].next_hop = next_hop;
                lpm->tbl24_lo_groups[group_idx][idx].depth = depth;
                lpm->tbl24_lo_groups[group_idx][idx].valid = 1;
                lpm->tbl24_lo_groups[group_idx][idx].ext_entry = 0;
            }
        }
    }

    lpm->routes_count++;
    return 0;
}

int cord_mac_lpm_delete(cord_mac_lpm_t *lpm, const cord_mac_addr_t *mac, uint8_t depth)
{
    uint64_t mac64;
    cord_mac_to_u64(mac, &mac64);

    if (!lpm || depth > 48)
    {
        return -1;
    }

    uint64_t mask = (depth == 48) ? 0xFFFFFFFFFFFFULL : ~((1ULL << (48 - depth)) - 1);
    mac64 = mac64 & mask;

    if (depth <= 24)
    {
        uint32_t hi_start = mac64 >> 24;
        uint32_t num_entries = 1 << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = hi_start + i;
            if (lpm->tbl24_hi[idx].depth == depth)
            {
                lpm->tbl24_hi[idx].valid = 0;
                lpm->tbl24_hi[idx].next_hop = 0;
                lpm->tbl24_hi[idx].depth = 0;
            }
        }
    }
    else
    {
        uint32_t hi_idx = mac64 >> 24;

        if (lpm->tbl24_hi[hi_idx].ext_entry)
        {
            uint32_t group_idx = lpm->tbl24_hi[hi_idx].group_idx;
            uint32_t lo_start = mac64 & 0xFFFFFF;
            uint32_t num_entries = 1 << (48 - depth);

            for (uint32_t i = 0; i < num_entries; i++)
            {
                uint32_t idx = lo_start + i;
                if (idx >= CORD_MAC_LPM_TBL24_SIZE)
                {
                    break;
                }

                if (lpm->tbl24_lo_groups[group_idx][idx].depth == depth)
                {
                    lpm->tbl24_lo_groups[group_idx][idx].valid = 0;
                    lpm->tbl24_lo_groups[group_idx][idx].next_hop = 0;
                    lpm->tbl24_lo_groups[group_idx][idx].depth = 0;
                }
            }
        }
    }

    lpm->routes_count--;
    return 0;
}

void cord_mac_lpm_print_stats(const cord_mac_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    CORD_LOG("=== MAC LPM Statistics ===\n");
    CORD_LOG("Routes:             %u\n", lpm->routes_count);
    CORD_LOG("Max routes:         %u\n", lpm->max_routes);
    CORD_LOG("TBL24_LO groups:    %u / %u\n", lpm->groups_used, CORD_MAC_LPM_TBL24_MAX_GROUPS);
    CORD_LOG("Lookup count:       %lu\n", lpm->lookup_count);
    CORD_LOG("TBL24_HI size:      %zu MB\n", (CORD_MAC_LPM_TBL24_SIZE * sizeof(cord_mac_lpm_entry_t)) / (1024 * 1024));
    CORD_LOG("TBL24_LO size:      %zu MB\n",
           (lpm->groups_used * CORD_MAC_LPM_TBL24_SIZE * sizeof(cord_mac_lpm_entry_t)) / (1024 * 1024));
    CORD_LOG("==========================\n");
}

//
// IPv6 LPM Implementation (Compressed Multi-bit Trie for 128-bit)
//

cord_ipv6_lpm_t *cord_ipv6_lpm_create(uint32_t max_routes)
{
    cord_ipv6_lpm_t *lpm = calloc(1, sizeof(cord_ipv6_lpm_t));
    if (!lpm)
    {
        return NULL;
    }

    lpm->max_routes = max_routes;

    // Allocate root node
    lpm->root = calloc(1, sizeof(cord_ipv6_lpm_node_t));
    if (!lpm->root)
    {
        free(lpm);
        return NULL;
    }

    // Allocate TBL16 for fast path (256KB)
    lpm->tbl16 = calloc(CORD_IPV6_LPM_TBL16_SIZE, sizeof(uint32_t));
    lpm->tbl16_valid = calloc(CORD_IPV6_LPM_TBL16_SIZE / 8, sizeof(uint8_t));
    if (!lpm->tbl16 || !lpm->tbl16_valid)
    {
        free(lpm->root);
        free(lpm->tbl16);
        free(lpm->tbl16_valid);
        free(lpm);
        return NULL;
    }

    // Initialize TBL16 with invalid entries
    for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL16_SIZE; i++)
    {
        lpm->tbl16[i] = CORD_IPV6_LPM_INVALID_NEXT_HOP;
    }

    // Allocate node pool (simplified - just use dynamic allocation)
    lpm->node_pool_capacity = max_routes * 8; // Estimate
    lpm->nodes_allocated = 1;                 // Root node

    lpm->lookup_count = 0;
    lpm->routes_count = 0;
    lpm->max_tree_depth = 0;

    return lpm;
}

void cord_ipv6_lpm_destroy(cord_ipv6_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    // TODO: Recursive free of trie nodes
    // For now, simplified cleanup
    free(lpm->root);
    free(lpm->tbl16);
    free(lpm->tbl16_valid);
    free(lpm);
}

// Simplified IPv6 LPM add (linear search fallback)
int cord_ipv6_lpm_add(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth, uint32_t next_hop)
{
    if (!lpm || depth > 128)
    {
        return -1;
    }

    // Fast path: Update TBL16 if depth <= 16
    if (depth <= 16)
    {
        uint16_t prefix16 = (ip->addr[0] << 8) | ip->addr[1];
        uint32_t num_entries = 1 << (16 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = (prefix16 >> (16 - depth)) << (16 - depth) | i;
            lpm->tbl16[idx] = next_hop;
            lpm->tbl16_valid[idx / 8] |= (1 << (idx % 8));
        }
    }

    // TODO: Full trie implementation for depth > 16

    lpm->routes_count++;
    return 0;
}

int cord_ipv6_lpm_delete(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth)
{
    if (!lpm || depth > 128)
    {
        return -1;
    }

    // Fast path: Clear TBL16 if depth <= 16
    if (depth <= 16)
    {
        uint16_t prefix16 = (ip->addr[0] << 8) | ip->addr[1];
        uint32_t num_entries = 1 << (16 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = (prefix16 >> (16 - depth)) << (16 - depth) | i;
            lpm->tbl16[idx] = CORD_IPV6_LPM_INVALID_NEXT_HOP;
            lpm->tbl16_valid[idx / 8] &= ~(1 << (idx % 8));
        }
    }

    // TODO: Full trie deletion for depth > 16

    lpm->routes_count--;
    return 0;
}

uint32_t cord_ipv6_lpm_lookup(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip)
{
    if (!lpm)
    {
        return CORD_IPV6_LPM_INVALID_NEXT_HOP;
    }

    // Fast path: Check TBL16
    uint16_t prefix16 = (ip->addr[0] << 8) | ip->addr[1];
    if (lpm->tbl16_valid[prefix16 / 8] & (1 << (prefix16 % 8)))
    {
        return lpm->tbl16[prefix16];
    }

    // TODO: Full trie traversal for longer prefixes

    return CORD_IPV6_LPM_INVALID_NEXT_HOP;
}

void cord_ipv6_lpm_lookup_batch(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ips, uint32_t *next_hops,
                                uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        next_hops[i] = cord_ipv6_lpm_lookup(lpm, &ips[i]);
    }
}

void cord_ipv6_lpm_print_stats(const cord_ipv6_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    CORD_LOG("=== IPv6 LPM Statistics ===\n");
    CORD_LOG("Routes:           %u\n", lpm->routes_count);
    CORD_LOG("Max routes:       %u\n", lpm->max_routes);
    CORD_LOG("Nodes allocated:  %u\n", lpm->nodes_allocated);
    CORD_LOG("Max tree depth:   %u\n", lpm->max_tree_depth);
    CORD_LOG("Lookup count:     %lu\n", lpm->lookup_count);
    CORD_LOG("TBL16 size:       256 KB\n");
    CORD_LOG("===========================\n");
}

//
// Generic LPM Wrapper
//

cord_lpm_table_t *cord_lpm_create(cord_lpm_type_t type, uint32_t max_routes)
{
    cord_lpm_table_t *table = calloc(1, sizeof(cord_lpm_table_t));
    if (!table)
    {
        return NULL;
    }

    table->type = type;

    switch (type)
    {
    case CORD_LPM_TYPE_IPV4:
        table->impl.ipv4 = cord_ipv4_lpm_create(max_routes);
        if (!table->impl.ipv4)
        {
            free(table);
            return NULL;
        }
        break;

    case CORD_LPM_TYPE_MAC:
        table->impl.mac = cord_mac_lpm_create(max_routes);
        if (!table->impl.mac)
        {
            free(table);
            return NULL;
        }
        break;

    case CORD_LPM_TYPE_IPV6:
        table->impl.ipv6 = cord_ipv6_lpm_create(max_routes);
        if (!table->impl.ipv6)
        {
            free(table);
            return NULL;
        }
        break;

    default:
        free(table);
        return NULL;
    }

    return table;
}

void cord_lpm_destroy(cord_lpm_table_t *table)
{
    if (!table)
    {
        return;
    }

    switch (table->type)
    {
    case CORD_LPM_TYPE_IPV4:
        cord_ipv4_lpm_destroy(table->impl.ipv4);
        break;
    case CORD_LPM_TYPE_MAC:
        cord_mac_lpm_destroy(table->impl.mac);
        break;
    case CORD_LPM_TYPE_IPV6:
        cord_ipv6_lpm_destroy(table->impl.ipv6);
        break;
    }

    free(table);
}

int cord_lpm_add(cord_lpm_table_t *table, const cord_lpm_addr_t *addr, uint8_t depth, uint32_t next_hop)
{
    if (!table || !addr)
    {
        return -1;
    }

    switch (table->type)
    {
    case CORD_LPM_TYPE_IPV4:
        return cord_ipv4_lpm_add(table->impl.ipv4, addr->ipv4, depth, next_hop);
    case CORD_LPM_TYPE_MAC:
        return cord_mac_lpm_add(table->impl.mac, &addr->mac, depth, next_hop);
    case CORD_LPM_TYPE_IPV6:
        return cord_ipv6_lpm_add(table->impl.ipv6, &addr->ipv6, depth, next_hop);
    default:
        return -1;
    }
}

int cord_lpm_delete(cord_lpm_table_t *table, const cord_lpm_addr_t *addr, uint8_t depth)
{
    if (!table || !addr)
    {
        return -1;
    }

    switch (table->type)
    {
    case CORD_LPM_TYPE_IPV4:
        return cord_ipv4_lpm_delete(table->impl.ipv4, addr->ipv4, depth);
    case CORD_LPM_TYPE_MAC:
        return cord_mac_lpm_delete(table->impl.mac, &addr->mac, depth);
    case CORD_LPM_TYPE_IPV6:
        return cord_ipv6_lpm_delete(table->impl.ipv6, &addr->ipv6, depth);
    default:
        return -1;
    }
}

uint32_t cord_lpm_lookup(const cord_lpm_table_t *table, const cord_lpm_addr_t *addr)
{
    if (!table || !addr)
    {
        return 0xFFFFFFFF;
    }

    switch (table->type)
    {
    case CORD_LPM_TYPE_IPV4:
        return cord_ipv4_lpm_lookup(table->impl.ipv4, addr->ipv4);
    case CORD_LPM_TYPE_MAC:
        return cord_mac_lpm_lookup(table->impl.mac, &addr->mac);
    case CORD_LPM_TYPE_IPV6:
        return cord_ipv6_lpm_lookup(table->impl.ipv6, &addr->ipv6);
    default:
        return 0xFFFFFFFF;
    }
}

void cord_lpm_print_stats(const cord_lpm_table_t *table)
{
    if (!table)
    {
        return;
    }

    switch (table->type)
    {
    case CORD_LPM_TYPE_IPV4:
        cord_ipv4_lpm_print_stats(table->impl.ipv4);
        break;
    case CORD_LPM_TYPE_MAC:
        cord_mac_lpm_print_stats(table->impl.mac);
        break;
    case CORD_LPM_TYPE_IPV6:
        cord_ipv6_lpm_print_stats(table->impl.ipv6);
        break;
    }
}

//
// Helper Functions
//

void cord_mac_to_u64(const cord_mac_addr_t *mac, uint64_t *mac64)
{
    *mac64 = ((uint64_t) mac->addr[0] << 40) | ((uint64_t) mac->addr[1] << 32) | ((uint64_t) mac->addr[2] << 24) |
             ((uint64_t) mac->addr[3] << 16) | ((uint64_t) mac->addr[4] << 8) | ((uint64_t) mac->addr[5]);
}

void cord_u64_to_mac(uint64_t mac64, cord_mac_addr_t *mac)
{
    mac->addr[0] = (mac64 >> 40) & 0xFF;
    mac->addr[1] = (mac64 >> 32) & 0xFF;
    mac->addr[2] = (mac64 >> 24) & 0xFF;
    mac->addr[3] = (mac64 >> 16) & 0xFF;
    mac->addr[4] = (mac64 >> 8) & 0xFF;
    mac->addr[5] = mac64 & 0xFF;
}

int cord_mac_str_to_binary(const char *mac_str, cord_mac_addr_t *mac)
{
    int values[6];
    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) !=
        6)
    {
        return -1;
    }

    for (int i = 0; i < 6; i++)
    {
        mac->addr[i] = (uint8_t) values[i];
    }

    return 0;
}

int cord_parse_cidr_mac(const char *cidr, cord_mac_addr_t *mac, uint8_t *depth)
{
    char mac_str[18];
    int prefix_len;

    if (sscanf(cidr, "%17[^/]/%d", mac_str, &prefix_len) != 2)
    {
        return -1;
    }

    if (prefix_len < 0 || prefix_len > 48)
    {
        return -1;
    }

    if (cord_mac_str_to_binary(mac_str, mac) != 0)
    {
        return -1;
    }

    *depth = (uint8_t) prefix_len;
    return 0;
}

void cord_mac_to_str(const cord_mac_addr_t *mac, char *buf, size_t len)
{
    snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x", mac->addr[0], mac->addr[1], mac->addr[2], mac->addr[3],
             mac->addr[4], mac->addr[5]);
}

int cord_ipv6_str_to_binary(const char *ip_str, cord_ipv6_addr_t *ip)
{
    return inet_pton(AF_INET6, ip_str, ip->addr) == 1 ? 0 : -1;
}

int cord_parse_cidr_ipv6(const char *cidr, cord_ipv6_addr_t *ip, uint8_t *depth)
{
    char ip_str[40];
    int prefix_len;

    if (sscanf(cidr, "%39[^/]/%d", ip_str, &prefix_len) != 2)
    {
        return -1;
    }

    if (prefix_len < 0 || prefix_len > 128)
    {
        return -1;
    }

    if (cord_ipv6_str_to_binary(ip_str, ip) != 0)
    {
        return -1;
    }

    *depth = (uint8_t) prefix_len;
    return 0;
}

void cord_ipv6_to_str(const cord_ipv6_addr_t *ip, char *buf, size_t len)
{
    inet_ntop(AF_INET6, ip->addr, buf, len);
}

//
// Layer 2 CAM Table Implementation (Exact Match: MAC → Port)
//

static inline uint32_t mac_hash(const cord_mac_addr_t *mac, uint16_t vlan_id, uint32_t num_buckets)
{
    uint64_t mac64;
    cord_mac_to_u64(mac, &mac64);

    uint64_t hash = mac64 ^ ((uint64_t) vlan_id << 32);

    return (uint32_t) (hash % num_buckets);
}

static inline bool mac_equal(const cord_mac_addr_t *a, const cord_mac_addr_t *b)
{
    return memcmp(a->addr, b->addr, 6) == 0;
}

cord_l2_cam_t *cord_l2_cam_create(uint32_t num_buckets, uint32_t max_entries)
{
    cord_l2_cam_t *cam = calloc(1, sizeof(cord_l2_cam_t));
    if (!cam)
    {
        return NULL;
    }

    cam->num_buckets = num_buckets;
    cam->max_entries = max_entries;
    cam->num_entries = 0;
    cam->lookup_count = 0;
    cam->hit_count = 0;
    cam->miss_count = 0;

    cam->buckets = calloc(num_buckets, sizeof(cord_l2_cam_entry_t *));
    if (!cam->buckets)
    {
        free(cam);
        return NULL;
    }

    return cam;
}

void cord_l2_cam_destroy(cord_l2_cam_t *cam)
{
    if (!cam)
    {
        return;
    }

    for (uint32_t i = 0; i < cam->num_buckets; i++)
    {
        cord_l2_cam_entry_t *entry = cam->buckets[i];
        while (entry)
        {
            cord_l2_cam_entry_t *next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(cam->buckets);
    free(cam);
}

int cord_l2_cam_add(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint32_t port_id, uint16_t vlan_id)
{
    if (!cam || !mac)
    {
        return -1;
    }

    if (cam->num_entries >= cam->max_entries)
    {
        return -1;
    }

    uint32_t bucket_idx = mac_hash(mac, vlan_id, cam->num_buckets);

    cord_l2_cam_entry_t *entry = cam->buckets[bucket_idx];
    while (entry)
    {
        if (entry->vlan_id == vlan_id && mac_equal(&entry->mac, mac))
        {
            entry->port_id = port_id;
            return 0;
        }
        entry = entry->next;
    }

    entry = calloc(1, sizeof(cord_l2_cam_entry_t));
    if (!entry)
    {
        return -1;
    }

    memcpy(&entry->mac, mac, sizeof(cord_mac_addr_t));
    entry->port_id = port_id;
    entry->vlan_id = vlan_id;
    entry->valid = 1;

    entry->next = cam->buckets[bucket_idx];
    cam->buckets[bucket_idx] = entry;

    cam->num_entries++;
    return 0;
}

int cord_l2_cam_delete(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint16_t vlan_id)
{
    if (!cam || !mac)
    {
        return -1;
    }

    uint32_t bucket_idx = mac_hash(mac, vlan_id, cam->num_buckets);

    cord_l2_cam_entry_t *entry = cam->buckets[bucket_idx];
    cord_l2_cam_entry_t *prev = NULL;

    while (entry)
    {
        if (entry->vlan_id == vlan_id && mac_equal(&entry->mac, mac))
        {
            if (prev)
            {
                prev->next = entry->next;
            }
            else
            {
                cam->buckets[bucket_idx] = entry->next;
            }

            free(entry);
            cam->num_entries--;
            return 0;
        }

        prev = entry;
        entry = entry->next;
    }

    return -1;
}

uint32_t cord_l2_cam_lookup(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint16_t vlan_id)
{
    if (!cam || !mac)
    {
        return CORD_L2_CAM_INVALID_PORT;
    }

    cam->lookup_count++;

    uint32_t bucket_idx = mac_hash(mac, vlan_id, cam->num_buckets);

    cord_l2_cam_entry_t *entry = cam->buckets[bucket_idx];
    while (entry)
    {
        if (entry->valid && entry->vlan_id == vlan_id && mac_equal(&entry->mac, mac))
        {
            cam->hit_count++;
            return entry->port_id;
        }
        entry = entry->next;
    }

    cam->miss_count++;
    return CORD_L2_CAM_INVALID_PORT;
}

void cord_l2_cam_clear(cord_l2_cam_t *cam)
{
    if (!cam)
    {
        return;
    }

    for (uint32_t i = 0; i < cam->num_buckets; i++)
    {
        cord_l2_cam_entry_t *entry = cam->buckets[i];
        while (entry)
        {
            cord_l2_cam_entry_t *next = entry->next;
            free(entry);
            entry = next;
        }
        cam->buckets[i] = NULL;
    }

    cam->num_entries = 0;
}

void cord_l2_cam_print_stats(const cord_l2_cam_t *cam)
{
    if (!cam)
    {
        return;
    }

    CORD_LOG("=== L2 CAM Statistics ===\n");
    CORD_LOG("Entries:          %u / %u\n", cam->num_entries, cam->max_entries);
    CORD_LOG("Buckets:          %u\n", cam->num_buckets);
    CORD_LOG("Lookups:          %lu\n", cam->lookup_count);
    CORD_LOG("Hits:             %lu\n", cam->hit_count);
    CORD_LOG("Misses:           %lu\n", cam->miss_count);
    if (cam->lookup_count > 0)
    {
        CORD_LOG("Hit rate:         %.2f%%\n", (double) cam->hit_count * 100.0 / cam->lookup_count);
    }
    CORD_LOG("=========================\n");
}

void cord_l2_cam_print_entries(const cord_l2_cam_t *cam)
{
    if (!cam)
    {
        return;
    }

    CORD_LOG("=== L2 CAM Entries ===\n");
    CORD_LOG("%-18s %-8s %-8s\n", "MAC Address", "VLAN", "Port");
    CORD_LOG("------------------------------------------------\n");

    uint32_t count = 0;
    for (uint32_t i = 0; i < cam->num_buckets; i++)
    {
        cord_l2_cam_entry_t *entry = cam->buckets[i];
        while (entry)
        {
            if (entry->valid)
            {
                CORD_LOG("%02x:%02x:%02x:%02x:%02x:%02x  %-8u %-8u\n", entry->mac.addr[0], entry->mac.addr[1],
                       entry->mac.addr[2], entry->mac.addr[3], entry->mac.addr[4], entry->mac.addr[5], entry->vlan_id,
                       entry->port_id);
                count++;
            }
            entry = entry->next;
        }
    }

    CORD_LOG("------------------------------------------------\n");
    CORD_LOG("Total: %u entries\n", count);
    CORD_LOG("======================\n");
}

int cord_l2_cam_add_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth, uint32_t port_id, uint16_t vlan_id,
                             bool use_src)
{
    if (!cam || !eth)
    {
        return -1;
    }

    cord_mac_addr_t mac;
    if (use_src)
    {
        cord_get_field_eth_src_addr(eth, &mac);
    }
    else
    {
        cord_get_field_eth_dst_addr(eth, &mac);
    }

    return cord_l2_cam_add(cam, &mac, port_id, vlan_id);
}

uint32_t cord_l2_cam_lookup_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth, uint16_t vlan_id, bool use_dst)
{
    if (!cam || !eth)
    {
        return CORD_L2_CAM_INVALID_PORT;
    }

    cord_mac_addr_t mac;
    if (use_dst)
    {
        cord_get_field_eth_dst_addr(eth, &mac);
    }
    else
    {
        cord_get_field_eth_src_addr(eth, &mac);
    }

    return cord_l2_cam_lookup(cam, &mac, vlan_id);
}

uint32_t cord_l2_cam_lookup_from_vlan(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth, const cord_vlan_hdr_t *vlan,
                                      bool use_dst)
{
    if (!cam || !eth || !vlan)
    {
        return CORD_L2_CAM_INVALID_PORT;
    }

    cord_mac_addr_t mac;
    if (use_dst)
    {
        cord_get_field_eth_dst_addr(eth, &mac);
    }
    else
    {
        cord_get_field_eth_src_addr(eth, &mac);
    }

    uint16_t vlan_id = cord_get_field_vlan_vid_ntohs(vlan);

    return cord_l2_cam_lookup(cam, &mac, vlan_id);
}
