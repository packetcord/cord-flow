#ifndef CORD_CONNTRACK_H
#define CORD_CONNTRACK_H

#include <cord_type.h>
#include <cord_retval.h>
#include <protocol_headers/cord_protocol_headers.h>
#include <memory/cord_memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/uio.h>

/**
 * @file cord_conntrack.h
 * @brief High-performance connection tracking for CORD-FLOW
 * 
 * This module provides zero-copy connection tracking capabilities for
 * stateful packet processing. Features include:
 * - Thread-safe connection state management
 * - Zero-copy packet buffering with IOV
 * - Connection-oriented and connectionless protocol support
 * - High-performance hash-based connection lookup
 * - Configurable connection limits and frame buffering
 */

// =============================================================================
// CONFIGURATION CONSTANTS
// =============================================================================

#define CORD_CONNTRACK_MAX_CONNECTIONS    256
#define CORD_CONNTRACK_MAX_FRAMES         32
#define CORD_CONNTRACK_FRAME_BUFFER_SIZE  2048

// Connection tracking flags
#define CORD_CONNTRACK_LOG_ENABLED        0
#define CORD_CONNTRACK_DEBUG_ENABLED      0

// =============================================================================
// CONNECTION TRACKING STRUCTURES
// =============================================================================

/**
 * @brief Connection state flags
 */
typedef enum {
    CORD_CONNTRACK_STATE_IDLE = 0,
    CORD_CONNTRACK_STATE_ACTIVE,
    CORD_CONNTRACK_STATE_CLOSING,
    CORD_CONNTRACK_STATE_CLOSED
} cord_conntrack_state_t;

/**
 * @brief Protocol type for connection tracking
 */
typedef enum {
    CORD_CONNTRACK_PROTO_UNKNOWN = 0,
    CORD_CONNTRACK_PROTO_TCP,
    CORD_CONNTRACK_PROTO_UDP,
    CORD_CONNTRACK_PROTO_SCTP,
    CORD_CONNTRACK_PROTO_ICMP
} cord_conntrack_protocol_t;

/**
 * @brief Connection hash information for fast lookup
 */
typedef struct {
    uint64_t flow_hash;      // 5-tuple hash (src_ip, dst_ip, src_port, dst_port, protocol)
    uint32_t src_ip;         // Source IP address (host byte order)
    uint32_t dst_ip;         // Destination IP address (host byte order) 
    uint16_t src_port;       // Source port (host byte order)
    uint16_t dst_port;       // Destination port (host byte order)
    uint8_t protocol;        // Protocol number
} cord_conntrack_flow_key_t;

/**
 * @brief Individual tracked connection
 */
typedef struct {
    // Connection identification
    cord_conntrack_flow_key_t flow_key;
    cord_conntrack_state_t state;
    cord_conntrack_protocol_t protocol_type;
    
    // Thread safety
    volatile bool locked;
    
    // Connection properties
    bool connection_oriented;  // TCP/SCTP vs UDP/ICMP
    uint64_t creation_time;    // Connection creation timestamp
    uint64_t last_activity;    // Last packet timestamp
    uint32_t packet_count;     // Total packets in this connection
    uint64_t bytes_total;      // Total bytes in this connection
    
    // Frame buffering (IOV-based zero-copy)
    struct iovec* frame_buffer;      // Array of frame buffers
    uint32_t frame_index;           // Current frame write index
    uint32_t frame_count;           // Number of frames allocated
    
    // Connection-specific data
    union {
        struct {
            uint32_t seq_num;       // TCP sequence number
            uint32_t ack_num;       // TCP acknowledgment number
            uint8_t tcp_state;      // TCP connection state
            uint16_t window_size;   // TCP window size
        } tcp;
        
        struct {
            uint32_t flow_label;    // UDP flow label
            uint16_t checksum;      // Last UDP checksum
        } udp;
        
        struct {
            uint32_t vtag;          // SCTP verification tag
            uint32_t tsn;           // SCTP transmission sequence number
        } sctp;
    } proto_data;
} cord_conntrack_connection_t;

/**
 * @brief Main connection tracker structure
 */
typedef struct {
    cord_conntrack_connection_t connections[CORD_CONNTRACK_MAX_CONNECTIONS];
    uint32_t connection_index;     // Round-robin allocation index
    uint32_t active_connections;   // Number of currently active connections
    uint64_t total_packets;        // Total packets processed
    uint64_t total_bytes;          // Total bytes processed
    
    // Configuration
    uint32_t max_connections;      // Maximum allowed connections
    uint32_t max_frames_per_conn;  // Maximum frames per connection
    uint32_t frame_buffer_size;    // Size of each frame buffer
    
    // Statistics
    uint32_t hash_collisions;      // Number of hash collisions
    uint32_t connections_dropped;  // Connections dropped due to limits
} cord_conntrack_tracker_t;

// =============================================================================
// CONNECTION TRACKING FUNCTIONS
// =============================================================================

/**
 * @brief Initialize a connection tracker
 * 
 * @param tracker Pointer to connection tracker structure
 * @param max_connections Maximum number of concurrent connections
 * @param max_frames Maximum frames per connection
 * @param frame_size Size of each frame buffer in bytes
 * @return CORD_OK on success, error code on failure
 */
cord_retval_t cord_conntrack_init(cord_conntrack_tracker_t* tracker,
                                  uint32_t max_connections,
                                  uint32_t max_frames,
                                  uint32_t frame_size);

