#include <action/cord_action.h>
#include <stdint.h>

//
// Compare
//

// L2 Ethernet Match Functions
bool cord_compare_eth_dst_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr)
{
    // Compare MAC addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 6; i++) {
        if (eth->h_dest.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_compare_eth_src_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr)
{
    // Compare MAC addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 6; i++) {
        if (eth->h_source.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_compare_eth_type(const cord_eth_hdr_t *eth, uint16_t eth_type)
{
    return cord_get_field_eth_type(eth) == eth_type;
}

bool cord_compare_eth_type_ntohs(const cord_eth_hdr_t *eth, uint16_t eth_type)
{
    return cord_get_field_eth_type_ntohs(eth) == eth_type;
}

// VLAN Match Functions
bool cord_compare_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    return cord_get_field_vlan_pcp(vlan) == pcp;
}

bool cord_compare_vlan_pcp_ntohs(const cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    return cord_get_field_vlan_pcp_ntohs(vlan) == pcp;
}

bool cord_compare_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei)
{
    return cord_get_field_vlan_dei(vlan) == dei;
}

bool cord_compare_vlan_dei_ntohs(const cord_vlan_hdr_t *vlan, uint8_t dei)
{
    return cord_get_field_vlan_dei_ntohs(vlan) == dei;
}

bool cord_compare_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid)
{
    return cord_get_field_vlan_vid(vlan) == vid;
}

bool cord_compare_vlan_vid_ntohs(const cord_vlan_hdr_t *vlan, uint16_t vid)
{
    return cord_get_field_vlan_vid_ntohs(vlan) == vid;
}

bool cord_compare_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid)
{
    uint16_t vid = cord_get_field_vlan_vid(vlan);
    return vid >= min_vid && vid <= max_vid;
}

bool cord_compare_vlan_vid_range_ntohs(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid)
{
    uint16_t vid = cord_get_field_vlan_vid_ntohs(vlan);
    return vid >= min_vid && vid <= max_vid;
}

// L3 IPv4 Match Functions
bool cord_compare_ipv4_version(const cord_ipv4_hdr_t *ip)
{
    return ip->version == 4;
}

bool cord_compare_ipv4_ihl(const cord_ipv4_hdr_t *ip, uint8_t ihl)
{
    return ip->ihl == ihl;
}

bool cord_compare_ipv4_tos(const cord_ipv4_hdr_t *ip, uint8_t tos)
{
    return ip->tos == tos;
}

bool cord_compare_ipv4_dscp(const cord_ipv4_hdr_t *ip, uint8_t dscp)
{
    return ((ip->tos >> 2) & 0x3F) == dscp;
}

bool cord_compare_ipv4_ecn(const cord_ipv4_hdr_t *ip, uint8_t ecn)
{
    return (ip->tos & 0x03) == ecn;
}

bool cord_compare_ipv4_total_length(const cord_ipv4_hdr_t *ip, uint16_t length)
{
    return cord_get_field_ipv4_total_length(ip) == length;
}

bool cord_compare_ipv4_total_length_ntohs(const cord_ipv4_hdr_t *ip, uint16_t length)
{
    return cord_get_field_ipv4_total_length_ntohs(ip) == length;
}

bool cord_compare_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id)
{
    return cord_get_field_ipv4_id(ip) == id;
}

bool cord_compare_ipv4_id_ntohs(const cord_ipv4_hdr_t *ip, uint16_t id)
{
    return cord_get_field_ipv4_id_ntohs(ip) == id;
}

bool cord_compare_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags)
{
    return (cord_get_field_ipv4_frag_off(ip) & 0xE000) == flags;
}

bool cord_compare_ipv4_flags_ntohs(const cord_ipv4_hdr_t *ip, uint16_t flags)
{
    return (cord_get_field_ipv4_frag_off_ntohs(ip) & 0xE000) == flags;
}

bool cord_compare_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset)
{
    return (cord_get_field_ipv4_frag_off(ip) & CORD_IPV4_OFFSET_MASK) == offset;
}

bool cord_compare_ipv4_frag_offset_ntohs(const cord_ipv4_hdr_t *ip, uint16_t offset)
{
    return (cord_get_field_ipv4_frag_off_ntohs(ip) & CORD_IPV4_OFFSET_MASK) == offset;
}

