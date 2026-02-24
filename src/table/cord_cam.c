#include <table/cord_cam.h>
#include <table/cord_lpm.h>
#include <match/cord_match.h>
#include <action/cord_action.h>
#include <string.h>

//
// Layer 2 CAM Table Implementation (Exact Match: MAC + VLAN → Port)
//
// Hash table with separate chaining for collision resolution
// Typical use case: Ethernet switching, MAC address learning
//
// Note: MAC conversion functions (cord_mac_to_u64, etc.) are now in cord_lpm.h/c
//

//
// Internal Helper Functions
//

// Hash function: combines MAC address and VLAN ID
static inline uint32_t mac_hash(const cord_mac_addr_t *mac, uint16_t vlan_id, uint32_t num_buckets)
{
    uint64_t mac64;
    cord_mac_to_u64(mac, &mac64);

    // XOR MAC with VLAN to create hash input
    uint64_t hash = mac64 ^ ((uint64_t)vlan_id << 32);

    return (uint32_t)(hash % num_buckets);
}

// MAC address comparison
static inline bool mac_equal(const cord_mac_addr_t *a, const cord_mac_addr_t *b)
{
    return memcmp(a->addr, b->addr, 6) == 0;
}

//
// L2 CAM Create/Destroy
//

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

    // Free all entries in all buckets
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

//
// L2 CAM Add/Delete/Lookup
//

int cord_l2_cam_add(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint32_t port_id, uint16_t vlan_id)
{
    if (!cam || !mac)
    {
        return -1;
    }

    if (cam->num_entries >= cam->max_entries)
    {
        return -1; // Table full
    }

    uint32_t bucket_idx = mac_hash(mac, vlan_id, cam->num_buckets);

    // Check if entry already exists (update case)
    cord_l2_cam_entry_t *entry = cam->buckets[bucket_idx];
    while (entry)
    {
        if (entry->vlan_id == vlan_id && mac_equal(&entry->mac, mac))
        {
            // Update existing entry
            entry->port_id = port_id;
            return 0;
        }
        entry = entry->next;
    }

    // Create new entry
    entry = calloc(1, sizeof(cord_l2_cam_entry_t));
    if (!entry)
    {
        return -1;
    }

    memcpy(&entry->mac, mac, sizeof(cord_mac_addr_t));
    entry->port_id = port_id;
    entry->vlan_id = vlan_id;
    entry->valid = 1;

    // Insert at head of bucket (constant time)
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

    // Search for matching entry
    while (entry)
    {
        if (entry->vlan_id == vlan_id && mac_equal(&entry->mac, mac))
        {
            // Remove from linked list
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

    return -1; // Entry not found
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

//
// Convenience Functions for Packet Processing
//

int cord_l2_cam_add_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth,
                             uint32_t port_id, uint16_t vlan_id, bool use_src)
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

uint32_t cord_l2_cam_lookup_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth,
                                     uint16_t vlan_id, bool use_dst)
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

uint32_t cord_l2_cam_lookup_from_vlan(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth,
                                      const cord_vlan_hdr_t *vlan, bool use_dst)
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

//
// Management Functions
//

void cord_l2_cam_clear(cord_l2_cam_t *cam)
{
    if (!cam)
    {
        return;
    }

    // Free all entries
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

//
// Statistics and Debugging
//

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
        CORD_LOG("Hit rate:         %.2f%%\n",
                 (double)cam->hit_count * 100.0 / cam->lookup_count);
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
                CORD_LOG("%02x:%02x:%02x:%02x:%02x:%02x  %-8u %-8u\n",
                         entry->mac.addr[0], entry->mac.addr[1],
                         entry->mac.addr[2], entry->mac.addr[3],
                         entry->mac.addr[4], entry->mac.addr[5],
                         entry->vlan_id, entry->port_id);
                count++;
            }
            entry = entry->next;
        }
    }

    CORD_LOG("------------------------------------------------\n");
    CORD_LOG("Total: %u entries\n", count);
    CORD_LOG("======================\n");
}
