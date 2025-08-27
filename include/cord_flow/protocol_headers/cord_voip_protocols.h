#ifndef CORD_VOIP_PROTOCOLS_H
#define CORD_VOIP_PROTOCOLS_H

#include "cord_protocol_common.h"

// RTP (Real-time Transport Protocol) header
typedef struct CORD_PACKED cord_rtp_hdr {
    uint8_t version_p_x_cc;     // Version:2, P:1, X:1, CC:4
    uint8_t m_pt;               // M:1, PT:7
    uint16_t sequence_number;   // Sequence Number
    uint32_t timestamp;         // Timestamp
    uint32_t ssrc;              // SSRC
    // Followed by optional CSRC list
} cord_rtp_hdr_t;

// RTP extension header
typedef struct CORD_PACKED cord_rtp_ext_hdr {
    uint16_t defined_by_profile; // Defined by Profile
    uint16_t length;            // Length
    uint32_t header_extension[]; // Header Extension
} cord_rtp_ext_hdr_t;

// RTCP (RTP Control Protocol) common header
typedef struct CORD_PACKED cord_rtcp_hdr {
    uint8_t version_p_rc;       // Version:2, P:1, RC:5
    uint8_t packet_type;        // Packet Type
    uint16_t length;            // Length
} cord_rtcp_hdr_t;

// RTCP Sender Report
typedef struct CORD_PACKED cord_rtcp_sr {
    cord_rtcp_hdr_t hdr;        // RTCP Header
    uint32_t ssrc;              // SSRC of sender
    uint32_t ntp_timestamp_msw; // NTP timestamp, most significant word
    uint32_t ntp_timestamp_lsw; // NTP timestamp, least significant word
    uint32_t rtp_timestamp;     // RTP timestamp
    uint32_t sender_packet_count; // Sender's packet count
    uint32_t sender_octet_count;  // Sender's octet count
    // Followed by reception report blocks
} cord_rtcp_sr_t;

// RTCP Receiver Report
typedef struct CORD_PACKED cord_rtcp_rr {
    cord_rtcp_hdr_t hdr;        // RTCP Header
    uint32_t ssrc;              // SSRC of source
    // Followed by reception report blocks
} cord_rtcp_rr_t;

// RTCP Reception Report Block
typedef struct CORD_PACKED cord_rtcp_rr_block {
    uint32_t ssrc;              // SSRC of source
    uint8_t fraction_lost;      // Fraction lost
    uint8_t cumulative_lost[3]; // Cumulative number of packets lost
    uint32_t extended_highest_seq; // Extended highest sequence number received
    uint32_t interarrival_jitter;  // Interarrival jitter
    uint32_t last_sr;           // Last SR timestamp
    uint32_t delay_since_last_sr;  // Delay since last SR
} cord_rtcp_rr_block_t;

// RTCP Source Description
typedef struct CORD_PACKED cord_rtcp_sdes {
    cord_rtcp_hdr_t hdr;        // RTCP Header
    // Followed by SDES chunks
} cord_rtcp_sdes_t;

// RTCP SDES Item
typedef struct CORD_PACKED cord_rtcp_sdes_item {
    uint8_t type;               // Item type
    uint8_t length;             // Item length
    uint8_t data[];             // Item data
} cord_rtcp_sdes_item_t;

// SIP (Session Initiation Protocol) - Text-based, but define some constants
#define CORD_SIP_METHOD_INVITE      "INVITE"
#define CORD_SIP_METHOD_ACK         "ACK"
#define CORD_SIP_METHOD_BYE         "BYE"
#define CORD_SIP_METHOD_CANCEL      "CANCEL"
#define CORD_SIP_METHOD_OPTIONS     "OPTIONS"
#define CORD_SIP_METHOD_REGISTER    "REGISTER"
#define CORD_SIP_METHOD_PRACK       "PRACK"
#define CORD_SIP_METHOD_SUBSCRIBE   "SUBSCRIBE"
#define CORD_SIP_METHOD_NOTIFY      "NOTIFY"
#define CORD_SIP_METHOD_PUBLISH     "PUBLISH"
#define CORD_SIP_METHOD_INFO        "INFO"
#define CORD_SIP_METHOD_REFER       "REFER"
#define CORD_SIP_METHOD_MESSAGE     "MESSAGE"
#define CORD_SIP_METHOD_UPDATE      "UPDATE"

// SDP (Session Description Protocol) - Text-based protocol
// Common SDP line types
#define CORD_SDP_VERSION            "v="
#define CORD_SDP_ORIGIN             "o="
#define CORD_SDP_SESSION_NAME       "s="
#define CORD_SDP_CONNECTION_INFO    "c="
#define CORD_SDP_TIME               "t="
#define CORD_SDP_MEDIA              "m="
#define CORD_SDP_ATTRIBUTE          "a="

// H.323 RAS (Registration, Admission, and Status) header
typedef struct CORD_PACKED cord_h323_ras_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator
    uint8_t message_type;       // Message Type
    uint16_t length;            // Length
    // Followed by ASN.1 encoded data
} cord_h323_ras_hdr_t;