bool cord_compare_ipv4_ttl(const cord_ipv4_hdr_t *ip, uint8_t ttl)
{
    return ip->ttl == ttl;
}

bool cord_compare_ipv4_protocol(const cord_ipv4_hdr_t *ip, uint8_t protocol)
{
    return ip->protocol == protocol;
}

bool cord_compare_ipv4_checksum(const cord_ipv4_hdr_t *ip, uint16_t checksum)
{
    return cord_get_field_ipv4_checksum(ip) == checksum;
}

bool cord_compare_ipv4_checksum_ntohs(const cord_ipv4_hdr_t *ip, uint16_t checksum)
{
    return cord_get_field_ipv4_checksum_ntohs(ip) == checksum;
}

bool cord_compare_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return cord_get_field_ipv4_src_addr(ip) == addr;
}

bool cord_compare_ipv4_src_addr_ntohl(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return cord_get_field_ipv4_src_addr_ntohl(ip) == addr;
}

bool cord_compare_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return cord_get_field_ipv4_dst_addr(ip) == addr;
}

bool cord_compare_ipv4_dst_addr_ntohl(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return cord_get_field_ipv4_dst_addr_ntohl(ip) == addr;
}

bool cord_compare_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_get_field_ipv4_src_addr(ip);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_src_subnet_ntohl(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_get_field_ipv4_src_addr_ntohl(ip);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_get_field_ipv4_dst_addr(ip);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_dst_subnet_ntohl(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_get_field_ipv4_dst_addr_ntohl(ip);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_fragmented(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off(ip);
    return (frag_off & CORD_IPV4_MF) || (frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_fragmented_ntohs(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off_ntohs(ip);
    return (frag_off & CORD_IPV4_MF) || (frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_first_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off(ip);
    return (frag_off & CORD_IPV4_MF) && !(frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_first_fragment_ntohs(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off_ntohs(ip);
    return (frag_off & CORD_IPV4_MF) && !(frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_last_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off(ip);
    return !(frag_off & CORD_IPV4_MF) && (frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_last_fragment_ntohs(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_get_field_ipv4_frag_off_ntohs(ip);
    return !(frag_off & CORD_IPV4_MF) && (frag_off & CORD_IPV4_OFFSET_MASK);
}

// L3 IPv6 Match Functions
bool cord_compare_ipv6_version(const cord_ipv6_hdr_t *ip6)
{
    return ip6->version == 6;
}

bool cord_compare_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, uint8_t tc)
{
    return ip6->traffic_class == tc;
}

bool cord_compare_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, uint32_t flow)
{
    return ip6->flow_label == flow;
}

bool cord_compare_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, uint16_t length)
{
    return cord_get_field_ipv6_payload_length(ip6) == length;
}

bool cord_compare_ipv6_payload_length_ntohs(const cord_ipv6_hdr_t *ip6, uint16_t length)
{
    return cord_get_field_ipv6_payload_length_ntohs(ip6) == length;
}

bool cord_compare_ipv6_next_header(const cord_ipv6_hdr_t *ip6, uint8_t next_hdr)
{
    return ip6->nexthdr == next_hdr;
}

bool cord_compare_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, uint8_t hop_limit)
{
    return ip6->hop_limit == hop_limit;
}

bool cord_compare_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr)
{
    // Compare IPv6 addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 16; i++) {
        if (ip6->saddr.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_compare_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr)
{
    // Compare IPv6 addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 16; i++) {
        if (ip6->daddr.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_compare_ipv6_src_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len)
{
    uint8_t bytes = prefix_len / 8;
    uint8_t bits = prefix_len % 8;
    
    // Compare full bytes
    for (int i = 0; i < bytes; i++) {
        if (ip6->saddr.addr[i] != prefix->addr[i]) {
            return false;
        }
    }
    
    // Compare partial byte if needed
    if (bits > 0) {
        uint8_t mask = 0xFF << (8 - bits);
        if ((ip6->saddr.addr[bytes] & mask) != (prefix->addr[bytes] & mask)) {
            return false;
        }
    }
    
    return true;
}

bool cord_compare_ipv6_dst_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len)
{
    uint8_t bytes = prefix_len / 8;
    uint8_t bits = prefix_len % 8;
    
    // Compare full bytes
    for (int i = 0; i < bytes; i++) {
        if (ip6->daddr.addr[i] != prefix->addr[i]) {
            return false;
        }
    }
    
    // Compare partial byte if needed
    if (bits > 0) {
        uint8_t mask = 0xFF << (8 - bits);
        if ((ip6->daddr.addr[bytes] & mask) != (prefix->addr[bytes] & mask)) {
            return false;
        }
    }
    
    return true;
}

// L4 TCP Match Functions
bool cord_compare_tcp_src_port(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_get_field_tcp_src_port(tcp) == port;
}

bool cord_compare_tcp_src_port_ntohs(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_get_field_tcp_src_port_ntohs(tcp) == port;
}

bool cord_compare_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_get_field_tcp_dst_port(tcp) == port;
}

bool cord_compare_tcp_dst_port_ntohs(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_get_field_tcp_dst_port_ntohs(tcp) == port;
}

bool cord_compare_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_tcp_src_port(tcp) : cord_get_field_tcp_dst_port(tcp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_tcp_port_range_ntohs(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_tcp_src_port_ntohs(tcp) : cord_get_field_tcp_dst_port_ntohs(tcp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq)
{
    return cord_get_field_tcp_seq_num(tcp) == seq;
}

bool cord_compare_tcp_seq_num_ntohl(const cord_tcp_hdr_t *tcp, uint32_t seq)
{
    return cord_get_field_tcp_seq_num_ntohl(tcp) == seq;
}

bool cord_compare_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack)
{
    return cord_get_field_tcp_ack_num(tcp) == ack;
}

bool cord_compare_tcp_ack_num_ntohl(const cord_tcp_hdr_t *tcp, uint32_t ack)
{
    return cord_get_field_tcp_ack_num_ntohl(tcp) == ack;
}

bool cord_compare_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset)
{
    return tcp->doff == offset;
}

bool cord_compare_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window)
{
    return cord_get_field_tcp_window(tcp) == window;
}

bool cord_compare_tcp_window_ntohs(const cord_tcp_hdr_t *tcp, uint16_t window)
{
    return cord_get_field_tcp_window_ntohs(tcp) == window;
}

bool cord_compare_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    return cord_get_field_tcp_checksum(tcp) == checksum;
}

bool cord_compare_tcp_checksum_ntohs(const cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    return cord_get_field_tcp_checksum_ntohs(tcp) == checksum;
}

bool cord_compare_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    return cord_get_field_tcp_urgent_ptr(tcp) == urg_ptr;
}

bool cord_compare_tcp_urgent_ptr_ntohs(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    return cord_get_field_tcp_urgent_ptr_ntohs(tcp) == urg_ptr;
}

bool cord_compare_tcp_syn(const cord_tcp_hdr_t *tcp)
{
    return tcp->syn;
}

bool cord_compare_tcp_ack(const cord_tcp_hdr_t *tcp)
{
    return tcp->ack;
}

bool cord_compare_tcp_fin(const cord_tcp_hdr_t *tcp)
{
    return tcp->fin;
}

bool cord_compare_tcp_rst(const cord_tcp_hdr_t *tcp)
{
    return tcp->rst;
}

bool cord_compare_tcp_psh(const cord_tcp_hdr_t *tcp)
{
    return tcp->psh;
}

bool cord_compare_tcp_urg(const cord_tcp_hdr_t *tcp)
{
    return tcp->urg;
}

bool cord_compare_tcp_ece(const cord_tcp_hdr_t *tcp)
{
    return tcp->ece;
}

bool cord_compare_tcp_cwr(const cord_tcp_hdr_t *tcp)
{
    return tcp->cwr;
}

bool cord_compare_tcp_established(const cord_tcp_hdr_t *tcp)
{
    return tcp->ack && !tcp->syn;
}

bool cord_compare_tcp_connection_request(const cord_tcp_hdr_t *tcp)
{
    return tcp->syn && !tcp->ack;
}

// L4 UDP Match Functions
bool cord_compare_udp_src_port(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_get_field_udp_src_port(udp) == port;
}

bool cord_compare_udp_src_port_ntohs(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_get_field_udp_src_port_ntohs(udp) == port;
}

bool cord_compare_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_get_field_udp_dst_port(udp) == port;
}

bool cord_compare_udp_dst_port_ntohs(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_get_field_udp_dst_port_ntohs(udp) == port;
}

bool cord_compare_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_udp_src_port(udp) : cord_get_field_udp_dst_port(udp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_udp_port_range_ntohs(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_udp_src_port_ntohs(udp) : cord_get_field_udp_dst_port_ntohs(udp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_udp_length(const cord_udp_hdr_t *udp, uint16_t length)
{
    return cord_get_field_udp_length(udp) == length;
}

bool cord_compare_udp_length_ntohs(const cord_udp_hdr_t *udp, uint16_t length)
{
    return cord_get_field_udp_length_ntohs(udp) == length;
}

bool cord_compare_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum)
{
    return cord_get_field_udp_checksum(udp) == checksum;
}

bool cord_compare_udp_checksum_ntohs(const cord_udp_hdr_t *udp, uint16_t checksum)
{
    return cord_get_field_udp_checksum_ntohs(udp) == checksum;
}

// L4 SCTP Match Functions  
bool cord_compare_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_get_field_sctp_src_port(sctp) == port;
}

bool cord_compare_sctp_src_port_ntohs(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_get_field_sctp_src_port_ntohs(sctp) == port;
}

bool cord_compare_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_get_field_sctp_dst_port(sctp) == port;
}

bool cord_compare_sctp_dst_port_ntohs(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_get_field_sctp_dst_port_ntohs(sctp) == port;
}

bool cord_compare_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_sctp_src_port(sctp) : cord_get_field_sctp_dst_port(sctp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_sctp_port_range_ntohs(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_get_field_sctp_src_port_ntohs(sctp) : cord_get_field_sctp_dst_port_ntohs(sctp);
    return port >= min_port && port <= max_port;
}

bool cord_compare_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    return cord_get_field_sctp_vtag(sctp) == vtag;
}

bool cord_compare_sctp_vtag_ntohl(const cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    return cord_get_field_sctp_vtag_ntohl(sctp) == vtag;
}

bool cord_compare_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    return cord_get_field_sctp_checksum(sctp) == checksum;
}

