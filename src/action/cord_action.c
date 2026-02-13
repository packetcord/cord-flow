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

// IPv4 checksum validation
bool cord_compare_if_ipv4_checksum_valid(const cord_ipv4_hdr_t *ip_hdr)
{
    uint32_t sum = 0;
    const uint8_t *ptr = (const uint8_t*)ip_hdr;
    uint8_t ihl = ip_hdr->ihl * 4; // Header length in bytes

    // Sum all 16-bit words including checksum field
    for (uint8_t i = 0; i < ihl; i += 2) {
        uint16_t word = (ptr[i] << 8) | ptr[i + 1];
        sum += word;
    }

    // Add carry bits and take one's complement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // For a valid checksum, the result should be 0
    return (~sum) == 0;
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

//
// Calculate
//

// Ethernet frame CRC32 calculation
uint32_t cord_calculate_ethernet_crc32(const void *buffer, size_t frame_len)
{
    // Standard Ethernet CRC32 polynomial: 0x04C11DB7
    static const uint32_t crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
        0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
        0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
        0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
        0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
        0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
        0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
        0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
        0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
        0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
        0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
        0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
        0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
        0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
        0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
        0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
        0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
        0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
        0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
        0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
        0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
        0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *data = (const uint8_t*)buffer;

    for (size_t i = 0; i < frame_len; i++) {
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }

    return ~crc;
}

// IPv4 payload length calculation
uint16_t cord_calculate_ipv4_payload_length_ntohs(const cord_ipv4_hdr_t *ip_hdr)
{
    uint16_t total_len = cord_ntohs(ip_hdr->tot_len);
    uint8_t hdr_len = ip_hdr->ihl * 4;
    return total_len - hdr_len;
}

// IPv4 checksum calculation
uint16_t cord_calculate_ipv4_checksum(const cord_ipv4_hdr_t *ip_hdr)
{
    uint32_t sum = 0;
    const uint8_t *ptr = (const uint8_t*)ip_hdr;
    uint8_t ihl = ip_hdr->ihl * 4; // Header length in bytes

    // Save original checksum and zero it for calculation
    uint16_t orig_check = ip_hdr->check;

    // Sum all 16-bit words in the header (skip checksum field)
    for (uint8_t i = 0; i < ihl; i += 2) {
        if (i == 10) continue; // Skip checksum field at offset 10-11
        uint16_t word = (ptr[i] << 8) | ptr[i + 1];
        sum += word;
    }

    // Restore original checksum
    *((uint16_t*)&ip_hdr->check) = orig_check;

    // Add carry bits and take one's complement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

// TCP checksum calculation for IPv4
uint16_t cord_calculate_tcp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    // Verify this is a TCP packet
    if (ip_hdr->protocol != CORD_IPPROTO_TCP) {
        return 0; // Invalid protocol
    }

    // Calculate IP header length and find TCP header
    uint8_t ip_hdr_len = ip_hdr->ihl * 4;
    const cord_tcp_hdr_t *tcp_hdr = (const cord_tcp_hdr_t*)((const uint8_t*)ip_hdr + ip_hdr_len);

    uint32_t sum = 0;
    uint16_t tcp_len = cord_calculate_ipv4_payload_length_ntohs(ip_hdr);

    // Pseudo header: src addr + dst addr + zero + protocol + length
    // Source address (network byte order, split into 16-bit words)
    sum += cord_ntohs((ip_hdr->saddr.addr >> 16) & 0xFFFF);
    sum += cord_ntohs(ip_hdr->saddr.addr & 0xFFFF);
    // Destination address (network byte order, split into 16-bit words)
    sum += cord_ntohs((ip_hdr->daddr.addr >> 16) & 0xFFFF);
    sum += cord_ntohs(ip_hdr->daddr.addr & 0xFFFF);
    // Zero byte + protocol (6 for TCP) - in network byte order
    sum += CORD_IPPROTO_TCP;
    // TCP length
    sum += tcp_len;

    // TCP header and data
    const uint8_t *ptr = (const uint8_t*)tcp_hdr;

    // Sum all 16-bit words, skipping checksum field at offset 16-17
    for (uint16_t i = 0; i < tcp_len / 2; i++) {
        if (i == 8) continue; // Skip checksum field (offset 16-17 = word 8)
        uint16_t word = (ptr[i*2] << 8) | ptr[i*2 + 1];
        sum += word;
    }

    // Handle odd byte
    if (tcp_len & 1) {
        sum += ptr[tcp_len - 1] << 8;
    }

    // Add carry bits and take one's complement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

// UDP checksum calculation for IPv4
uint16_t cord_calculate_udp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    // Verify this is a UDP packet
    if (ip_hdr->protocol != CORD_IPPROTO_UDP) {
        return 0; // Invalid protocol
    }

    // Calculate IP header length and find UDP header
    uint8_t ip_hdr_len = ip_hdr->ihl * 4;
    const cord_udp_hdr_t *udp_hdr = (const cord_udp_hdr_t*)((const uint8_t*)ip_hdr + ip_hdr_len);

    uint32_t sum = 0;
    uint16_t udp_len = cord_ntohs(udp_hdr->len);

    // Pseudo header: src addr + dst addr + zero + protocol + length
    // Source address (network byte order, split into 16-bit words)
    sum += cord_ntohs((ip_hdr->saddr.addr >> 16) & 0xFFFF);
    sum += cord_ntohs(ip_hdr->saddr.addr & 0xFFFF);
    // Destination address (network byte order, split into 16-bit words)
    sum += cord_ntohs((ip_hdr->daddr.addr >> 16) & 0xFFFF);
    sum += cord_ntohs(ip_hdr->daddr.addr & 0xFFFF);
    // Zero byte + protocol (17 for UDP) - in network byte order
    sum += CORD_IPPROTO_UDP;
    // UDP length - already converted to host byte order above
    sum += udp_len;

    // UDP header and data
    const uint8_t *ptr = (const uint8_t*)udp_hdr;

    // Sum all 16-bit words, skipping checksum field at offset 6-7
    for (uint16_t i = 0; i < udp_len / 2; i++) {
        if (i == 3) continue; // Skip checksum field (offset 6-7 = word 3)
        uint16_t word = (ptr[i*2] << 8) | ptr[i*2 + 1];
        sum += word;
    }

    // Handle odd byte
    if (udp_len & 1) {
        sum += ptr[udp_len - 1] << 8;
    }

    // Add carry bits and take one's complement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

// ICMP checksum calculation for IPv4
uint16_t cord_calculate_icmp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    // Verify this is an ICMP packet
    if (ip_hdr->protocol != CORD_IPPROTO_ICMP) {
        return 0; // Invalid protocol
    }

    // Calculate IP header length and find ICMP header
    uint8_t ip_hdr_len = ip_hdr->ihl * 4;
    const cord_icmp_hdr_t *icmp_hdr = (const cord_icmp_hdr_t*)((const uint8_t*)ip_hdr + ip_hdr_len);

    // Calculate ICMP data length
    uint16_t total_len = cord_ntohs(ip_hdr->tot_len);
    uint16_t icmp_len = total_len - ip_hdr_len;

    uint32_t sum = 0;
    const uint8_t *ptr = (const uint8_t*)icmp_hdr;

    // Sum all 16-bit words, skipping checksum field at offset 2-3
    for (uint16_t i = 0; i < icmp_len / 2; i++) {
        if (i == 1) continue; // Skip checksum field (offset 2-3 = word 1)
        uint16_t word = (ptr[i*2] << 8) | ptr[i*2 + 1];
        sum += word;
    }

    // Handle odd byte
    if (icmp_len & 1) {
        sum += ptr[icmp_len - 1] << 8;
    }

    // Add carry bits and take one's complement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}