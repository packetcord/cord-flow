#ifndef CORD_ACTION_H
#define CORD_ACTION_H

// ========================================================= //
//                        CORD Actions                       //
// ========================================================= //
// Compare (exact <value> | LPM | compare if is <something>) //
//                                                           //
// Set                                                       //
//                                                           //
// Log                                                       //
//                                                           //
// Push | Pop | Swap                                         //
// --------------------------------------------------------- //

#include <match/cord_match.h>
#include <protocol_headers/cord_protocol_headers.h>
#include <stdint.h>

//
// Compare
//

// L2 Ethernet Match Functions
bool cord_compare_eth_dst_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr);
bool cord_compare_eth_src_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr);
bool cord_compare_eth_type(const cord_eth_hdr_t *eth, uint16_t eth_type);

// VLAN Match Functions
bool cord_compare_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp);
bool cord_compare_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei);
bool cord_compare_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid);
bool cord_compare_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid);

// L3 IPv4 Match Functions
bool cord_compare_ipv4_version(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_ihl(const cord_ipv4_hdr_t *ip, uint8_t ihl);
bool cord_compare_ipv4_tos(const cord_ipv4_hdr_t *ip, uint8_t tos);
bool cord_compare_ipv4_dscp(const cord_ipv4_hdr_t *ip, uint8_t dscp);
bool cord_compare_ipv4_ecn(const cord_ipv4_hdr_t *ip, uint8_t ecn);
bool cord_compare_ipv4_total_length(const cord_ipv4_hdr_t *ip, uint16_t length);
bool cord_compare_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id);
bool cord_compare_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags);
bool cord_compare_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset);
bool cord_compare_ipv4_ttl(const cord_ipv4_hdr_t *ip, uint8_t ttl);
bool cord_compare_ipv4_protocol(const cord_ipv4_hdr_t *ip, uint8_t protocol);
bool cord_compare_ipv4_checksum(const cord_ipv4_hdr_t *ip, uint16_t checksum);
bool cord_compare_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_fragmented(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_first_fragment(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_last_fragment(const cord_ipv4_hdr_t *ip);

// L3 IPv6 Match Functions
bool cord_compare_ipv6_version(const cord_ipv6_hdr_t *ip6);
bool cord_compare_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, uint8_t tc);
bool cord_compare_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, uint32_t flow);
bool cord_compare_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, uint16_t length);
bool cord_compare_ipv6_next_header(const cord_ipv6_hdr_t *ip6, uint8_t next_hdr);
bool cord_compare_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, uint8_t hop_limit);
bool cord_compare_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_compare_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_compare_ipv6_src_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);
bool cord_compare_ipv6_dst_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);

// L4 TCP Match Functions
bool cord_compare_tcp_src_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq);
bool cord_compare_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack);
bool cord_compare_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset);
bool cord_compare_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window);
bool cord_compare_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum);
bool cord_compare_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
bool cord_compare_tcp_syn(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_ack(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_fin(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_rst(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_psh(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_urg(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_ece(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_cwr(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_established(const cord_tcp_hdr_t *tcp);
bool cord_compare_tcp_connection_request(const cord_tcp_hdr_t *tcp);

// L4 UDP Match Functions
bool cord_compare_udp_src_port(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_compare_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_compare_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_udp_length(const cord_udp_hdr_t *udp, uint16_t length);
bool cord_compare_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum);

// L4 SCTP Match Functions  
bool cord_compare_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag);
bool cord_compare_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum);

// ICMP Match Functions
bool cord_compare_icmp_type(const cord_icmp_hdr_t *icmp, uint8_t type);
bool cord_compare_icmp_code(const cord_icmp_hdr_t *icmp, uint8_t code);
bool cord_compare_icmp_echo_request(const cord_icmp_hdr_t *icmp);
bool cord_compare_icmp_echo_reply(const cord_icmp_hdr_t *icmp);
bool cord_compare_icmp_dest_unreachable(const cord_icmp_hdr_t *icmp);

// Tunneling Protocol Match Functions
bool cord_compare_gre_checksum_present(const cord_gre_hdr_t *gre);
bool cord_compare_gre_key_present(const cord_gre_hdr_t *gre);
bool cord_compare_gre_sequence_present(const cord_gre_hdr_t *gre);
bool cord_compare_gre_protocol(const cord_gre_hdr_t *gre, uint16_t protocol);

bool cord_compare_vxlan_vni(const cord_vxlan_hdr_t *vxlan, uint32_t vni);
bool cord_compare_vxlan_flags(const cord_vxlan_hdr_t *vxlan, uint8_t flags);

bool cord_compare_gtpu_teid(const cord_gtpu_hdr_t *gtpu, uint32_t teid);
bool cord_compare_gtpu_msg_type(const cord_gtpu_hdr_t *gtpu, uint8_t msg_type);

// VoIP Protocol Match Functions  
bool cord_compare_rtp_version(const cord_rtp_hdr_t *rtp, uint8_t version);
bool cord_compare_rtp_payload_type(const cord_rtp_hdr_t *rtp, uint8_t pt);
bool cord_compare_rtp_ssrc(const cord_rtp_hdr_t *rtp, uint32_t ssrc);
bool cord_compare_rtp_marker(const cord_rtp_hdr_t *rtp);

bool cord_compare_rtcp_packet_type(const cord_rtcp_hdr_t *rtcp, uint8_t type);
bool cord_compare_rtcp_version(const cord_rtcp_hdr_t *rtcp, uint8_t version);

// Ethernet multicast or broadcast
bool cord_compare_if_is_multicast(const cord_mac_addr_t *mac_addr);
bool cord_compare_if_is_broadcast(const cord_mac_addr_t *mac_addr);

// Advanced Protocol Analysis Functions
bool cord_compare_if_is_dns_query(const cord_udp_hdr_t *udp);
bool cord_compare_if_is_dns_response(const cord_udp_hdr_t *udp);
bool cord_compare_if_is_dhcp_request(const cord_udp_hdr_t *udp);
bool cord_compare_if_is_dhcp_response(const cord_udp_hdr_t *udp);
bool cord_compare_if_is_http_request(const cord_tcp_hdr_t *tcp);
bool cord_compare_if_is_http_response(const cord_tcp_hdr_t *tcp);
bool cord_compare_if_is_https_traffic(const cord_tcp_hdr_t *tcp);
bool cord_compare_if_is_ssh_traffic(const cord_tcp_hdr_t *tcp);

#endif // CORD_ACTION_H