bool cord_compare_sctp_checksum_ntohl(const cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    return cord_get_field_sctp_checksum_ntohl(sctp) == checksum;
}

// ICMP Match Functions
bool cord_compare_icmp_type(const cord_icmp_hdr_t *icmp, uint8_t type)
{
    return cord_get_field_icmp_type(icmp) == type;
}

bool cord_compare_icmp_code(const cord_icmp_hdr_t *icmp, uint8_t code)
{
    return cord_get_field_icmp_code(icmp) == code;
}

bool cord_compare_icmp_echo_request(const cord_icmp_hdr_t *icmp)
{
    return cord_get_field_icmp_type(icmp) == CORD_ICMP_ECHO;
}

bool cord_compare_icmp_echo_reply(const cord_icmp_hdr_t *icmp)
{
    return cord_get_field_icmp_type(icmp) == CORD_ICMP_ECHOREPLY;
}

bool cord_compare_icmp_dest_unreachable(const cord_icmp_hdr_t *icmp)
{
    return cord_get_field_icmp_type(icmp) == CORD_ICMP_DEST_UNREACH;
}

// Tunneling Protocol Match Functions
bool cord_compare_gre_checksum_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_CSUM(gre);
}

bool cord_compare_gre_key_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_KEY(gre);
}

bool cord_compare_gre_sequence_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_SEQ(gre);
}

