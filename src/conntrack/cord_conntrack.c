#include <conntrack/cord_conntrack.h>
#include <match/cord_match.h>
#include <cord_error.h>
#include <cord_retval.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
 * @file cord_conntrack.c
 * @brief High-performance connection tracking implementation for CORD-FLOW
 * 
 * Provides zero-copy connection tracking with IOV-based frame buffering
 * and hash-based connection lookup for stateful packet processing.
 */

// =============================================================================
// PRIVATE HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Get current timestamp in microseconds
 */
static uint64_t cord_conntrack_get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

/**
 * @brief Compute 5-tuple hash for connection tracking
 */
static uint64_t cord_conntrack_compute_hash(uint32_t src_ip, uint32_t dst_ip,
                                            uint16_t src_port, uint16_t dst_port,
                                            uint8_t protocol)
{
    // Simple but effective hash function for network flows
    uint64_t hash = 0x1234567890ABCDEF;
    hash ^= ((uint64_t)src_ip << 32) | dst_ip;
    hash ^= ((uint64_t)src_port << 16) | dst_port;
    hash ^= (uint64_t)protocol;
    
    // Mix bits for better distribution
    hash ^= (hash >> 32);
    hash ^= (hash >> 16);
    hash ^= (hash >> 8);
    
    return hash;
}

/**
 * @brief Initialize frame buffers for a connection
 */
static cord_retval_t cord_conntrack_init_frame_buffers(cord_conntrack_connection_t* conn,
                                                       uint32_t max_frames,
                                                       uint32_t frame_size)
{
    // Allocate IOV array
    conn->frame_buffer = cord_cache_aligned_malloc(max_frames * sizeof(struct iovec));
    if (conn->frame_buffer == NULL) {
        return CORD_ERR_NO_MEMORY;
    }
    
    // Initialize each frame buffer
    for (uint32_t i = 0; i < max_frames; i++) {
        conn->frame_buffer[i].iov_base = cord_cache_aligned_malloc(frame_size);
        if (conn->frame_buffer[i].iov_base == NULL) {
            // Cleanup previously allocated buffers
            for (uint32_t j = 0; j < i; j++) {
                cord_cache_aligned_free(conn->frame_buffer[j].iov_base);
            }
            cord_cache_aligned_free(conn->frame_buffer);
            conn->frame_buffer = NULL;
            return CORD_ERR_NO_MEMORY;
        }
        conn->frame_buffer[i].iov_len = 0;
    }
    
    conn->frame_count = max_frames;
    conn->frame_index = 0;
    
    return CORD_OK;
}

/**
 * @brief Clean up frame buffers for a connection
 */
static void cord_conntrack_cleanup_frame_buffers(cord_conntrack_connection_t* conn)
{
    if (conn->frame_buffer != NULL) {
        for (uint32_t i = 0; i < conn->frame_count; i++) {
            if (conn->frame_buffer[i].iov_base != NULL) {
                cord_cache_aligned_free(conn->frame_buffer[i].iov_base);
            }
        }
        cord_cache_aligned_free(conn->frame_buffer);
        conn->frame_buffer = NULL;
    }
    
    conn->frame_count = 0;
    conn->frame_index = 0;
}

// =============================================================================
// PUBLIC API IMPLEMENTATION
// =============================================================================

cord_retval_t cord_conntrack_init(cord_conntrack_tracker_t* tracker,
                                  uint32_t max_connections,
                                  uint32_t max_frames,
                                  uint32_t frame_size)
{
    if (tracker == NULL) {
        return CORD_ERR_INVALID;
    }
    
    // Validate limits
    if (max_connections > CORD_CONNTRACK_MAX_CONNECTIONS) {
        max_connections = CORD_CONNTRACK_MAX_CONNECTIONS;
    }
    if (max_frames > CORD_CONNTRACK_MAX_FRAMES) {
        max_frames = CORD_CONNTRACK_MAX_FRAMES;
    }
    if (frame_size == 0) {
        frame_size = CORD_CONNTRACK_FRAME_BUFFER_SIZE;
    }
    
    // Initialize tracker structure
    memset(tracker, 0, sizeof(cord_conntrack_tracker_t));
    tracker->max_connections = max_connections;
    tracker->max_frames_per_conn = max_frames;
    tracker->frame_buffer_size = frame_size;
    
    // Initialize all connections as idle
    for (uint32_t i = 0; i < max_connections; i++) {
        tracker->connections[i].state = CORD_CONNTRACK_STATE_IDLE;
        tracker->connections[i].locked = false;
    }
    
    return CORD_OK;
}

