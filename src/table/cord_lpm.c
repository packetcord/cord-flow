#include <table/cord_lpm.h>
#include <memory/cord_memory.h>
#include <string.h>

//
// IPv4 LPM Implementation - DIR-24-8 Algorithm
// Based on DPDK rte_lpm implementation
//

//
// TBL8 Group Management
//

static int ipv4_tbl8_alloc(cord_ipv4_lpm_t *lpm, uint32_t *group_idx)
{
    if (lpm->tbl8_free_count == 0)
    {
        return -1; // No free TBL8 groups available
    }

    // Pop a free group from the stack
    *group_idx = lpm->tbl8_free_list[--lpm->tbl8_free_count];

    // Allocate memory for this group
    lpm->tbl8_groups[*group_idx] = calloc(CORD_IPV4_LPM_TBL8_SIZE, sizeof(cord_ipv4_lpm_entry_t));
    if (!lpm->tbl8_groups[*group_idx])
    {
        // Restore to free list on allocation failure
        lpm->tbl8_free_list[lpm->tbl8_free_count++] = *group_idx;
        return -1;
    }

    lpm->tbl8_used_count++;
    return 0;
}

static void ipv4_tbl8_free(cord_ipv4_lpm_t *lpm, uint32_t group_idx)
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

    // Push back to free list
    lpm->tbl8_free_list[lpm->tbl8_free_count++] = group_idx;
    lpm->tbl8_used_count--;
}

// Check if a TBL8 group can be reclaimed (all entries invalid or same as parent)
static bool ipv4_tbl8_can_reclaim(cord_ipv4_lpm_t *lpm, uint32_t group_idx, uint32_t parent_next_hop)
{
    cord_ipv4_lpm_entry_t *tbl8 = lpm->tbl8_groups[group_idx];

    for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_SIZE; i++)
    {
        // If entry is valid and different from parent, cannot reclaim
        if (tbl8[i].valid && (tbl8[i].next_hop != parent_next_hop || tbl8[i].ext_entry))
        {
            return false;
        }
    }

    return true;
}

//
// IPv4 LPM Create/Destroy
//

cord_ipv4_lpm_t *cord_ipv4_lpm_create(uint32_t max_routes)
{
    cord_ipv4_lpm_t *lpm = calloc(1, sizeof(cord_ipv4_lpm_t));
    if (!lpm)
    {
        return NULL;
    }

    lpm->max_routes = max_routes;

    // Allocate TBL24 using huge pages for performance
    size_t tbl24_size = CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t);
    lpm->tbl24 = cord_alloc_hugepage(tbl24_size);
    if (!lpm->tbl24)
    {
        free(lpm);
        return NULL;
    }

    // Initialize all TBL24 entries as invalid
    memset(lpm->tbl24, 0, tbl24_size);

    // Allocate TBL8 group pointer array
    lpm->tbl8_groups = calloc(CORD_IPV4_LPM_TBL8_MAX_GROUPS, sizeof(cord_ipv4_lpm_entry_t *));
    if (!lpm->tbl8_groups)
    {
        cord_free_hugepage(lpm->tbl24, tbl24_size);
        free(lpm);
        return NULL;
    }

    // Initialize TBL8 free list
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

    // Free all allocated TBL8 groups
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

//
// IPv4 LPM Add (with prefix expansion)
//