// H.225.0 Call Signaling header
typedef struct CORD_PACKED cord_h225_hdr {
    uint8_t protocol_discriminator; // Protocol Discriminator (0x08)
    uint8_t call_reference_length;  // Call Reference Length
    // Variable-length call reference follows
    // uint8_t call_reference[call_reference_length]; 
    // uint8_t message_type;       // Message Type after call reference
    // Followed by Information Elements
} cord_h225_hdr_t;

// H.245 Control Protocol header
typedef struct CORD_PACKED cord_h245_hdr {
    uint8_t tpkt_version;       // TPKT Version (3)
    uint8_t tpkt_reserved;      // TPKT Reserved (0)
    uint16_t tpkt_length;       // TPKT Length
    // Followed by ASN.1 encoded H.245 message
} cord_h245_hdr_t;

// IAX2 (Inter-Asterisk eXchange version 2) header
typedef struct CORD_PACKED cord_iax2_hdr {
    uint16_t scallno_full;      // Source call number and full frame flag
    uint16_t dcallno_retrans;   // Dest call number and retransmission flag
    uint32_t timestamp;         // Timestamp
    uint8_t oseqno;             // Outbound sequence number
    uint8_t iseqno;             // Inbound sequence number
    uint8_t frametype;          // Frame type
    uint8_t csub;               // C-bit and subclass
} cord_iax2_hdr_t;

// IAX2 Mini frame header
typedef struct CORD_PACKED cord_iax2_mini_hdr {
    uint16_t callno;            // Call number (high bit = 0)
    uint16_t timestamp;         // Timestamp
} cord_iax2_mini_hdr_t;

// MGCP (Media Gateway Control Protocol) - Text-based
#define CORD_MGCP_CRCX              "CRCX"  // Create Connection
#define CORD_MGCP_MDCX              "MDCX"  // Modify Connection
#define CORD_MGCP_DLCX              "DLCX"  // Delete Connection
#define CORD_MGCP_RQNT              "RQNT"  // Request Notification
#define CORD_MGCP_NTFY              "NTFY"  // Notify
#define CORD_MGCP_AUEP              "AUEP"  // Audit Endpoint
#define CORD_MGCP_AUCX              "AUCX"  // Audit Connection
#define CORD_MGCP_RSIP              "RSIP"  // Restart In Progress

// MEGACO/H.248 - Text and binary encodings supported
typedef struct CORD_PACKED cord_megaco_hdr {
    uint32_t version;           // Version
    uint32_t mid;               // Media Gateway ID
    // Followed by transaction data
} cord_megaco_hdr_t;

// SCCP (Signalling Connection Control Part) header
typedef struct CORD_PACKED cord_sccp_hdr {
    uint8_t msg_type;           // Message Type
    // Variable format based on message type
} cord_sccp_hdr_t;

// TCAP (Transaction Capabilities Application Part) header
typedef struct CORD_PACKED cord_tcap_hdr {
    uint8_t tag;                // Tag
    uint8_t length;             // Length
    // Followed by TCAP components
} cord_tcap_hdr_t;

// ISUP (ISDN User Part) header
typedef struct CORD_PACKED cord_isup_hdr {
    uint8_t msg_type;           // Message Type
    // Variable format based on message type
} cord_isup_hdr_t;

// DIAMETER header
typedef struct CORD_PACKED cord_diameter_hdr {
    uint8_t version;            // Version
    uint8_t length[3];          // Message Length
    uint8_t flags;              // Flags
    uint8_t command_code[3];    // Command Code
    uint32_t application_id;    // Application ID
    uint32_t hop_by_hop_id;     // Hop-by-Hop Identifier
    uint32_t end_to_end_id;     // End-to-End Identifier
} cord_diameter_hdr_t;

// RADIUS header
typedef struct CORD_PACKED cord_radius_hdr {
    uint8_t code;               // Code
    uint8_t identifier;         // Identifier
    uint16_t length;            // Length
    uint8_t authenticator[16];  // Response Authenticator
} cord_radius_hdr_t;

// RTP payload types (common ones)
#define CORD_RTP_PT_PCMU            0   // ITU-T G.711 PCMU
#define CORD_RTP_PT_RESERVED1       1   // Reserved
#define CORD_RTP_PT_RESERVED2       2   // Reserved
#define CORD_RTP_PT_GSM             3   // GSM 06.10
#define CORD_RTP_PT_G723            4   // ITU-T G.723.1
#define CORD_RTP_PT_DVI4_8000       5   // DVI4 8000 Hz
#define CORD_RTP_PT_DVI4_16000      6   // DVI4 16000 Hz
#define CORD_RTP_PT_LPC             7   // LPC
#define CORD_RTP_PT_PCMA            8   // ITU-T G.711 PCMA
#define CORD_RTP_PT_G722            9   // ITU-T G.722
#define CORD_RTP_PT_L16_2CH         10  // L16 stereo
#define CORD_RTP_PT_L16_1CH         11  // L16 mono
#define CORD_RTP_PT_QCELP           12  // QCELP
#define CORD_RTP_PT_CN              13  // Comfort Noise
#define CORD_RTP_PT_MPA             14  // MPEG Audio
#define CORD_RTP_PT_G728            15  // ITU-T G.728
#define CORD_RTP_PT_DVI4_11025      16  // DVI4 11025 Hz
#define CORD_RTP_PT_DVI4_22050      17  // DVI4 22050 Hz
#define CORD_RTP_PT_G729            18  // ITU-T G.729

