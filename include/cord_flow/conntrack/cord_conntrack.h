#ifndef CORD_CONNTRACK_H
#define CORD_CONNTRACK_H

#include <cord_retval.h>
#include <cord_type.h>
#include <memory/cord_memory.h>
#include <protocol_headers/cord_protocol_headers.h>
#include <stdatomic.h>
#include <sys/uio.h>

//
// Macros
//
#define CONNTRACK_RX_BUFFER_SIZE 1500 // Set to the maximum MTU size
#define CONNTRACK_FRAME_COUNT 10
#define CONNTRACK_MAX_CONNTRACK_SOURCES 16
#define CONNTRACK_FRAME_COUNT 10
#define CONNTRACK_FRAME_COEFFICIENT 3
#define CONNTRACK_FRAME_WINDOW (CONNTRACK_FRAME_COUNT - CONNTRACK_FRAME_COEFFICIENT)

#define CONNTRACK_LOG_ENABLED (0)
#define CONNTRACK_TABLE_WALK_LOG_ENABLED (0)
#define CONNTRACK_CONCATENATED_PAYLOAD_LOG_ENABLED \
    (0) // This may crash the program, since it expects ASCII characters/strings \
        // as paylaod (use only for isolated tests)

//
// Conntrack
//
typedef struct cord_connection_t
{
    atomic_bool locked;
    uint8_t connection_oriented;
    uint64_t connection_hash; // Hash ID of the (src IP, src PORT) pair
    uint32_t frame_index;     // Packet/frame index inside the IOV buffer
    struct iovec *iov;        // The IOV buffer
} cord_connection_t;

typedef struct cord_connection_tracker_t
{
    cord_connection_t sources[CONNTRACK_MAX_CONNTRACK_SOURCES]; // Array of all connections
    uint32_t sources_index;                                     // Connections index pointer
} cord_connection_tracker_t;

extern cord_connection_tracker_t connection_tracker_singleton;

void cord_init_conntrack(cord_connection_tracker_t *connections);
void cord_add_new_connection(cord_connection_tracker_t *connections, uint64_t current_hash, uint8_t *buffer,
                             int buf_len);
void cord_append_packet_to_connection(cord_connection_tracker_t *connections, uint32_t index, uint8_t *buffer,
                                      int buf_len);

//
// Hash
//
uint64_t cord_ipv4_tcp_connection_hash(cord_ipv4_hdr_t *ipv4_header, cord_tcp_hdr_t *tcp_header);
uint64_t cord_ipv4_udp_connection_hash(cord_ipv4_hdr_t *ipv4_header, cord_udp_hdr_t *udp_header);
uint64_t cord_ipv6_tcp_connection_hash(cord_ipv6_hdr_t *ipv6_header, cord_tcp_hdr_t *tcp_header);
uint64_t cord_ipv6_udp_connection_hash(cord_ipv6_hdr_t *ipv6_header, cord_udp_hdr_t *udp_header);
bool cord_source_hash_detected(cord_connection_tracker_t *connections, uint64_t current_hash,
                                  uint32_t *hash_found_index);
void cord_show_connection_hashes(cord_connection_tracker_t *connections, uint32_t elements_count);

//
// Arrangement
//
void cord_find_min(uint32_t *arr, size_t len, uint32_t *min_index, uint8_t *prev_min_arr);
void cord_asterisk_sort(uint32_t *arr, uint32_t **sorted_asterisk_arr, size_t len);
void cord_tcp_find_min(struct iovec *arr, size_t len, uint32_t *min_index, uint8_t *prev_min_arr);
void cord_tcp_asterisk_sort(struct iovec *arr, struct iovec **sorted_asterisk_arr, size_t len);
void cord_concatenate_packet_payload(uint8_t **concat_result, size_t *concat_len, uint8_t *current, size_t current_len);

#endif // CORD_CONNTRACK_H
