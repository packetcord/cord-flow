#ifndef CORD_CAM_H
#define CORD_CAM_H

#include <cord_type.h>
#include <protocol_headers/cord_protocol_headers.h>

//
// CORD CAM - Content Addressable Memory (Exact Match Tables)
//
// Layer 2 CAM for Ethernet switching: (MAC address, VLAN ID) → Port ID
// Uses hash table with chaining for collision resolution
//

//
// Layer 2 CAM Table (Exact Match: MAC + VLAN → Port)
//

#define CORD_L2_CAM_INVALID_PORT 0xFFFFFFFF

// L2 CAM entry structure
typedef struct cord_l2_cam_entry
{
    cord_mac_addr_t mac;                  // MAC address (48 bits)
    uint32_t port_id;                     // Output port/interface ID
    uint16_t vlan_id;                     // VLAN ID (0-4095)
    uint8_t valid;                        // Entry is valid
    uint8_t reserved;                     // Reserved for alignment
    struct cord_l2_cam_entry *next;       // Collision chain (linked list)
} cord_l2_cam_entry_t;

// L2 CAM table structure
typedef struct cord_l2_cam
{
    cord_l2_cam_entry_t **buckets;        // Hash buckets
    uint32_t num_buckets;                 // Number of hash buckets
    uint32_t num_entries;                 // Current number of entries
    uint32_t max_entries;                 // Maximum capacity
    uint64_t lookup_count;                // Total lookups performed
    uint64_t hit_count;                   // Successful lookups
    uint64_t miss_count;                  // Failed lookups
} cord_l2_cam_t;

//
// L2 CAM API
//

// Create and destroy
cord_l2_cam_t *cord_l2_cam_create(uint32_t num_buckets, uint32_t max_entries);
void cord_l2_cam_destroy(cord_l2_cam_t *cam);

// Basic operations
int cord_l2_cam_add(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint32_t port_id, uint16_t vlan_id);
int cord_l2_cam_delete(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint16_t vlan_id);
uint32_t cord_l2_cam_lookup(cord_l2_cam_t *cam, const cord_mac_addr_t *mac, uint16_t vlan_id);

// Convenience functions for packet processing
int cord_l2_cam_add_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth, uint32_t port_id,
                             uint16_t vlan_id, bool use_src);
uint32_t cord_l2_cam_lookup_from_eth(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth,
                                     uint16_t vlan_id, bool use_dst);
uint32_t cord_l2_cam_lookup_from_vlan(cord_l2_cam_t *cam, const cord_eth_hdr_t *eth,
                                      const cord_vlan_hdr_t *vlan, bool use_dst);

// Management
void cord_l2_cam_clear(cord_l2_cam_t *cam);

// Statistics and debugging
void cord_l2_cam_print_stats(const cord_l2_cam_t *cam);
void cord_l2_cam_print_entries(const cord_l2_cam_t *cam);

//
// Note: MAC Address Helper Functions
//
// MAC conversion functions (cord_mac_to_u64, cord_u64_to_mac, etc.)
// are now in <table/cord_lpm.h> for use in MAC-based LPM research

#endif // CORD_CAM_H