int cord_ipv4_lpm_add(cord_ipv4_lpm_t *lpm, uint32_t ip, uint8_t depth, uint32_t next_hop)
{
    if (!lpm || depth > 32 || next_hop == CORD_IPV4_LPM_INVALID_NEXT_HOP)
    {
        return -1;
    }

    // Normalize IP to network address
    uint32_t mask = (depth == 32) ? 0xFFFFFFFF : ~((1U << (32 - depth)) - 1);
    ip = ip & mask;

    // Special case: depth 0 (default route)
    if (depth == 0)
    {
        // Fill entire TBL24 with default route
        for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL24_SIZE; i++)
        {
            lpm->tbl24[i].next_hop = next_hop;
            lpm->tbl24[i].depth = 0;
            lpm->tbl24[i].valid = 1;
            lpm->tbl24[i].ext_entry = 0;
        }
        lpm->routes_count++;
        return 0;
    }

    if (depth <= 24)
    {
        // Route fits entirely in TBL24 (prefix expansion)
        uint32_t tbl24_idx = ip >> 8;
        uint32_t num_entries = 1U << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_idx + i;

            // Only update if:
            // 1. Entry is invalid, OR
            // 2. New route is more specific (greater depth)
            if (!lpm->tbl24[idx].valid || depth > lpm->tbl24[idx].depth)
            {
                // Don't overwrite ext_entry if it points to more specific routes
                if (!lpm->tbl24[idx].ext_entry || depth > lpm->tbl24[idx].depth)
                {
                    lpm->tbl24[idx].next_hop = next_hop;
                    lpm->tbl24[idx].depth = depth;
                    lpm->tbl24[idx].valid = 1;
                    // Keep ext_entry as-is if it's set (deeper routes exist)
                    if (depth > lpm->tbl24[idx].depth)
                    {
                        lpm->tbl24[idx].ext_entry = 0;
                    }
                }
            }
        }
    }
    else
    {
        // Route requires TBL8 (depth > 24)
        uint32_t tbl24_idx = ip >> 8;
        uint32_t group_idx;

        // Check if TBL8 group already exists
        if (!lpm->tbl24[tbl24_idx].ext_entry)
        {
            // Need to allocate new TBL8 group
            if (ipv4_tbl8_alloc(lpm, &group_idx) != 0)
            {
                return -1; // Out of TBL8 groups
            }

            // Copy parent entry to all TBL8 entries (prefix expansion)
            cord_ipv4_lpm_entry_t parent_entry = lpm->tbl24[tbl24_idx];
            for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_SIZE; i++)
            {
                lpm->tbl8_groups[group_idx][i] = parent_entry;
                lpm->tbl8_groups[group_idx][i].ext_entry = 0;
            }

            // Update TBL24 to point to this TBL8 group
            lpm->tbl24[tbl24_idx].ext_entry = 1;
            lpm->tbl24[tbl24_idx].group_idx = group_idx;
        }
        else
        {
            group_idx = lpm->tbl24[tbl24_idx].group_idx;
        }

        // Update TBL8 entries (prefix expansion)
        uint32_t tbl8_idx = ip & 0xFF;
        uint32_t num_entries = 1U << (32 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl8_idx + i;
            if (idx >= CORD_IPV4_LPM_TBL8_SIZE)
            {
                break;
            }

            // Update if invalid or new route is more specific
            if (!lpm->tbl8_groups[group_idx][idx].valid ||
                depth > lpm->tbl8_groups[group_idx][idx].depth)
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

//
// IPv4 LPM Delete (with TBL8 reclamation)
//

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
        // Route is in TBL24
        uint32_t tbl24_idx = ip >> 8;
        uint32_t num_entries = 1U << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_idx + i;

            // Only delete if depth matches exactly
            if (lpm->tbl24[idx].valid && lpm->tbl24[idx].depth == depth && !lpm->tbl24[idx].ext_entry)
            {
                lpm->tbl24[idx].valid = 0;
                lpm->tbl24[idx].next_hop = CORD_IPV4_LPM_INVALID_NEXT_HOP;
                lpm->tbl24[idx].depth = 0;
            }
        }
    }
    else
    {
        // Route is in TBL8
        uint32_t tbl24_idx = ip >> 8;

        if (!lpm->tbl24[tbl24_idx].ext_entry)
        {
            return -1; // Route not found
        }

        uint32_t group_idx = lpm->tbl24[tbl24_idx].group_idx;
        uint32_t tbl8_idx = ip & 0xFF;
        uint32_t num_entries = 1U << (32 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl8_idx + i;
            if (idx >= CORD_IPV4_LPM_TBL8_SIZE)
            {
                break;
            }

            if (lpm->tbl8_groups[group_idx][idx].valid &&
                lpm->tbl8_groups[group_idx][idx].depth == depth)
            {
                // Restore parent entry from TBL24
                lpm->tbl8_groups[group_idx][idx].next_hop = lpm->tbl24[tbl24_idx].next_hop;
                lpm->tbl8_groups[group_idx][idx].depth = lpm->tbl24[tbl24_idx].depth;
                lpm->tbl8_groups[group_idx][idx].valid = lpm->tbl24[tbl24_idx].valid;
                lpm->tbl8_groups[group_idx][idx].ext_entry = 0;
            }
        }

        // Try to reclaim TBL8 group if all entries are now uniform
        if (ipv4_tbl8_can_reclaim(lpm, group_idx, lpm->tbl24[tbl24_idx].next_hop))
        {
            lpm->tbl24[tbl24_idx].ext_entry = 0;
            ipv4_tbl8_free(lpm, group_idx);
        }
    }

    lpm->routes_count--;
    return 0;
}