bool cord_compare_gre_protocol(const cord_gre_hdr_t *gre, uint16_t protocol)
{
    return cord_ntohs(gre->protocol) == protocol;
}

bool cord_compare_vxlan_vni(const cord_vxlan_hdr_t *vxlan, uint32_t vni)
{
    return CORD_VXLAN_GET_VNI(vxlan) == vni;
}

bool cord_compare_vxlan_flags(const cord_vxlan_hdr_t *vxlan, uint8_t flags)
{
    return vxlan->flags == flags;
}

bool cord_compare_gtpu_teid(const cord_gtpu_hdr_t *gtpu, uint32_t teid)
{
    return cord_ntohl(gtpu->teid) == teid;
}

bool cord_compare_gtpu_msg_type(const cord_gtpu_hdr_t *gtpu, uint8_t msg_type)
{
    return gtpu->message_type == msg_type;
}

// VoIP Protocol Match Functions  
bool cord_compare_rtp_version(const cord_rtp_hdr_t *rtp, uint8_t version)
{
    return CORD_RTP_GET_VERSION(rtp) == version;
}

bool cord_compare_rtp_payload_type(const cord_rtp_hdr_t *rtp, uint8_t pt)
{
    return CORD_RTP_GET_PT(rtp) == pt;
}

bool cord_compare_rtp_ssrc(const cord_rtp_hdr_t *rtp, uint32_t ssrc)
{
    return cord_ntohl(rtp->ssrc) == ssrc;
}