void cord_conntrack_cleanup(cord_conntrack_tracker_t* tracker)
{
    if (tracker == NULL) {
        return;
    }
    
    // Clean up all active connections
    for (uint32_t i = 0; i < tracker->max_connections; i++) {
        if (tracker->connections[i].state != CORD_CONNTRACK_STATE_IDLE) {
            cord_conntrack_cleanup_frame_buffers(&tracker->connections[i]);
        }
    }
    
    // Reset tracker
    memset(tracker, 0, sizeof(cord_conntrack_tracker_t));
}

cord_retval_t cord_conntrack_extract_flow_key(const cord_eth_hdr_t* eth_hdr,
                                              size_t buffer_len,
                                              cord_conntrack_flow_key_t* flow_key)
{
    if (eth_hdr == NULL || flow_key == NULL) {
        return CORD_ERR_INVALID;
    }
    
    // Initialize flow key
    memset(flow_key, 0, sizeof(cord_conntrack_flow_key_t));
    
    // Check Ethernet type
    uint16_t eth_type = cord_get_eth_type(eth_hdr);
    if (eth_type != CORD_ETH_P_IP) {
        // Only IPv4 supported for now
        return CORD_ERR_UNSUPPORTED;
    }
    
    // Get IPv4 header
    const cord_ipv4_hdr_t* ip_hdr = cord_get_ipv4_hdr_from_eth(eth_hdr);
    if (ip_hdr == NULL) {
        return CORD_ERR_INVALID;
    }
    
    // Validate packet length
    if (buffer_len < sizeof(cord_eth_hdr_t) + sizeof(cord_ipv4_hdr_t)) {
        return CORD_ERR_INVALID;
    }
    
    // Extract IPv4 addresses and protocol
    flow_key->src_ip = cord_get_ipv4_src_addr_ntohl(ip_hdr);
    flow_key->dst_ip = cord_get_ipv4_dst_addr_ntohl(ip_hdr);
    flow_key->protocol = cord_get_ipv4_protocol(ip_hdr);
    
    // Extract port numbers based on protocol
    switch (flow_key->protocol) {
        case CORD_IPPROTO_TCP: {
            const cord_tcp_hdr_t* tcp_hdr = cord_get_tcp_hdr_ipv4(ip_hdr);
            if (tcp_hdr != NULL) {
                flow_key->src_port = cord_get_tcp_src_port(tcp_hdr);
                flow_key->dst_port = cord_get_tcp_dst_port(tcp_hdr);
            }
            break;
        }
        case CORD_IPPROTO_UDP: {
            const cord_udp_hdr_t* udp_hdr = cord_get_udp_hdr_ipv4(ip_hdr);
            if (udp_hdr != NULL) {
                flow_key->src_port = cord_get_udp_src_port(udp_hdr);
                flow_key->dst_port = cord_get_udp_dst_port(udp_hdr);
            }
            break;
        }
        case CORD_IPPROTO_SCTP: {
            const cord_sctp_hdr_t* sctp_hdr = cord_get_sctp_hdr_ipv4(ip_hdr);
            if (sctp_hdr != NULL) {
                flow_key->src_port = cord_get_sctp_src_port(sctp_hdr);
                flow_key->dst_port = cord_get_sctp_dst_port(sctp_hdr);
            }
            break;
        }
        case CORD_IPPROTO_ICMP:
            // ICMP doesn't have ports, use type/code instead
            flow_key->src_port = 0;
            flow_key->dst_port = 0;
            break;
        default:
            // Unknown protocol
            flow_key->src_port = 0;
            flow_key->dst_port = 0;
            break;
    }
    
    // Compute flow hash
    flow_key->flow_hash = cord_conntrack_compute_hash(flow_key->src_ip,
                                                      flow_key->dst_ip,
                                                      flow_key->src_port,
                                                      flow_key->dst_port,
                                                      flow_key->protocol);
    
    return CORD_OK;
}

bool cord_conntrack_lookup_connection(const cord_conntrack_tracker_t* tracker,
                                      const cord_conntrack_flow_key_t* flow_key,
                                      uint32_t* connection_id)
{
    if (tracker == NULL || flow_key == NULL || connection_id == NULL) {
        return false;
    }
    
    // Linear search through active connections
    for (uint32_t i = 0; i < tracker->max_connections; i++) {
        const cord_conntrack_connection_t* conn = &tracker->connections[i];
        
        if (conn->state != CORD_CONNTRACK_STATE_IDLE &&
            conn->flow_key.flow_hash == flow_key->flow_hash) {
            // Verify complete 5-tuple match to handle hash collisions
            if (conn->flow_key.src_ip == flow_key->src_ip &&
                conn->flow_key.dst_ip == flow_key->dst_ip &&
                conn->flow_key.src_port == flow_key->src_port &&
                conn->flow_key.dst_port == flow_key->dst_port &&
                conn->flow_key.protocol == flow_key->protocol) {
                *connection_id = i;
                return true;
            }
        }
    }
    
    return false;
}

