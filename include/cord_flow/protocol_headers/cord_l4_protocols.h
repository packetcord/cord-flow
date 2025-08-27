#ifndef CORD_L4_PROTOCOLS_H
#define CORD_L4_PROTOCOLS_H

#include "cord_protocol_common.h"

// TCP header - RFC 793 compliant
typedef struct CORD_PACKED cord_tcp_hdr {
    uint16_t source;            // Source Port
    uint16_t dest;              // Destination Port
    uint32_t seq;               // Sequence Number
    uint32_t ack_seq;           // Acknowledgment Number
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t res1:4,             // Reserved
            doff:4;             // Data Offset
    uint8_t fin:1,              // FIN
            syn:1,              // SYN  
            rst:1,              // RST
            psh:1,              // PSH
            ack:1,              // ACK
            urg:1,              // URG
            ece:1,              // ECE
            cwr:1;              // CWR
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t doff:4,             // Data Offset
            res1:4;             // Reserved
    uint8_t cwr:1,              // CWR
            ece:1,              // ECE
            urg:1,              // URG
            ack:1,              // ACK
            psh:1,              // PSH
            rst:1,              // RST
            syn:1,              // SYN
            fin:1;              // FIN
#else
#error "Please define __BYTE_ORDER"
#endif
    uint16_t window;            // Window Size
    uint16_t check;             // Checksum
    uint16_t urg_ptr;           // Urgent Pointer
} cord_tcp_hdr_t;

// UDP header
typedef struct CORD_PACKED cord_udp_hdr {
    uint16_t source;            // Source Port
    uint16_t dest;              // Destination Port
    uint16_t len;               // Length
    uint16_t check;             // Checksum
} cord_udp_hdr_t;

// SCTP Common header
typedef struct CORD_PACKED cord_sctp_hdr {
    uint16_t source;            // Source Port
    uint16_t dest;              // Destination Port
    uint32_t vtag;              // Verification Tag
    uint32_t checksum;          // Checksum
} cord_sctp_hdr_t;

// SCTP Chunk header
typedef struct CORD_PACKED cord_sctp_chunk {
    uint8_t type;               // Chunk Type
    uint8_t flags;              // Chunk Flags
    uint16_t length;            // Chunk Length
    uint8_t data[];             // Variable length data
} cord_sctp_chunk_t;

// SCTP DATA chunk
typedef struct CORD_PACKED cord_sctp_data_chunk {
    uint8_t type;               // Chunk Type (0)
    uint8_t flags;              // Flags (U,B,E,reserved)
    uint16_t length;            // Chunk Length
    uint32_t tsn;               // Transmission Sequence Number
    uint16_t stream_id;         // Stream Identifier
    uint16_t stream_seq;        // Stream Sequence Number
    uint32_t ppid;              // Payload Protocol Identifier
    uint8_t user_data[];        // User Data
} cord_sctp_data_chunk_t;

// SCTP INIT chunk
typedef struct CORD_PACKED cord_sctp_init_chunk {
    uint8_t type;               // Chunk Type (1)
    uint8_t flags;              // Chunk Flags (reserved)
    uint16_t length;            // Chunk Length
    uint32_t init_tag;          // Initiate Tag
    uint32_t a_rwnd;            // Advertised Receiver Window Credit
    uint16_t num_outbound_streams;  // Number of Outbound Streams
    uint16_t num_inbound_streams;   // Number of Inbound Streams
    uint32_t initial_tsn;       // Initial TSN
    uint8_t variable_params[];  // Variable Length Parameters
} cord_sctp_init_chunk_t;

// SCTP SACK chunk
typedef struct CORD_PACKED cord_sctp_sack_chunk {
    uint8_t type;               // Chunk Type (3)
    uint8_t flags;              // Chunk Flags (reserved)
    uint16_t length;            // Chunk Length
    uint32_t cum_tsn_ack;       // Cumulative TSN Ack
    uint32_t a_rwnd;            // Advertised Receiver Window Credit
    uint16_t num_gap_ack_blocks;    // Number of Gap Ack Blocks
    uint16_t num_dup_tsns;      // Number of Duplicate TSNs
    // Followed by Gap Ack Blocks and Duplicate TSNs
} cord_sctp_sack_chunk_t;