bool cord_compare_rtp_marker(const cord_rtp_hdr_t *rtp)
{
    return CORD_RTP_GET_MARKER(rtp);
}

bool cord_compare_rtcp_packet_type(const cord_rtcp_hdr_t *rtcp, uint8_t type)
{
    return rtcp->packet_type == type;
}

bool cord_compare_rtcp_version(const cord_rtcp_hdr_t *rtcp, uint8_t version)
{
    return CORD_RTCP_GET_VERSION(rtcp) == version;
}

// Ethernet multicast or broadcast
bool cord_compare_if_is_multicast(const cord_mac_addr_t *mac_addr)
{
    return (mac_addr->addr[0] & 0x01) != 0;
}

bool cord_compare_if_is_broadcast(const cord_mac_addr_t *mac_addr)
{
    return (mac_addr->addr[0] == 0xFF && mac_addr->addr[1] == 0xFF && 
            mac_addr->addr[2] == 0xFF && mac_addr->addr[3] == 0xFF &&
            mac_addr->addr[4] == 0xFF && mac_addr->addr[5] == 0xFF);
}

// Advanced Protocol Analysis Functions
bool cord_compare_if_is_dns_query(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->dest) == CORD_PORT_DNS || cord_ntohs(udp->source) == CORD_PORT_DNS;
}

bool cord_compare_if_is_dns_response(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DNS;
}

bool cord_compare_if_is_dhcp_request(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DHCP_CLIENT && cord_ntohs(udp->dest) == CORD_PORT_DHCP_SERVER;
}

bool cord_compare_if_is_dhcp_response(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DHCP_SERVER && cord_ntohs(udp->dest) == CORD_PORT_DHCP_CLIENT;
}

bool cord_compare_if_is_http_request(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_HTTP;
}

bool cord_compare_if_is_http_response(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->source) == CORD_PORT_HTTP;
}

bool cord_compare_if_is_https_traffic(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_HTTPS || cord_ntohs(tcp->source) == CORD_PORT_HTTPS;
}

bool cord_compare_if_is_ssh_traffic(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_SSH || cord_ntohs(tcp->source) == CORD_PORT_SSH;
}

//
// Set
//

// Ethernet Field Setters
void cord_set_field_eth_dst_addr(cord_eth_hdr_t *eth, const cord_mac_addr_t *dst)
{
    eth->h_dest = *dst;
}

void cord_set_field_eth_src_addr(cord_eth_hdr_t *eth, const cord_mac_addr_t *src)
{
    eth->h_source = *src;
}

void cord_set_field_eth_type(cord_eth_hdr_t *eth, uint16_t type)
{
    eth->h_proto = type;
}

void cord_set_field_eth_type_htons(cord_eth_hdr_t *eth, uint16_t type)
{
    eth->h_proto = cord_htons(type);
}

// VLAN Field Setters
void cord_set_field_vlan_tci(cord_vlan_hdr_t *vlan, uint16_t tci)
{
    vlan->tci = tci;
}

void cord_set_field_vlan_tci_htons(cord_vlan_hdr_t *vlan, uint16_t tci)
{
    vlan->tci = cord_htons(tci);
}

void cord_set_field_vlan_pcp(cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    vlan->tci = (vlan->tci & ~(0x07 << 13)) | ((pcp & 0x07) << 13);
}

void cord_set_field_vlan_pcp_htons(cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    uint16_t tci_host = cord_ntohs(vlan->tci);
    tci_host = (tci_host & ~(0x07 << 13)) | ((pcp & 0x07) << 13);
    vlan->tci = cord_htons(tci_host);
}