bool cord_conntrack_is_connection_oriented(uint8_t protocol)
{
    switch (protocol) {
        case CORD_IPPROTO_TCP:
        case CORD_IPPROTO_SCTP:
            return true;
        case CORD_IPPROTO_UDP:
        case CORD_IPPROTO_ICMP:
        default:
            return false;
    }
}

cord_retval_t cord_conntrack_process_packet(cord_conntrack_tracker_t* tracker,
                                            const uint8_t* buffer,
                                            size_t buffer_len,
                                            uint32_t* connection_id)
{
    if (tracker == NULL || buffer == NULL || connection_id == NULL) {
        return CORD_ERR_INVALID;
    }
    
    // Extract flow key from packet
    cord_conntrack_flow_key_t flow_key;
    cord_retval_t result = cord_conntrack_extract_flow_key((const cord_eth_hdr_t*)buffer,
                                                           buffer_len,
                                                           &flow_key);
    if (result != CORD_OK) {
        return result;
    }
    
    // Look for existing connection
    if (cord_conntrack_lookup_connection(tracker, &flow_key, connection_id)) {
        // Update existing connection
        cord_conntrack_connection_t* conn = &tracker->connections[*connection_id];
        
        // Thread-safe update
        while (__sync_val_compare_and_swap(&conn->locked, false, true)) {
            // Spin until lock acquired
        }
        
        conn->last_activity = cord_conntrack_get_timestamp();
        conn->packet_count++;
        conn->bytes_total += buffer_len;
        
        // Store packet in frame buffer (zero-copy)
        uint32_t frame_idx = conn->frame_index;
        if (buffer_len <= tracker->frame_buffer_size) {
            memcpy(conn->frame_buffer[frame_idx].iov_base, buffer, buffer_len);
            conn->frame_buffer[frame_idx].iov_len = buffer_len;
            conn->frame_index = (conn->frame_index + 1) % conn->frame_count;
        }
        
        conn->locked = false;
        
        // Update tracker statistics
        tracker->total_packets++;
        tracker->total_bytes += buffer_len;
        
        return CORD_OK;
    }
    
    // Create new connection
    uint32_t new_conn_id = tracker->connection_index;
    cord_conntrack_connection_t* new_conn = &tracker->connections[new_conn_id];
    
    // Clean up existing connection if slot is occupied
    if (new_conn->state != CORD_CONNTRACK_STATE_IDLE) {
        cord_conntrack_cleanup_frame_buffers(new_conn);
        tracker->connections_dropped++;
    }
    
    // Initialize new connection
    memset(new_conn, 0, sizeof(cord_conntrack_connection_t));
    new_conn->flow_key = flow_key;
    new_conn->state = CORD_CONNTRACK_STATE_ACTIVE;
    new_conn->connection_oriented = cord_conntrack_is_connection_oriented(flow_key.protocol);
    new_conn->creation_time = cord_conntrack_get_timestamp();
    new_conn->last_activity = new_conn->creation_time;
    new_conn->packet_count = 1;
    new_conn->bytes_total = buffer_len;
    
    // Set protocol type
    switch (flow_key.protocol) {
        case CORD_IPPROTO_TCP:
            new_conn->protocol_type = CORD_CONNTRACK_PROTO_TCP;
            break;
        case CORD_IPPROTO_UDP:
            new_conn->protocol_type = CORD_CONNTRACK_PROTO_UDP;
            break;
        case CORD_IPPROTO_SCTP:
            new_conn->protocol_type = CORD_CONNTRACK_PROTO_SCTP;
            break;
        case CORD_IPPROTO_ICMP:
            new_conn->protocol_type = CORD_CONNTRACK_PROTO_ICMP;
            break;
        default:
            new_conn->protocol_type = CORD_CONNTRACK_PROTO_UNKNOWN;
            break;
    }
    
    // Initialize frame buffers
    result = cord_conntrack_init_frame_buffers(new_conn,
                                               tracker->max_frames_per_conn,
                                               tracker->frame_buffer_size);
    if (result != CORD_OK) {
        new_conn->state = CORD_CONNTRACK_STATE_IDLE;
        return result;
    }
    
    // Store first packet
    if (buffer_len <= tracker->frame_buffer_size) {
        memcpy(new_conn->frame_buffer[0].iov_base, buffer, buffer_len);
        new_conn->frame_buffer[0].iov_len = buffer_len;
        new_conn->frame_index = 1;
    }
    
    // Update tracker state
    *connection_id = new_conn_id;
    tracker->connection_index = (tracker->connection_index + 1) % tracker->max_connections;
    if (tracker->active_connections < tracker->max_connections) {
        tracker->active_connections++;
    }
    tracker->total_packets++;
    tracker->total_bytes += buffer_len;
    
    return CORD_OK;
}

