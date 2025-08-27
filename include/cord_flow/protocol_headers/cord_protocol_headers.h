#ifndef CORD_PROTOCOL_HEADERS_H
#define CORD_PROTOCOL_HEADERS_H

/**
 * @file cord_protocol_headers.h
 * @brief Comprehensive portable protocol header definitions for CORD-FLOW
 * 
 * This header provides portable protocol definitions that work across different
 * platforms and architectures, from high-end servers to embedded microcontrollers.
 * 
 * Features:
 * - Platform-independent byte order handling
 * - Portable data types (no OS-specific includes)
 * - Comprehensive protocol coverage (L2-L7)
 * - Optimized for embedded and constrained environments
 * - Support for specialized protocols (VoIP, 3GPP, routing)
 * 
 * Usage example:
 * @code
 * #include <cord_flow/protocol_headers/cord_protocol_headers.h>
 * 
 * // Create and populate an IPv4 header
 * cord_ipv4_hdr_t ip_hdr;
 * ip_hdr.version_ihl = CORD_IPV4_SET_VER_IHL(4, 5);
 * ip_hdr.protocol = CORD_IPPROTO_UDP;
 * ip_hdr.saddr.addr = cord_htonl(0xC0A80001); // 192.168.0.1
 * ip_hdr.daddr.addr = cord_htonl(0xC0A80002); // 192.168.0.2
 * 
 * // Extract UDP header from packet
 * cord_udp_hdr_t *udp = (cord_udp_hdr_t*)((uint8_t*)&ip_hdr + (CORD_IPV4_GET_IHL(&ip_hdr) * 4));
 * uint16_t src_port = cord_ntohs(udp->source);
 * @endcode
 */

// Core common definitions and portable types
#include "cord_protocol_common.h"

// Layer 2 protocols
#include "cord_l2_protocols.h"

// Layer 3 protocols  
#include "cord_l3_protocols.h"

// Layer 4 protocols
#include "cord_l4_protocols.h"

// Tunneling and encapsulation protocols
#include "cord_tunneling_protocols.h"

// Routing protocols
#include "cord_routing_protocols.h"

// Voice over IP protocols
#include "cord_voip_protocols.h"

// 3GPP mobile protocols
#include "cord_3gpp_protocols.h"

#endif // CORD_PROTOCOL_HEADERS_H