void cord_set_field_vlan_dei(cord_vlan_hdr_t *vlan, uint8_t dei)
{
    vlan->tci = (vlan->tci & ~(0x01 << 12)) | ((dei & 0x01) << 12);
}

void cord_set_field_vlan_dei_htons(cord_vlan_hdr_t *vlan, uint8_t dei)
{
    uint16_t tci_host = cord_ntohs(vlan->tci);
    tci_host = (tci_host & ~(0x01 << 12)) | ((dei & 0x01) << 12);
    vlan->tci = cord_htons(tci_host);
}

void cord_set_field_vlan_vid(cord_vlan_hdr_t *vlan, uint16_t vid)
{
    vlan->tci = (vlan->tci & ~0x0FFF) | (vid & 0x0FFF);
}

void cord_set_field_vlan_vid_htons(cord_vlan_hdr_t *vlan, uint16_t vid)
{
    uint16_t tci_host = cord_ntohs(vlan->tci);
    tci_host = (tci_host & ~0x0FFF) | (vid & 0x0FFF);
    vlan->tci = cord_htons(tci_host);
}

void cord_set_field_vlan_type(cord_vlan_hdr_t *vlan, uint16_t type)
{
    vlan->h_proto = type;
}

void cord_set_field_vlan_type_htons(cord_vlan_hdr_t *vlan, uint16_t type)
{
    vlan->h_proto = cord_htons(type);
}

// IPv4 Field Setters
void cord_set_field_ipv4_version(cord_ipv4_hdr_t *ip, uint8_t version)
{
    ip->version = version;
}

void cord_set_field_ipv4_ihl(cord_ipv4_hdr_t *ip, uint8_t ihl)
{
    ip->ihl = ihl;
}

void cord_set_field_ipv4_tos(cord_ipv4_hdr_t *ip, uint8_t tos)
{
    ip->tos = tos;
}

void cord_set_field_ipv4_dscp(cord_ipv4_hdr_t *ip, uint8_t dscp)
{
    ip->tos = (ip->tos & 0x03) | ((dscp & 0x3F) << 2);
}

void cord_set_field_ipv4_ecn(cord_ipv4_hdr_t *ip, uint8_t ecn)
{
    ip->tos = (ip->tos & 0xFC) | (ecn & 0x03);
}

void cord_set_field_ipv4_total_length(cord_ipv4_hdr_t *ip, uint16_t length)
{
    ip->tot_len = length;
}

void cord_set_field_ipv4_total_length_htons(cord_ipv4_hdr_t *ip, uint16_t length)
{
    ip->tot_len = cord_htons(length);
}

void cord_set_field_ipv4_id(cord_ipv4_hdr_t *ip, uint16_t id)
{
    ip->id = id;
}

void cord_set_field_ipv4_id_htons(cord_ipv4_hdr_t *ip, uint16_t id)
{
    ip->id = cord_htons(id);
}

void cord_set_field_ipv4_frag_off(cord_ipv4_hdr_t *ip, uint16_t frag_off)
{
    ip->frag_off = frag_off;
}

void cord_set_field_ipv4_frag_off_htons(cord_ipv4_hdr_t *ip, uint16_t frag_off)
{
    ip->frag_off = cord_htons(frag_off);
}

void cord_set_field_ipv4_ttl(cord_ipv4_hdr_t *ip, uint8_t ttl)
{
    ip->ttl = ttl;
}

void cord_set_field_ipv4_protocol(cord_ipv4_hdr_t *ip, uint8_t protocol)
{
    ip->protocol = protocol;
}

void cord_set_field_ipv4_checksum(cord_ipv4_hdr_t *ip, uint16_t checksum)
{
    ip->check = checksum;
}

void cord_set_field_ipv4_checksum_htons(cord_ipv4_hdr_t *ip, uint16_t checksum)
{
    ip->check = cord_htons(checksum);
}

void cord_set_field_ipv4_src_addr(cord_ipv4_hdr_t *ip, uint32_t addr)
{
    ip->saddr.addr = addr;
}

void cord_set_field_ipv4_src_addr_htonl(cord_ipv4_hdr_t *ip, uint32_t addr)
{
    ip->saddr.addr = cord_htonl(addr);
}

void cord_set_field_ipv4_dst_addr(cord_ipv4_hdr_t *ip, uint32_t addr)
{
    ip->daddr.addr = addr;
}