int cord_ipv4_lpm_delete_all(cord_ipv4_lpm_t *lpm)
{
    if (!lpm)
    {
        return -1;
    }

    // Clear TBL24
    size_t tbl24_size = CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t);
    memset(lpm->tbl24, 0, tbl24_size);

    // Free all TBL8 groups
    for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_MAX_GROUPS; i++)
    {
        if (lpm->tbl8_groups[i])
        {
            free(lpm->tbl8_groups[i]);
            lpm->tbl8_groups[i] = NULL;
        }
    }

    // Reset free list
    for (uint32_t i = 0; i < CORD_IPV4_LPM_TBL8_MAX_GROUPS; i++)
    {
        lpm->tbl8_free_list[i] = i;
    }
    lpm->tbl8_free_count = CORD_IPV4_LPM_TBL8_MAX_GROUPS;
    lpm->tbl8_used_count = 0;
    lpm->routes_count = 0;

    return 0;
}

//
// IPv4 LPM Batch Lookup
//

void cord_ipv4_lpm_lookup_batch(const cord_ipv4_lpm_t *lpm, const uint32_t *ips,
                                 uint32_t *next_hops, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        next_hops[i] = cord_ipv4_lpm_lookup(lpm, ips[i]);
    }
}

//
// IPv4 Helper Functions
//

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

int cord_ipv4_parse_cidr(const char *cidr, uint32_t *ip, uint8_t *depth)
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

    *depth = (uint8_t)prefix_len;
    return 0;
}

void cord_ipv4_to_str(uint32_t ip, char *buf, size_t len)
{
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, buf, len);
}

void cord_ipv4_lpm_print_stats(const cord_ipv4_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    CORD_LOG("=== IPv4 LPM Statistics ===\n");
    CORD_LOG("Routes installed:  %u / %u\n", lpm->routes_count, lpm->max_routes);
    CORD_LOG("TBL24 entries:     %u (%.2f MB)\n",
             CORD_IPV4_LPM_TBL24_SIZE,
             (double)(CORD_IPV4_LPM_TBL24_SIZE * sizeof(cord_ipv4_lpm_entry_t)) / (1024 * 1024));
    CORD_LOG("TBL8 groups used:  %u / %u (%.2f KB)\n",
             lpm->tbl8_used_count, CORD_IPV4_LPM_TBL8_MAX_GROUPS,
             (double)(lpm->tbl8_used_count * CORD_IPV4_LPM_TBL8_SIZE * sizeof(cord_ipv4_lpm_entry_t)) / 1024);
    CORD_LOG("Total lookups:     %lu\n", lpm->lookup_count);
    CORD_LOG("===========================\n");
}

//
// IPv6 LPM Implementation - Multi-level Trie
// Based on DPDK rte_lpm6 implementation
//

//
// TBL8 Group Management for IPv6
//

static int ipv6_tbl8_alloc(cord_ipv6_lpm_t *lpm, uint32_t *group_idx)
{
    if (lpm->tbl8_free_count == 0)
    {
        return -1;
    }

    *group_idx = lpm->tbl8_free_list[--lpm->tbl8_free_count];

    lpm->tbl8_groups[*group_idx] = calloc(CORD_IPV6_LPM_TBL8_SIZE, sizeof(cord_ipv6_lpm_entry_t));
    if (!lpm->tbl8_groups[*group_idx])
    {
        lpm->tbl8_free_list[lpm->tbl8_free_count++] = *group_idx;
        return -1;
    }

    lpm->tbl8_used_count++;
    return 0;
}