// SCTP HEARTBEAT chunk
typedef struct CORD_PACKED cord_sctp_heartbeat_chunk {
    uint8_t type;               // Chunk Type (4)
    uint8_t flags;              // Chunk Flags (reserved)
    uint16_t length;            // Chunk Length
    uint8_t heartbeat_info[];   // Heartbeat Information
} cord_sctp_heartbeat_chunk_t;

// DCCP header
typedef struct CORD_PACKED cord_dccp_hdr {
    uint16_t source;            // Source Port
    uint16_t dest;              // Destination Port
    uint8_t dccph_doff;         // Data Offset
    uint8_t dccph_cscov;        // Checksum Coverage
    uint16_t dccph_checksum;    // Checksum
    uint8_t dccph_x_res_type_seq; // X:1, Reserved:2, Type:4, Sequence Number High:1
    uint16_t dccph_seq_low;     // Sequence Number Low
} cord_dccp_hdr_t;

// MPTCP option header
typedef struct CORD_PACKED cord_mptcp_opt {
    uint8_t kind;               // Option Kind (30)
    uint8_t len;                // Option Length
    uint8_t subtype_flags;      // Subtype:4, Flags:4
    uint8_t data[];             // Variable length data
} cord_mptcp_opt_t;

// Quick UDP Internet Connections (QUIC) header (simplified)
typedef struct CORD_PACKED cord_quic_hdr {
    uint8_t flags;              // Header Form, Fixed Bit, Packet Type, etc.
    uint32_t version;           // Version (long header only)
    uint8_t dcil_scil;          // Dest/Source Connection ID Lengths (long header)
    // Followed by variable-length Connection IDs, Packet Number, etc.
} cord_quic_hdr_t;

// TCP flags
#define CORD_TCP_FLAG_FIN       0x01
#define CORD_TCP_FLAG_SYN       0x02
#define CORD_TCP_FLAG_RST       0x04
#define CORD_TCP_FLAG_PSH       0x08
#define CORD_TCP_FLAG_ACK       0x10
#define CORD_TCP_FLAG_URG       0x20
#define CORD_TCP_FLAG_ECE       0x40
#define CORD_TCP_FLAG_CWR       0x80
#define CORD_TCP_FLAG_NS        0x100

// TCP option kinds
#define CORD_TCPOPT_EOL         0       // End of Option List
#define CORD_TCPOPT_NOP         1       // No-Operation
#define CORD_TCPOPT_MSS         2       // Maximum Segment Size
#define CORD_TCPOPT_WSCALE      3       // Window Scale
#define CORD_TCPOPT_SACK_PERM   4       // SACK Permitted
#define CORD_TCPOPT_SACK        5       // SACK
#define CORD_TCPOPT_TIMESTAMP   8       // Timestamp
#define CORD_TCPOPT_MD5SIG      19      // MD5 Signature
#define CORD_TCPOPT_FASTOPEN    34      // Fast Open
#define CORD_TCPOPT_MPTCP       30      // Multipath TCP

// SCTP chunk types
#define CORD_SCTP_CID_DATA      0
#define CORD_SCTP_CID_INIT      1
#define CORD_SCTP_CID_INIT_ACK  2
#define CORD_SCTP_CID_SACK      3
#define CORD_SCTP_CID_HEARTBEAT 4
#define CORD_SCTP_CID_HEARTBEAT_ACK 5
#define CORD_SCTP_CID_ABORT     6
#define CORD_SCTP_CID_SHUTDOWN  7
#define CORD_SCTP_CID_SHUTDOWN_ACK 8
#define CORD_SCTP_CID_ERROR     9
#define CORD_SCTP_CID_COOKIE_ECHO 10
#define CORD_SCTP_CID_COOKIE_ACK 11
#define CORD_SCTP_CID_SHUTDOWN_COMPLETE 14

