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
    return cord_ntohs(eth->h_proto) == eth_type;
}

// VLAN Match Functions
bool cord_compare_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    return cord_get_field_vlan_pcp_ntohs(vlan) == pcp;
}

bool cord_compare_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei)
{
    return cord_get_field_vlan_dei_ntohs(vlan) == dei;
}

bool cord_compare_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid)
{
    return cord_get_field_vlan_vid_ntohs(vlan) == vid;
}

bool cord_compare_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid)
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
    return cord_ntohs(ip->tot_len) == length;
}

bool cord_compare_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id)
{
    return cord_ntohs(ip->id) == id;
}

bool cord_compare_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags)
{
    return (cord_ntohs(ip->frag_off) & 0xE000) == flags;
}

bool cord_compare_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset)
{
    return (cord_ntohs(ip->frag_off) & CORD_IPV4_OFFSET_MASK) == offset;
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
    return cord_ntohs(ip->check) == checksum;
}

bool cord_compare_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return ip->saddr.addr == cord_htonl(addr);  // Compare in network byte order
}

bool cord_compare_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return ip->daddr.addr == cord_htonl(addr);  // Compare in network byte order
}

bool cord_compare_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_ntohl(ip->saddr.addr);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_ntohl(ip->daddr.addr);
    return (addr & mask) == (subnet & mask);
}

bool cord_compare_ipv4_fragmented(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
    return (frag_off & CORD_IPV4_MF) || (frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_first_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
    return (frag_off & CORD_IPV4_MF) && !(frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_compare_ipv4_last_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
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
    return cord_ntohs(ip6->payload_len) == length;
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
    return cord_ntohs(tcp->source) == port;
}

bool cord_compare_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_ntohs(tcp->dest) == port;
}

bool cord_compare_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(tcp->source) : cord_ntohs(tcp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_compare_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq)
{
    return cord_ntohl(tcp->seq) == seq;
}

bool cord_compare_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack)
{
    return cord_ntohl(tcp->ack_seq) == ack;
}

bool cord_compare_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset)
{
    return tcp->doff == offset;
}

bool cord_compare_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window)
{
    return cord_ntohs(tcp->window) == window;
}

bool cord_compare_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    return cord_ntohs(tcp->check) == checksum;
}

bool cord_compare_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    return cord_ntohs(tcp->urg_ptr) == urg_ptr;
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
    return cord_ntohs(udp->source) == port;
}

bool cord_compare_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_ntohs(udp->dest) == port;
}

bool cord_compare_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(udp->source) : cord_ntohs(udp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_compare_udp_length(const cord_udp_hdr_t *udp, uint16_t length)
{
    return cord_ntohs(udp->len) == length;
}

bool cord_compare_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum)
{
    return cord_ntohs(udp->check) == checksum;
}

// L4 SCTP Match Functions  
bool cord_compare_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_ntohs(sctp->source) == port;
}

bool cord_compare_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_ntohs(sctp->dest) == port;
}

bool cord_compare_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(sctp->source) : cord_ntohs(sctp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_compare_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    return cord_ntohl(sctp->vtag) == vtag;
}

bool cord_compare_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    return cord_ntohl(sctp->checksum) == checksum;
}

// ICMP Match Functions
bool cord_compare_icmp_type(const cord_icmp_hdr_t *icmp, uint8_t type)
{
    return icmp->type == type;
}

bool cord_compare_icmp_code(const cord_icmp_hdr_t *icmp, uint8_t code)
{
    return icmp->code == code;
}

bool cord_compare_icmp_echo_request(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_ECHO;
}

bool cord_compare_icmp_echo_reply(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_ECHOREPLY;
}

bool cord_compare_icmp_dest_unreachable(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_DEST_UNREACH;
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