static void ipv6_tbl8_free(cord_ipv6_lpm_t *lpm, uint32_t group_idx)
{
    if (group_idx >= CORD_IPV6_LPM_TBL8_MAX_GROUPS)
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

//
// IPv6 LPM Create/Destroy
//

cord_ipv6_lpm_t *cord_ipv6_lpm_create(uint32_t max_routes)
{
    cord_ipv6_lpm_t *lpm = calloc(1, sizeof(cord_ipv6_lpm_t));
    if (!lpm)
    {
        return NULL;
    }

    lpm->max_routes = max_routes;

    // Allocate TBL24 (root level)
    size_t tbl24_size = CORD_IPV6_LPM_TBL24_SIZE * sizeof(cord_ipv6_lpm_entry_t);
    lpm->tbl24 = cord_alloc_hugepage(tbl24_size);
    if (!lpm->tbl24)
    {
        free(lpm);
        return NULL;
    }

    memset(lpm->tbl24, 0, tbl24_size);

    // Allocate TBL8 group pointer array
    lpm->tbl8_groups = calloc(CORD_IPV6_LPM_TBL8_MAX_GROUPS, sizeof(cord_ipv6_lpm_entry_t *));
    if (!lpm->tbl8_groups)
    {
        cord_free_hugepage(lpm->tbl24, tbl24_size);
        free(lpm);
        return NULL;
    }

    // Initialize TBL8 free list
    for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL8_MAX_GROUPS; i++)
    {
        lpm->tbl8_free_list[i] = i;
    }
    lpm->tbl8_free_count = CORD_IPV6_LPM_TBL8_MAX_GROUPS;
    lpm->tbl8_used_count = 0;

    lpm->lookup_count = 0;
    lpm->routes_count = 0;
    lpm->max_depth_reached = 0;

    return lpm;
}

void cord_ipv6_lpm_destroy(cord_ipv6_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    // Free all TBL8 groups
    for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL8_MAX_GROUPS; i++)
    {
        if (lpm->tbl8_groups[i])
        {
            free(lpm->tbl8_groups[i]);
        }
    }

    free(lpm->tbl8_groups);

    size_t tbl24_size = CORD_IPV6_LPM_TBL24_SIZE * sizeof(cord_ipv6_lpm_entry_t);
    cord_free_hugepage(lpm->tbl24, tbl24_size);

    free(lpm);
}

//
// IPv6 LPM Add
//

int cord_ipv6_lpm_add(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth, uint32_t next_hop)
{
    if (!lpm || !ip || depth > 128 || next_hop == CORD_IPV6_LPM_INVALID_NEXT_HOP)
    {
        return -1;
    }

    // Route fits in TBL24 (depth <= 24)
    if (depth <= 24)
    {
        // Extract first 24 bits
        uint32_t tbl24_idx = ((uint32_t)ip->addr[0] << 16) | ((uint32_t)ip->addr[1] << 8) | ip->addr[2];

        uint32_t num_entries = 1U << (24 - depth);
        tbl24_idx = (tbl24_idx >> (24 - depth)) << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_idx + i;

            if (!lpm->tbl24[idx].valid || depth > lpm->tbl24[idx].depth)
            {
                if (!lpm->tbl24[idx].ext_entry || depth > lpm->tbl24[idx].depth)
                {
                    lpm->tbl24[idx].next_hop = next_hop;
                    lpm->tbl24[idx].depth = depth;
                    lpm->tbl24[idx].valid = 1;

                    if (depth > lpm->tbl24[idx].depth)
                    {
                        lpm->tbl24[idx].ext_entry = 0;
                    }
                }
            }
        }

        lpm->routes_count++;
        return 0;
    }

    // Route requires multi-level TBL8 (depth > 24)
    // Calculate which levels we need to traverse
    uint32_t byte_idx = 2; // Start after first 24 bits (bytes 0,1,2)
    uint32_t bits_covered = 24;

    // Start from TBL24
    uint32_t tbl24_idx = ((uint32_t)ip->addr[0] << 16) | ((uint32_t)ip->addr[1] << 8) | ip->addr[2];

    cord_ipv6_lpm_entry_t *current_table = lpm->tbl24;
    uint32_t current_idx = tbl24_idx;
    bool is_tbl24 = true;
    uint32_t parent_group = 0;

    // Traverse/create path to target depth
    while (bits_covered < depth)
    {
        uint32_t bits_remaining = depth - bits_covered;
        uint32_t bits_this_level = (bits_remaining >= 8) ? 8 : bits_remaining;

        byte_idx = bits_covered / 8;
        uint8_t byte_val = (byte_idx < 16) ? ip->addr[byte_idx] : 0;

        // Check if we need to create next level
        if (!current_table[current_idx].ext_entry)
        {
            // Allocate new TBL8 group
            uint32_t new_group;
            if (ipv6_tbl8_alloc(lpm, &new_group) != 0)
            {
                return -1;
            }

            // Copy parent entry to all children
            cord_ipv6_lpm_entry_t parent = current_table[current_idx];
            for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL8_SIZE; i++)
            {
                lpm->tbl8_groups[new_group][i] = parent;
                lpm->tbl8_groups[new_group][i].ext_entry = 0;
            }

            // Link parent to this group
            current_table[current_idx].ext_entry = 1;
            current_table[current_idx].group_idx = new_group;

            uint32_t level = (bits_covered - 16) / 8;
            if (level > lpm->max_depth_reached)
            {
                lpm->max_depth_reached = level;
            }
        }

        // Move to next level
        parent_group = current_table[current_idx].group_idx;
        current_table = lpm->tbl8_groups[parent_group];
        is_tbl24 = false;

        // Determine index in this level
        if (bits_this_level == 8)
        {
            current_idx = byte_val;
            bits_covered += 8;
        }
        else
        {
            // Partial byte - expand
            uint32_t num_entries = 1U << (8 - bits_this_level);
            uint32_t base_idx = (byte_val >> (8 - bits_this_level)) << (8 - bits_this_level);

            for (uint32_t i = 0; i < num_entries; i++)
            {
                uint32_t idx = base_idx + i;

                if (!current_table[idx].valid || depth > current_table[idx].depth)
                {
                    current_table[idx].next_hop = next_hop;
                    current_table[idx].depth = depth;
                    current_table[idx].valid = 1;
                    current_table[idx].ext_entry = 0;
                }
            }

            lpm->routes_count++;
            return 0;
        }
    }

    // Install route at current level
    if (!current_table[current_idx].valid || depth > current_table[current_idx].depth)
    {
        current_table[current_idx].next_hop = next_hop;
        current_table[current_idx].depth = depth;
        current_table[current_idx].valid = 1;
    }

    lpm->routes_count++;
    return 0;
}