void cord_set_field_ipv4_dst_addr_htonl(cord_ipv4_hdr_t *ip, uint32_t addr)
{
    ip->daddr.addr = cord_htonl(addr);
}

// IPv6 Field Setters
void cord_set_field_ipv6_version(cord_ipv6_hdr_t *ip6, uint8_t version)
{
    ip6->version = version;
}

void cord_set_field_ipv6_traffic_class(cord_ipv6_hdr_t *ip6, uint8_t tc)
{
    ip6->traffic_class = tc;
}

void cord_set_field_ipv6_flow_label(cord_ipv6_hdr_t *ip6, uint32_t flow)
{
    ip6->flow_label = flow;
}

void cord_set_field_ipv6_payload_length(cord_ipv6_hdr_t *ip6, uint16_t length)
{
    ip6->payload_len = length;
}

void cord_set_field_ipv6_payload_length_htons(cord_ipv6_hdr_t *ip6, uint16_t length)
{
    ip6->payload_len = cord_htons(length);
}

void cord_set_field_ipv6_next_header(cord_ipv6_hdr_t *ip6, uint8_t next_hdr)
{
    ip6->nexthdr = next_hdr;
}

void cord_set_field_ipv6_hop_limit(cord_ipv6_hdr_t *ip6, uint8_t hop_limit)
{
    ip6->hop_limit = hop_limit;
}

void cord_set_field_ipv6_src_addr(cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *src)
{
    ip6->saddr = *src;
}

void cord_set_field_ipv6_dst_addr(cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *dst)
{
    ip6->daddr = *dst;
}

// TCP Field Setters
void cord_set_field_tcp_src_port(cord_tcp_hdr_t *tcp, uint16_t port)
{
    tcp->source = port;
}

void cord_set_field_tcp_src_port_htons(cord_tcp_hdr_t *tcp, uint16_t port)
{
    tcp->source = cord_htons(port);
}

void cord_set_field_tcp_dst_port(cord_tcp_hdr_t *tcp, uint16_t port)
{
    tcp->dest = port;
}

void cord_set_field_tcp_dst_port_htons(cord_tcp_hdr_t *tcp, uint16_t port)
{
    tcp->dest = cord_htons(port);
}

void cord_set_field_tcp_seq_num(cord_tcp_hdr_t *tcp, uint32_t seq)
{
    tcp->seq = seq;
}

void cord_set_field_tcp_seq_num_htonl(cord_tcp_hdr_t *tcp, uint32_t seq)
{
    tcp->seq = cord_htonl(seq);
}

void cord_set_field_tcp_ack_num(cord_tcp_hdr_t *tcp, uint32_t ack)
{
    tcp->ack_seq = ack;
}

void cord_set_field_tcp_ack_num_htonl(cord_tcp_hdr_t *tcp, uint32_t ack)
{
    tcp->ack_seq = cord_htonl(ack);
}

void cord_set_field_tcp_doff(cord_tcp_hdr_t *tcp, uint8_t doff)
{
    tcp->doff = doff;
}

void cord_set_field_tcp_window(cord_tcp_hdr_t *tcp, uint16_t window)
{
    tcp->window = window;
}

void cord_set_field_tcp_window_htons(cord_tcp_hdr_t *tcp, uint16_t window)
{
    tcp->window = cord_htons(window);
}

void cord_set_field_tcp_checksum(cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    tcp->check = checksum;
}

void cord_set_field_tcp_checksum_htons(cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    tcp->check = cord_htons(checksum);
}

void cord_set_field_tcp_urgent_ptr(cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    tcp->urg_ptr = urg_ptr;
}

void cord_set_field_tcp_urgent_ptr_htons(cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    tcp->urg_ptr = cord_htons(urg_ptr);
}

void cord_set_field_tcp_fin(cord_tcp_hdr_t *tcp, bool fin)
{
    tcp->fin = fin ? 1 : 0;
}

void cord_set_field_tcp_syn(cord_tcp_hdr_t *tcp, bool syn)
{
    tcp->syn = syn ? 1 : 0;
}

void cord_set_field_tcp_rst(cord_tcp_hdr_t *tcp, bool rst)
{
    tcp->rst = rst ? 1 : 0;
}