// SCTP data chunk flags
#define CORD_SCTP_DATA_FLAG_E   0x01    // End of message
#define CORD_SCTP_DATA_FLAG_B   0x02    // Beginning of message
#define CORD_SCTP_DATA_FLAG_U   0x04    // Unordered delivery
#define CORD_SCTP_DATA_FLAG_I   0x08    // Immediate SACK

// DCCP packet types
#define CORD_DCCP_PKT_REQUEST   0
#define CORD_DCCP_PKT_RESPONSE  1
#define CORD_DCCP_PKT_DATA      2
#define CORD_DCCP_PKT_ACK       3
#define CORD_DCCP_PKT_DATAACK   4
#define CORD_DCCP_PKT_CLOSEREQ  5
#define CORD_DCCP_PKT_CLOSE     6
#define CORD_DCCP_PKT_RESET     7
#define CORD_DCCP_PKT_SYNC      8
#define CORD_DCCP_PKT_SYNCACK   9

// MPTCP subtypes
#define CORD_MPTCP_SUB_CAPABLE  0
#define CORD_MPTCP_SUB_JOIN     1
#define CORD_MPTCP_SUB_DSS      2
#define CORD_MPTCP_SUB_ADD_ADDR 3
#define CORD_MPTCP_SUB_REMOVE_ADDR 4
#define CORD_MPTCP_SUB_PRIO     5
#define CORD_MPTCP_SUB_FAIL     6
#define CORD_MPTCP_SUB_FASTCLOSE 7

// Helper macros for TCP
#define CORD_TCP_GET_DOFF(tcph)     ((tcph)->doff)
#define CORD_TCP_GET_FIN(tcph)      ((tcph)->fin)
#define CORD_TCP_GET_SYN(tcph)      ((tcph)->syn)
#define CORD_TCP_GET_RST(tcph)      ((tcph)->rst)
#define CORD_TCP_GET_PSH(tcph)      ((tcph)->psh)
#define CORD_TCP_GET_ACK(tcph)      ((tcph)->ack)
#define CORD_TCP_GET_URG(tcph)      ((tcph)->urg)
#define CORD_TCP_GET_ECE(tcph)      ((tcph)->ece)
#define CORD_TCP_GET_CWR(tcph)      ((tcph)->cwr)
#define CORD_TCP_SET_DOFF(tcph, doff_val) ((tcph)->doff = (doff_val))
#define CORD_TCP_HEADER_LEN(tcph)   ((tcph)->doff * 4)

// Helper macros for DCCP
#define CORD_DCCP_GET_X(dccph)      (((dccph)->dccph_x_res_type_seq & 0x80) >> 7)
#define CORD_DCCP_GET_TYPE(dccph)   (((dccph)->dccph_x_res_type_seq & 0x78) >> 3)
#define CORD_DCCP_GET_SEQ_HIGH(dccph) ((dccph)->dccph_x_res_type_seq & 0x01)

// Common port numbers
#define CORD_PORT_FTP_DATA      20
#define CORD_PORT_FTP_CONTROL   21
#define CORD_PORT_SSH           22
#define CORD_PORT_TELNET        23
#define CORD_PORT_SMTP          25
#define CORD_PORT_DNS           53
#define CORD_PORT_DHCP_SERVER   67
#define CORD_PORT_DHCP_CLIENT   68
#define CORD_PORT_HTTP          80
#define CORD_PORT_POP3          110
#define CORD_PORT_NTP           123
#define CORD_PORT_IMAP          143
#define CORD_PORT_SNMP          161
#define CORD_PORT_SNMP_TRAP     162
#define CORD_PORT_HTTPS         443
#define CORD_PORT_SYSLOG        514

#endif // CORD_L4_PROTOCOLS_H