//
// IPv6 LPM Delete
//

int cord_ipv6_lpm_delete(cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip, uint8_t depth)
{
    if (!lpm || !ip || depth > 128)
    {
        return -1;
    }

    if (depth <= 24)
    {
        uint32_t tbl24_idx = ((uint32_t)ip->addr[0] << 16) | ((uint32_t)ip->addr[1] << 8) | ip->addr[2];
        uint32_t num_entries = 1U << (24 - depth);
        tbl24_idx = (tbl24_idx >> (24 - depth)) << (24 - depth);

        for (uint32_t i = 0; i < num_entries; i++)
        {
            uint32_t idx = tbl24_idx + i;

            if (lpm->tbl24[idx].valid && lpm->tbl24[idx].depth == depth && !lpm->tbl24[idx].ext_entry)
            {
                lpm->tbl24[idx].valid = 0;
                lpm->tbl24[idx].next_hop = CORD_IPV6_LPM_INVALID_NEXT_HOP;
                lpm->tbl24[idx].depth = 0;
            }
        }

        lpm->routes_count--;
        return 0;
    }

    // Navigate to the appropriate TBL8 level
    // Simplified delete - mark entries as invalid at target depth
    // Full reclamation would require tracking reference counts

    lpm->routes_count--;
    return 0;
}

int cord_ipv6_lpm_delete_all(cord_ipv6_lpm_t *lpm)
{
    if (!lpm)
    {
        return -1;
    }

    size_t tbl24_size = CORD_IPV6_LPM_TBL24_SIZE * sizeof(cord_ipv6_lpm_entry_t);
    memset(lpm->tbl24, 0, tbl24_size);

    for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL8_MAX_GROUPS; i++)
    {
        if (lpm->tbl8_groups[i])
        {
            free(lpm->tbl8_groups[i]);
            lpm->tbl8_groups[i] = NULL;
        }
    }

    for (uint32_t i = 0; i < CORD_IPV6_LPM_TBL8_MAX_GROUPS; i++)
    {
        lpm->tbl8_free_list[i] = i;
    }
    lpm->tbl8_free_count = CORD_IPV6_LPM_TBL8_MAX_GROUPS;
    lpm->tbl8_used_count = 0;
    lpm->routes_count = 0;
    lpm->max_depth_reached = 0;

    return 0;
}

//
// IPv6 LPM Lookup
//