const cord_conntrack_connection_t* cord_conntrack_get_connection(
    const cord_conntrack_tracker_t* tracker,
    uint32_t connection_id)
{
    if (tracker == NULL || connection_id >= tracker->max_connections) {
        return NULL;
    }
    
    const cord_conntrack_connection_t* conn = &tracker->connections[connection_id];
    if (conn->state == CORD_CONNTRACK_STATE_IDLE) {
        return NULL;
    }
    
    return conn;
}

cord_retval_t cord_conntrack_close_connection(cord_conntrack_tracker_t* tracker,
                                              uint32_t connection_id)
{
    if (tracker == NULL || connection_id >= tracker->max_connections) {
        return CORD_ERR_INVALID;
    }
    
    cord_conntrack_connection_t* conn = &tracker->connections[connection_id];
    if (conn->state == CORD_CONNTRACK_STATE_IDLE) {
        return CORD_ERR_NOT_FOUND;
    }
    
    // Clean up frame buffers
    cord_conntrack_cleanup_frame_buffers(conn);
    
    // Reset connection state
    conn->state = CORD_CONNTRACK_STATE_IDLE;
    if (tracker->active_connections > 0) {
        tracker->active_connections--;
    }
    
    return CORD_OK;
}

void cord_conntrack_get_stats(const cord_conntrack_tracker_t* tracker,
                              uint32_t* active_connections,
                              uint64_t* total_packets,
                              uint64_t* total_bytes)
{
    if (tracker == NULL) {
        return;
    }
    
    if (active_connections != NULL) {
        *active_connections = tracker->active_connections;
    }
    if (total_packets != NULL) {
        *total_packets = tracker->total_packets;
    }
    if (total_bytes != NULL) {
        *total_bytes = tracker->total_bytes;
    }
}

void cord_conntrack_debug_print(const cord_conntrack_tracker_t* tracker)
{
    if (tracker == NULL) {
        return;
    }
    
#if (CORD_CONNTRACK_DEBUG_ENABLED == 1)
    printf("=== CORD Connection Tracker Debug ===\n");
    printf("Active connections: %u/%u\n", tracker->active_connections, tracker->max_connections);
    printf("Total packets: %lu\n", tracker->total_packets);
    printf("Total bytes: %lu\n", tracker->total_bytes);
    printf("Hash collisions: %u\n", tracker->hash_collisions);
    printf("Connections dropped: %u\n", tracker->connections_dropped);
    
    for (uint32_t i = 0; i < tracker->max_connections; i++) {
        const cord_conntrack_connection_t* conn = &tracker->connections[i];
        if (conn->state != CORD_CONNTRACK_STATE_IDLE) {
            printf("Connection[%u]: %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u proto=%u packets=%u\n",
                   i,
                   (conn->flow_key.src_ip >> 24) & 0xFF,
                   (conn->flow_key.src_ip >> 16) & 0xFF,
                   (conn->flow_key.src_ip >> 8) & 0xFF,
                   conn->flow_key.src_ip & 0xFF,
                   conn->flow_key.src_port,
                   (conn->flow_key.dst_ip >> 24) & 0xFF,
                   (conn->flow_key.dst_ip >> 16) & 0xFF,
                   (conn->flow_key.dst_ip >> 8) & 0xFF,
                   conn->flow_key.dst_ip & 0xFF,
                   conn->flow_key.dst_port,
                   conn->flow_key.protocol,
                   conn->packet_count);
        }
    }
    printf("=====================================\n");
#endif
}

// =============================================================================
// HASH FUNCTIONS FOR CONNECTION TRACKING (Ported from paraaccel)
// =============================================================================