// RTCP packet types
#define CORD_RTCP_TYPE_SR           200 // Sender Report
#define CORD_RTCP_TYPE_RR           201 // Receiver Report
#define CORD_RTCP_TYPE_SDES         202 // Source Description
#define CORD_RTCP_TYPE_BYE          203 // Goodbye
#define CORD_RTCP_TYPE_APP          204 // Application-defined

// RTCP SDES types
#define CORD_RTCP_SDES_END          0   // End of SDES list
#define CORD_RTCP_SDES_CNAME        1   // Canonical name
#define CORD_RTCP_SDES_NAME         2   // User name
#define CORD_RTCP_SDES_EMAIL        3   // Electronic mail address
#define CORD_RTCP_SDES_PHONE        4   // Phone number
#define CORD_RTCP_SDES_LOC          5   // Geographic user location
#define CORD_RTCP_SDES_TOOL         6   // Application or tool name
#define CORD_RTCP_SDES_NOTE         7   // Notice/status
#define CORD_RTCP_SDES_PRIV         8   // Private extensions

// IAX2 frame types
#define CORD_IAX2_FRAME_DTMF        1   // DTMF
#define CORD_IAX2_FRAME_VOICE       2   // Voice
#define CORD_IAX2_FRAME_VIDEO       3   // Video
#define CORD_IAX2_FRAME_CONTROL     4   // Control
#define CORD_IAX2_FRAME_NULL        5   // NULL
#define CORD_IAX2_FRAME_IAX         6   // IAX
#define CORD_IAX2_FRAME_TEXT        7   // Text
#define CORD_IAX2_FRAME_IMAGE       8   // Image
#define CORD_IAX2_FRAME_HTML        9   // HTML
#define CORD_IAX2_FRAME_CNG         10  // Comfort Noise

// Helper macros for RTP
#define CORD_RTP_GET_VERSION(rtp)   (((rtp)->version_p_x_cc & 0xC0) >> 6)
#define CORD_RTP_GET_PADDING(rtp)   (((rtp)->version_p_x_cc & 0x20) >> 5)
#define CORD_RTP_GET_EXTENSION(rtp) (((rtp)->version_p_x_cc & 0x10) >> 4)
#define CORD_RTP_GET_CC(rtp)        ((rtp)->version_p_x_cc & 0x0F)
#define CORD_RTP_GET_MARKER(rtp)    (((rtp)->m_pt & 0x80) >> 7)
#define CORD_RTP_GET_PT(rtp)        ((rtp)->m_pt & 0x7F)

#define CORD_RTP_SET_VERSION(rtp, v)   ((rtp)->version_p_x_cc = ((rtp)->version_p_x_cc & 0x3F) | ((v) << 6))
#define CORD_RTP_SET_PADDING(rtp, p)   ((rtp)->version_p_x_cc = ((rtp)->version_p_x_cc & 0xDF) | ((p) << 5))
#define CORD_RTP_SET_EXTENSION(rtp, x) ((rtp)->version_p_x_cc = ((rtp)->version_p_x_cc & 0xEF) | ((x) << 4))
#define CORD_RTP_SET_CC(rtp, cc)       ((rtp)->version_p_x_cc = ((rtp)->version_p_x_cc & 0xF0) | (cc))
#define CORD_RTP_SET_MARKER(rtp, m)    ((rtp)->m_pt = ((rtp)->m_pt & 0x7F) | ((m) << 7))
#define CORD_RTP_SET_PT(rtp, pt)       ((rtp)->m_pt = ((rtp)->m_pt & 0x80) | (pt))

// Helper macros for RTCP
#define CORD_RTCP_GET_VERSION(rtcp) (((rtcp)->version_p_rc & 0xC0) >> 6)
#define CORD_RTCP_GET_PADDING(rtcp) (((rtcp)->version_p_rc & 0x20) >> 5)
#define CORD_RTCP_GET_RC(rtcp)      ((rtcp)->version_p_rc & 0x1F)

// Helper macros for IAX2
#define CORD_IAX2_GET_SCALLNO(iax)  (((iax)->scallno_full & 0x7FFF))
#define CORD_IAX2_GET_FULL(iax)     (((iax)->scallno_full & 0x8000) >> 15)
#define CORD_IAX2_GET_DCALLNO(iax)  (((iax)->dcallno_retrans & 0x7FFF))
#define CORD_IAX2_GET_RETRANS(iax)  (((iax)->dcallno_retrans & 0x8000) >> 15)

#endif // CORD_VOIP_PROTOCOLS_H