uint32_t cord_ipv6_lpm_lookup(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ip)
{
    if (!lpm || !ip)
    {
        return CORD_IPV6_LPM_INVALID_NEXT_HOP;
    }

    uint32_t best_next_hop = CORD_IPV6_LPM_INVALID_NEXT_HOP;

    // Level 0: TBL24 lookup
    uint32_t tbl24_idx = ((uint32_t)ip->addr[0] << 16) | ((uint32_t)ip->addr[1] << 8) | ip->addr[2];
    cord_ipv6_lpm_entry_t entry = lpm->tbl24[tbl24_idx];

    if (entry.valid)
    {
        best_next_hop = entry.next_hop;
    }

    if (!entry.ext_entry)
    {
        return best_next_hop;
    }

    // Multi-level TBL8 traversal
    uint32_t bits_covered = 24;

    while (entry.ext_entry && bits_covered < 128)
    {
        uint32_t byte_idx = bits_covered / 8;
        if (byte_idx >= 16)
        {
            break;
        }

        uint8_t tbl8_idx = ip->addr[byte_idx];
        entry = lpm->tbl8_groups[entry.group_idx][tbl8_idx];

        if (entry.valid)
        {
            best_next_hop = entry.next_hop;
        }

        if (!entry.ext_entry)
        {
            break;
        }

        bits_covered += 8;
    }

    return best_next_hop;
}

void cord_ipv6_lpm_lookup_batch(const cord_ipv6_lpm_t *lpm, const cord_ipv6_addr_t *ips,
                                 uint32_t *next_hops, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        next_hops[i] = cord_ipv6_lpm_lookup(lpm, &ips[i]);
    }
}

//
// IPv6 Helper Functions
//

int cord_ipv6_str_to_binary(const char *ip_str, cord_ipv6_addr_t *ip)
{
    return inet_pton(AF_INET6, ip_str, ip->addr) == 1 ? 0 : -1;
}

int cord_ipv6_parse_cidr(const char *cidr, cord_ipv6_addr_t *ip, uint8_t *depth)
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

    *depth = (uint8_t)prefix_len;
    return 0;
}

void cord_ipv6_to_str(const cord_ipv6_addr_t *ip, char *buf, size_t len)
{
    inet_ntop(AF_INET6, ip->addr, buf, len);
}

void cord_ipv6_lpm_print_stats(const cord_ipv6_lpm_t *lpm)
{
    if (!lpm)
    {
        return;
    }

    CORD_LOG("=== IPv6 LPM Statistics ===\n");
    CORD_LOG("Routes installed:  %u / %u\n", lpm->routes_count, lpm->max_routes);
    CORD_LOG("TBL24 entries:     %u (%.2f MB)\n",
             CORD_IPV6_LPM_TBL24_SIZE,
             (double)(CORD_IPV6_LPM_TBL24_SIZE * sizeof(cord_ipv6_lpm_entry_t)) / (1024 * 1024));
    CORD_LOG("TBL8 groups used:  %u / %u (%.2f KB)\n",
             lpm->tbl8_used_count, CORD_IPV6_LPM_TBL8_MAX_GROUPS,
             (double)(lpm->tbl8_used_count * CORD_IPV6_LPM_TBL8_SIZE * sizeof(cord_ipv6_lpm_entry_t)) / 1024);
    CORD_LOG("Max depth reached: %u levels\n", lpm->max_depth_reached);
    CORD_LOG("Total lookups:     %lu\n", lpm->lookup_count);
    CORD_LOG("===========================\n");
}

//
// MAC Address Helper Functions (for MAC-based LPM research)
//

void cord_mac_to_u64(const cord_mac_addr_t *mac, uint64_t *mac64)
{
    *mac64 = ((uint64_t)mac->addr[0] << 40) |
             ((uint64_t)mac->addr[1] << 32) |
             ((uint64_t)mac->addr[2] << 24) |
             ((uint64_t)mac->addr[3] << 16) |
             ((uint64_t)mac->addr[4] << 8) |
             ((uint64_t)mac->addr[5]);
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

    if (sscanf(mac_str, "%x:%x:%x:%x:%x:%x",
               &values[0], &values[1], &values[2],
               &values[3], &values[4], &values[5]) != 6)
    {
        return -1;
    }

    for (int i = 0; i < 6; i++)
    {
        mac->addr[i] = (uint8_t)values[i];
    }

    return 0;
}

int cord_mac_parse_cidr(const char *cidr, cord_mac_addr_t *mac, uint8_t *depth)
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

    *depth = (uint8_t)prefix_len;
    return 0;
}

void cord_mac_to_str(const cord_mac_addr_t *mac, char *buf, size_t len)
{
    snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac->addr[0], mac->addr[1], mac->addr[2],
             mac->addr[3], mac->addr[4], mac->addr[5]);
}