uint64_t cord_conntrack_hash_ipv4_tcp(const cord_ipv4_hdr_t* ipv4_header, 
                                      const cord_tcp_hdr_t* tcp_header)
{
    if (ipv4_header == NULL || tcp_header == NULL) {
        return 0;
    }
    
    uint64_t hash = 0;
    // Use raw source port and source IP (network byte order)
    hash |= ((uint64_t)cord_get_tcp_src_port(tcp_header) << 32);
    hash |= cord_get_ipv4_src_addr_l3(ipv4_header);
    
#if (CORD_CONNTRACK_LOG_ENABLED == 1)
    uint32_t src_ip = cord_get_ipv4_src_addr_ntohl(ipv4_header);
    printf("Hash... IPv4: %u.%u.%u.%u | TCP src port: %u | Hash: %lu\n", 
           (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF, 
           (src_ip >> 8) & 0xFF, src_ip & 0xFF,
           cord_get_tcp_src_port(tcp_header),
           hash);
#endif
    
    return hash;
}

uint64_t cord_conntrack_hash_ipv4_udp(const cord_ipv4_hdr_t* ipv4_header, 
                                      const cord_udp_hdr_t* udp_header)
{
    if (ipv4_header == NULL || udp_header == NULL) {
        return 0;
    }
    
    uint64_t hash = 0;
    // Use raw source port and source IP (network byte order)
    hash |= ((uint64_t)cord_get_udp_src_port(udp_header) << 32);
    hash |= cord_get_ipv4_src_addr_l3(ipv4_header);
    
#if (CORD_CONNTRACK_LOG_ENABLED == 1)
    uint32_t src_ip = cord_get_ipv4_src_addr_ntohl(ipv4_header);
    printf("Hash... IPv4: %u.%u.%u.%u | UDP src port: %u | Hash: %lu\n", 
           (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF, 
           (src_ip >> 8) & 0xFF, src_ip & 0xFF,
           cord_get_udp_src_port(udp_header),
           hash);
#endif
    
    return hash;
}

uint64_t cord_conntrack_hash_ipv6_tcp(const cord_ipv6_hdr_t* ipv6_header, 
                                      const cord_tcp_hdr_t* tcp_header)
{
    if (ipv6_header == NULL || tcp_header == NULL) {
        return 0;
    }
    
    // Get IPv6 source address
    cord_ipv6_addr_t src_addr;
    cord_get_ipv6_src_addr(ipv6_header, &src_addr);
    
    // Extract last 48 bits of IPv6 source address
    uint64_t ipv6_src_last_48_bits = (*(uint64_t*)(&src_addr.addr[8])) & 0x0000FFFFFFFFFFFF;
    uint64_t hash = ((uint64_t)cord_get_tcp_src_port(tcp_header) << 48);
    hash |= ipv6_src_last_48_bits;
    
#if (CORD_CONNTRACK_LOG_ENABLED == 1)
    printf("Hash... IPv6 | TCP src port: %u | Hash: %lu\n", 
           cord_get_tcp_src_port(tcp_header), hash);
#endif
    
    return hash;
}

uint64_t cord_conntrack_hash_ipv6_udp(const cord_ipv6_hdr_t* ipv6_header, 
                                      const cord_udp_hdr_t* udp_header)
{
    if (ipv6_header == NULL || udp_header == NULL) {
        return 0;
    }
    
    // Get IPv6 source address
    cord_ipv6_addr_t src_addr;
    cord_get_ipv6_src_addr(ipv6_header, &src_addr);
    
    // Extract last 48 bits of IPv6 source address  
    uint64_t ipv6_src_last_48_bits = (*(uint64_t*)(&src_addr.addr[8])) & 0x0000FFFFFFFFFFFF;
    uint64_t hash = ((uint64_t)cord_get_udp_src_port(udp_header) << 48);
    hash |= ipv6_src_last_48_bits;
    
#if (CORD_CONNTRACK_LOG_ENABLED == 1)
    printf("Hash... IPv6 | UDP src port: %u | Hash: %lu\n", 
           cord_get_udp_src_port(udp_header), hash);
#endif
    
    return hash;
}

uint64_t cord_conntrack_hash_ipv4_sctp(const cord_ipv4_hdr_t* ipv4_header, 
                                       const cord_sctp_hdr_t* sctp_header)
{
    if (ipv4_header == NULL || sctp_header == NULL) {
        return 0;
    }
    
    uint64_t hash = 0;
    // Use raw source port and source IP (network byte order)
    hash |= ((uint64_t)cord_get_sctp_src_port(sctp_header) << 32);
    hash |= cord_get_ipv4_src_addr_l3(ipv4_header);
    
#if (CORD_CONNTRACK_LOG_ENABLED == 1)
    uint32_t src_ip = cord_get_ipv4_src_addr_ntohl(ipv4_header);
    printf("Hash... IPv4: %u.%u.%u.%u | SCTP src port: %u | Hash: %lu\n", 
           (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF, 
           (src_ip >> 8) & 0xFF, src_ip & 0xFF,
           cord_get_sctp_src_port(sctp_header),
           hash);
#endif
    
    return hash;
}