/**
 * @brief Cleanup and free connection tracker resources
 * 
 * @param tracker Pointer to connection tracker structure
 */
void cord_conntrack_cleanup(cord_conntrack_tracker_t* tracker);

/**
 * @brief Process a packet and update connection tracking
 * 
 * Processes an incoming packet, either updating an existing connection
 * or creating a new connection entry. Implements zero-copy buffering.
 * 
 * @param tracker Pointer to connection tracker
 * @param buffer Pointer to packet buffer (including Ethernet header)
 * @param buffer_len Length of packet buffer
 * @param connection_id Output: connection ID for this packet
 * @return CORD_OK on success, error code on failure
 */
cord_retval_t cord_conntrack_process_packet(cord_conntrack_tracker_t* tracker,
                                            const uint8_t* buffer,
                                            size_t buffer_len,
                                            uint32_t* connection_id);

/**
 * @brief Look up existing connection by flow key
 * 
 * @param tracker Pointer to connection tracker
 * @param flow_key Flow key to search for
 * @param connection_id Output: connection ID if found
 * @return true if connection found, false otherwise
 */
bool cord_conntrack_lookup_connection(const cord_conntrack_tracker_t* tracker,
                                      const cord_conntrack_flow_key_t* flow_key,
                                      uint32_t* connection_id);

/**
 * @brief Get connection information by ID
 * 
 * @param tracker Pointer to connection tracker
 * @param connection_id Connection ID
 * @return Pointer to connection structure, or NULL if invalid ID
 */
const cord_conntrack_connection_t* cord_conntrack_get_connection(
    const cord_conntrack_tracker_t* tracker,
    uint32_t connection_id);

/**
 * @brief Close and clean up a specific connection
 * 
 * @param tracker Pointer to connection tracker
 * @param connection_id Connection ID to close
 * @return CORD_OK on success, error code on failure
 */
cord_retval_t cord_conntrack_close_connection(cord_conntrack_tracker_t* tracker,
                                              uint32_t connection_id);

/**
 * @brief Generate flow hash from packet headers
 * 
 * @param eth_hdr Ethernet header
 * @param buffer_len Total buffer length
 * @param flow_key Output: parsed flow key with hash
 * @return CORD_OK on success, error code on failure
 */
cord_retval_t cord_conntrack_extract_flow_key(const cord_eth_hdr_t* eth_hdr,
                                              size_t buffer_len,
                                              cord_conntrack_flow_key_t* flow_key);

/**
 * @brief Check if protocol is connection-oriented
 * 
 * @param protocol Protocol number (IPPROTO_*)
 * @return true if connection-oriented (TCP, SCTP), false otherwise
 */
bool cord_conntrack_is_connection_oriented(uint8_t protocol);

/**
 * @brief Get connection tracking statistics
 * 
 * @param tracker Pointer to connection tracker
 * @param active_connections Output: number of active connections
 * @param total_packets Output: total packets processed
 * @param total_bytes Output: total bytes processed
 */
void cord_conntrack_get_stats(const cord_conntrack_tracker_t* tracker,
                              uint32_t* active_connections,
                              uint64_t* total_packets,
                              uint64_t* total_bytes);

/**
 * @brief Print connection tracking debug information
 * 
 * @param tracker Pointer to connection tracker
 */
void cord_conntrack_debug_print(const cord_conntrack_tracker_t* tracker);

// =============================================================================
// HASH FUNCTIONS FOR CONNECTION TRACKING
// =============================================================================

/**
 * @brief Compute connection hash for IPv4 TCP packets
 * 
 * @param ipv4_header IPv4 header
 * @param tcp_header TCP header
 * @return 64-bit hash value
 */
uint64_t cord_conntrack_hash_ipv4_tcp(const cord_ipv4_hdr_t* ipv4_header, 
                                      const cord_tcp_hdr_t* tcp_header);

/**
 * @brief Compute connection hash for IPv4 UDP packets
 * 
 * @param ipv4_header IPv4 header
 * @param udp_header UDP header
 * @return 64-bit hash value
 */
uint64_t cord_conntrack_hash_ipv4_udp(const cord_ipv4_hdr_t* ipv4_header, 
                                      const cord_udp_hdr_t* udp_header);

/**
 * @brief Compute connection hash for IPv6 TCP packets
 * 
 * @param ipv6_header IPv6 header
 * @param tcp_header TCP header
 * @return 64-bit hash value
 */
uint64_t cord_conntrack_hash_ipv6_tcp(const cord_ipv6_hdr_t* ipv6_header, 
                                      const cord_tcp_hdr_t* tcp_header);

/**
 * @brief Compute connection hash for IPv6 UDP packets
 * 
 * @param ipv6_header IPv6 header
 * @param udp_header UDP header
 * @return 64-bit hash value
 */
uint64_t cord_conntrack_hash_ipv6_udp(const cord_ipv6_hdr_t* ipv6_header, 
                                      const cord_udp_hdr_t* udp_header);

/**
 * @brief Compute connection hash for SCTP packets
 * 
 * @param ipv4_header IPv4 header (IPv6 variant available if needed)
 * @param sctp_header SCTP header
 * @return 64-bit hash value
 */
uint64_t cord_conntrack_hash_ipv4_sctp(const cord_ipv4_hdr_t* ipv4_header, 
                                       const cord_sctp_hdr_t* sctp_header);

#endif // CORD_CONNTRACK_H