void cord_set_field_tcp_psh(cord_tcp_hdr_t *tcp, bool psh)
{
    tcp->psh = psh ? 1 : 0;
}

void cord_set_field_tcp_ack(cord_tcp_hdr_t *tcp, bool ack)
{
    tcp->ack = ack ? 1 : 0;
}

void cord_set_field_tcp_urg(cord_tcp_hdr_t *tcp, bool urg)
{
    tcp->urg = urg ? 1 : 0;
}

void cord_set_field_tcp_ece(cord_tcp_hdr_t *tcp, bool ece)
{
    tcp->ece = ece ? 1 : 0;
}

void cord_set_field_tcp_cwr(cord_tcp_hdr_t *tcp, bool cwr)
{
    tcp->cwr = cwr ? 1 : 0;
}

// UDP Field Setters
void cord_set_field_udp_src_port(cord_udp_hdr_t *udp, uint16_t port)
{
    udp->source = port;
}

void cord_set_field_udp_src_port_htons(cord_udp_hdr_t *udp, uint16_t port)
{
    udp->source = cord_htons(port);
}

void cord_set_field_udp_dst_port(cord_udp_hdr_t *udp, uint16_t port)
{
    udp->dest = port;
}

void cord_set_field_udp_dst_port_htons(cord_udp_hdr_t *udp, uint16_t port)
{
    udp->dest = cord_htons(port);
}

void cord_set_field_udp_length(cord_udp_hdr_t *udp, uint16_t length)
{
    udp->len = length;
}

void cord_set_field_udp_length_htons(cord_udp_hdr_t *udp, uint16_t length)
{
    udp->len = cord_htons(length);
}

void cord_set_field_udp_checksum(cord_udp_hdr_t *udp, uint16_t checksum)
{
    udp->check = checksum;
}

void cord_set_field_udp_checksum_htons(cord_udp_hdr_t *udp, uint16_t checksum)
{
    udp->check = cord_htons(checksum);
}

// SCTP Field Setters
void cord_set_field_sctp_src_port(cord_sctp_hdr_t *sctp, uint16_t port)
{
    sctp->source = port;
}

void cord_set_field_sctp_src_port_htons(cord_sctp_hdr_t *sctp, uint16_t port)
{
    sctp->source = cord_htons(port);
}

void cord_set_field_sctp_dst_port(cord_sctp_hdr_t *sctp, uint16_t port)
{
    sctp->dest = port;
}

void cord_set_field_sctp_dst_port_htons(cord_sctp_hdr_t *sctp, uint16_t port)
{
    sctp->dest = cord_htons(port);
}

void cord_set_field_sctp_vtag(cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    sctp->vtag = vtag;
}

void cord_set_field_sctp_vtag_htonl(cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    sctp->vtag = cord_htonl(vtag);
}

void cord_set_field_sctp_checksum(cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    sctp->checksum = checksum;
}

void cord_set_field_sctp_checksum_htonl(cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    sctp->checksum = cord_htonl(checksum);
}

// ICMP Field Setters
void cord_set_field_icmp_type(cord_icmp_hdr_t *icmp, uint8_t type)
{
    icmp->type = type;
}

void cord_set_field_icmp_code(cord_icmp_hdr_t *icmp, uint8_t code)
{
    icmp->code = code;
}

void cord_set_field_icmp_checksum(cord_icmp_hdr_t *icmp, uint16_t checksum)
{
    icmp->checksum = checksum;
}

void cord_set_field_icmp_checksum_htons(cord_icmp_hdr_t *icmp, uint16_t checksum)
{
    icmp->checksum = cord_htons(checksum);
}

void cord_set_field_icmp_id(cord_icmp_hdr_t *icmp, uint16_t id)
{
    icmp->un.echo.id = id;
}

void cord_set_field_icmp_id_htons(cord_icmp_hdr_t *icmp, uint16_t id)
{
    icmp->un.echo.id = cord_htons(id);
}

void cord_set_field_icmp_sequence(cord_icmp_hdr_t *icmp, uint16_t sequence)
{
    icmp->un.echo.sequence = sequence;
}

void cord_set_field_icmp_sequence_htons(cord_icmp_hdr_t *icmp, uint16_t sequence)
{
    icmp->un.echo.sequence = cord_htons(sequence);
}