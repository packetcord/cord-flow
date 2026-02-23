#ifndef CORD_ACTION_H
#define CORD_ACTION_H

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
bool cord_compare_eth_type_ntohs(const cord_eth_hdr_t *eth, uint16_t eth_type);

// VLAN Match Functions
bool cord_compare_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp);
bool cord_compare_vlan_pcp_ntohs(const cord_vlan_hdr_t *vlan, uint8_t pcp);
bool cord_compare_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei);
bool cord_compare_vlan_dei_ntohs(const cord_vlan_hdr_t *vlan, uint8_t dei);
bool cord_compare_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid);
bool cord_compare_vlan_vid_ntohs(const cord_vlan_hdr_t *vlan, uint16_t vid);
bool cord_compare_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid);
bool cord_compare_vlan_vid_range_ntohs(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid);

// L3 IPv4 Match Functions
bool cord_compare_ipv4_version(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_ihl(const cord_ipv4_hdr_t *ip, uint8_t ihl);
bool cord_compare_ipv4_tos(const cord_ipv4_hdr_t *ip, uint8_t tos);
bool cord_compare_ipv4_dscp(const cord_ipv4_hdr_t *ip, uint8_t dscp);
bool cord_compare_ipv4_ecn(const cord_ipv4_hdr_t *ip, uint8_t ecn);
bool cord_compare_ipv4_total_length(const cord_ipv4_hdr_t *ip, uint16_t length);
bool cord_compare_ipv4_total_length_ntohs(const cord_ipv4_hdr_t *ip, uint16_t length);
bool cord_compare_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id);
bool cord_compare_ipv4_id_ntohs(const cord_ipv4_hdr_t *ip, uint16_t id);
bool cord_compare_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags);
bool cord_compare_ipv4_flags_ntohs(const cord_ipv4_hdr_t *ip, uint16_t flags);
bool cord_compare_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset);
bool cord_compare_ipv4_frag_offset_ntohs(const cord_ipv4_hdr_t *ip, uint16_t offset);
bool cord_compare_ipv4_ttl(const cord_ipv4_hdr_t *ip, uint8_t ttl);
bool cord_compare_ipv4_protocol(const cord_ipv4_hdr_t *ip, uint8_t protocol);
bool cord_compare_ipv4_checksum(const cord_ipv4_hdr_t *ip, uint16_t checksum);
bool cord_compare_ipv4_checksum_ntohs(const cord_ipv4_hdr_t *ip, uint16_t checksum);
bool cord_compare_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_src_addr_ntohl(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_dst_addr_ntohl(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_compare_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_src_subnet_ntohl(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_dst_subnet_ntohl(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_compare_ipv4_fragmented(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_fragmented_ntohs(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_first_fragment(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_first_fragment_ntohs(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_last_fragment(const cord_ipv4_hdr_t *ip);
bool cord_compare_ipv4_last_fragment_ntohs(const cord_ipv4_hdr_t *ip);
bool cord_compare_if_ipv4_checksum_valid(const cord_ipv4_hdr_t *ip_hdr);

// L3 IPv6 Match Functions
bool cord_compare_ipv6_version(const cord_ipv6_hdr_t *ip6);
bool cord_compare_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, uint8_t tc);
bool cord_compare_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, uint32_t flow);
bool cord_compare_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, uint16_t length);
bool cord_compare_ipv6_payload_length_ntohs(const cord_ipv6_hdr_t *ip6, uint16_t length);
bool cord_compare_ipv6_next_header(const cord_ipv6_hdr_t *ip6, uint8_t next_hdr);
bool cord_compare_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, uint8_t hop_limit);
bool cord_compare_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_compare_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_compare_ipv6_src_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);
bool cord_compare_ipv6_dst_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);

// L4 TCP Match Functions
bool cord_compare_tcp_src_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_src_port_ntohs(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_dst_port_ntohs(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_compare_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_tcp_port_range_ntohs(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq);
bool cord_compare_tcp_seq_num_ntohl(const cord_tcp_hdr_t *tcp, uint32_t seq);
bool cord_compare_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack);
bool cord_compare_tcp_ack_num_ntohl(const cord_tcp_hdr_t *tcp, uint32_t ack);
bool cord_compare_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset);
bool cord_compare_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window);
bool cord_compare_tcp_window_ntohs(const cord_tcp_hdr_t *tcp, uint16_t window);
bool cord_compare_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum);
bool cord_compare_tcp_checksum_ntohs(const cord_tcp_hdr_t *tcp, uint16_t checksum);
bool cord_compare_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
bool cord_compare_tcp_urgent_ptr_ntohs(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
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
bool cord_compare_udp_src_port_ntohs(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_compare_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_compare_udp_dst_port_ntohs(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_compare_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_udp_port_range_ntohs(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_udp_length(const cord_udp_hdr_t *udp, uint16_t length);
bool cord_compare_udp_length_ntohs(const cord_udp_hdr_t *udp, uint16_t length);
bool cord_compare_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum);
bool cord_compare_udp_checksum_ntohs(const cord_udp_hdr_t *udp, uint16_t checksum);

// L4 SCTP Match Functions
bool cord_compare_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_src_port_ntohs(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_dst_port_ntohs(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_compare_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_sctp_port_range_ntohs(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_compare_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag);
bool cord_compare_sctp_vtag_ntohl(const cord_sctp_hdr_t *sctp, uint32_t vtag);
bool cord_compare_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum);
bool cord_compare_sctp_checksum_ntohl(const cord_sctp_hdr_t *sctp, uint32_t checksum);

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

//
// Set
//

// Ethernet Field Setters
void cord_set_field_eth_dst_addr(cord_eth_hdr_t *eth, const cord_mac_addr_t *dst);
void cord_set_field_eth_src_addr(cord_eth_hdr_t *eth, const cord_mac_addr_t *src);
void cord_set_field_eth_type(cord_eth_hdr_t *eth, uint16_t type);
void cord_set_field_eth_type_htons(cord_eth_hdr_t *eth, uint16_t type);

// VLAN Field Setters
void cord_set_field_vlan_tci(cord_vlan_hdr_t *vlan, uint16_t tci);
void cord_set_field_vlan_tci_htons(cord_vlan_hdr_t *vlan, uint16_t tci);
void cord_set_field_vlan_pcp(cord_vlan_hdr_t *vlan, uint8_t pcp);
void cord_set_field_vlan_pcp_htons(cord_vlan_hdr_t *vlan, uint8_t pcp);
void cord_set_field_vlan_dei(cord_vlan_hdr_t *vlan, uint8_t dei);
void cord_set_field_vlan_dei_htons(cord_vlan_hdr_t *vlan, uint8_t dei);
void cord_set_field_vlan_vid(cord_vlan_hdr_t *vlan, uint16_t vid);
void cord_set_field_vlan_vid_htons(cord_vlan_hdr_t *vlan, uint16_t vid);
void cord_set_field_vlan_type(cord_vlan_hdr_t *vlan, uint16_t type);
void cord_set_field_vlan_type_htons(cord_vlan_hdr_t *vlan, uint16_t type);

// IPv4 Field Setters
void cord_set_field_ipv4_version(cord_ipv4_hdr_t *ip, uint8_t version);
void cord_set_field_ipv4_ihl(cord_ipv4_hdr_t *ip, uint8_t ihl);
void cord_set_field_ipv4_tos(cord_ipv4_hdr_t *ip, uint8_t tos);
void cord_set_field_ipv4_dscp(cord_ipv4_hdr_t *ip, uint8_t dscp);
void cord_set_field_ipv4_ecn(cord_ipv4_hdr_t *ip, uint8_t ecn);
void cord_set_field_ipv4_total_length(cord_ipv4_hdr_t *ip, uint16_t length);
void cord_set_field_ipv4_total_length_htons(cord_ipv4_hdr_t *ip, uint16_t length);
void cord_set_field_ipv4_id(cord_ipv4_hdr_t *ip, uint16_t id);
void cord_set_field_ipv4_id_htons(cord_ipv4_hdr_t *ip, uint16_t id);
void cord_set_field_ipv4_frag_off(cord_ipv4_hdr_t *ip, uint16_t frag_off);
void cord_set_field_ipv4_frag_off_htons(cord_ipv4_hdr_t *ip, uint16_t frag_off);
void cord_set_field_ipv4_ttl(cord_ipv4_hdr_t *ip, uint8_t ttl);
void cord_set_field_ipv4_protocol(cord_ipv4_hdr_t *ip, uint8_t protocol);
void cord_set_field_ipv4_checksum(cord_ipv4_hdr_t *ip, uint16_t checksum);
void cord_set_field_ipv4_checksum_htons(cord_ipv4_hdr_t *ip, uint16_t checksum);
void cord_set_field_ipv4_src_addr(cord_ipv4_hdr_t *ip, uint32_t addr);
void cord_set_field_ipv4_src_addr_htonl(cord_ipv4_hdr_t *ip, uint32_t addr);
void cord_set_field_ipv4_dst_addr(cord_ipv4_hdr_t *ip, uint32_t addr);
void cord_set_field_ipv4_dst_addr_htonl(cord_ipv4_hdr_t *ip, uint32_t addr);

// IPv6 Field Setters
void cord_set_field_ipv6_version(cord_ipv6_hdr_t *ip6, uint8_t version);
void cord_set_field_ipv6_traffic_class(cord_ipv6_hdr_t *ip6, uint8_t tc);
void cord_set_field_ipv6_flow_label(cord_ipv6_hdr_t *ip6, uint32_t flow);
void cord_set_field_ipv6_payload_length(cord_ipv6_hdr_t *ip6, uint16_t length);
void cord_set_field_ipv6_payload_length_htons(cord_ipv6_hdr_t *ip6, uint16_t length);
void cord_set_field_ipv6_next_header(cord_ipv6_hdr_t *ip6, uint8_t next_hdr);
void cord_set_field_ipv6_hop_limit(cord_ipv6_hdr_t *ip6, uint8_t hop_limit);
void cord_set_field_ipv6_src_addr(cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *src);
void cord_set_field_ipv6_dst_addr(cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *dst);

// TCP Field Setters
void cord_set_field_tcp_src_port(cord_tcp_hdr_t *tcp, uint16_t port);
void cord_set_field_tcp_src_port_htons(cord_tcp_hdr_t *tcp, uint16_t port);
void cord_set_field_tcp_dst_port(cord_tcp_hdr_t *tcp, uint16_t port);
void cord_set_field_tcp_dst_port_htons(cord_tcp_hdr_t *tcp, uint16_t port);
void cord_set_field_tcp_seq_num(cord_tcp_hdr_t *tcp, uint32_t seq);
void cord_set_field_tcp_seq_num_htonl(cord_tcp_hdr_t *tcp, uint32_t seq);
void cord_set_field_tcp_ack_num(cord_tcp_hdr_t *tcp, uint32_t ack);
void cord_set_field_tcp_ack_num_htonl(cord_tcp_hdr_t *tcp, uint32_t ack);
void cord_set_field_tcp_doff(cord_tcp_hdr_t *tcp, uint8_t doff);
void cord_set_field_tcp_window(cord_tcp_hdr_t *tcp, uint16_t window);
void cord_set_field_tcp_window_htons(cord_tcp_hdr_t *tcp, uint16_t window);
void cord_set_field_tcp_checksum(cord_tcp_hdr_t *tcp, uint16_t checksum);
void cord_set_field_tcp_checksum_htons(cord_tcp_hdr_t *tcp, uint16_t checksum);
void cord_set_field_tcp_urgent_ptr(cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
void cord_set_field_tcp_urgent_ptr_htons(cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
void cord_set_field_tcp_fin(cord_tcp_hdr_t *tcp, bool fin);
void cord_set_field_tcp_syn(cord_tcp_hdr_t *tcp, bool syn);
void cord_set_field_tcp_rst(cord_tcp_hdr_t *tcp, bool rst);
void cord_set_field_tcp_psh(cord_tcp_hdr_t *tcp, bool psh);
void cord_set_field_tcp_ack(cord_tcp_hdr_t *tcp, bool ack);
void cord_set_field_tcp_urg(cord_tcp_hdr_t *tcp, bool urg);
void cord_set_field_tcp_ece(cord_tcp_hdr_t *tcp, bool ece);
void cord_set_field_tcp_cwr(cord_tcp_hdr_t *tcp, bool cwr);

// UDP Field Setters
void cord_set_field_udp_src_port(cord_udp_hdr_t *udp, uint16_t port);
void cord_set_field_udp_src_port_htons(cord_udp_hdr_t *udp, uint16_t port);
void cord_set_field_udp_dst_port(cord_udp_hdr_t *udp, uint16_t port);
void cord_set_field_udp_dst_port_htons(cord_udp_hdr_t *udp, uint16_t port);
void cord_set_field_udp_length(cord_udp_hdr_t *udp, uint16_t length);
void cord_set_field_udp_length_htons(cord_udp_hdr_t *udp, uint16_t length);
void cord_set_field_udp_checksum(cord_udp_hdr_t *udp, uint16_t checksum);
void cord_set_field_udp_checksum_htons(cord_udp_hdr_t *udp, uint16_t checksum);

// SCTP Field Setters
void cord_set_field_sctp_src_port(cord_sctp_hdr_t *sctp, uint16_t port);
void cord_set_field_sctp_src_port_htons(cord_sctp_hdr_t *sctp, uint16_t port);
void cord_set_field_sctp_dst_port(cord_sctp_hdr_t *sctp, uint16_t port);
void cord_set_field_sctp_dst_port_htons(cord_sctp_hdr_t *sctp, uint16_t port);
void cord_set_field_sctp_vtag(cord_sctp_hdr_t *sctp, uint32_t vtag);
void cord_set_field_sctp_vtag_htonl(cord_sctp_hdr_t *sctp, uint32_t vtag);
void cord_set_field_sctp_checksum(cord_sctp_hdr_t *sctp, uint32_t checksum);
void cord_set_field_sctp_checksum_htonl(cord_sctp_hdr_t *sctp, uint32_t checksum);

// ICMP Field Setters
void cord_set_field_icmp_type(cord_icmp_hdr_t *icmp, uint8_t type);
void cord_set_field_icmp_code(cord_icmp_hdr_t *icmp, uint8_t code);
void cord_set_field_icmp_checksum(cord_icmp_hdr_t *icmp, uint16_t checksum);
void cord_set_field_icmp_checksum_htons(cord_icmp_hdr_t *icmp, uint16_t checksum);
void cord_set_field_icmp_id(cord_icmp_hdr_t *icmp, uint16_t id);
void cord_set_field_icmp_id_htons(cord_icmp_hdr_t *icmp, uint16_t id);
void cord_set_field_icmp_sequence(cord_icmp_hdr_t *icmp, uint16_t sequence);
void cord_set_field_icmp_sequence_htons(cord_icmp_hdr_t *icmp, uint16_t sequence);

//
// Calculate
//

// Ethernet frame CRC32 calculation
uint32_t cord_calculate_ethernet_crc32(const void *buffer, size_t frame_len);

// IPv4 payload length calculation
uint16_t cord_calculate_ipv4_payload_length_ntohs(const cord_ipv4_hdr_t *ip_hdr);

// IPv4 checksum calculation
uint16_t cord_calculate_ipv4_checksum(const cord_ipv4_hdr_t *ip_hdr);

// TCP checksum calculation for IPv4
uint16_t cord_calculate_tcp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr);

// UDP checksum calculation for IPv4
uint16_t cord_calculate_udp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr);

// ICMP checksum calculation for IPv4
uint16_t cord_calculate_icmp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr);

//
// Log
//

// Ethernet Field Loggers
void cord_log_field_eth_dst_addr(const cord_eth_hdr_t *eth, const char *prefix);
void cord_log_field_eth_src_addr(const cord_eth_hdr_t *eth, const char *prefix);
void cord_log_field_eth_type(const cord_eth_hdr_t *eth, const char *prefix);
void cord_log_field_eth_type_ntohs(const cord_eth_hdr_t *eth, const char *prefix);

// VLAN Field Loggers
void cord_log_field_vlan_tci(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_tci_ntohs(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_pcp(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_pcp_ntohs(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_dei(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_dei_ntohs(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_vid(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_vid_ntohs(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_type(const cord_vlan_hdr_t *vlan, const char *prefix);
void cord_log_field_vlan_type_ntohs(const cord_vlan_hdr_t *vlan, const char *prefix);

// IPv4 Field Loggers
void cord_log_field_ipv4_version(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_ihl(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_tos(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_dscp(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_ecn(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_total_length(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_total_length_ntohs(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_header_length(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_id(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_id_ntohs(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_frag_off(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_frag_off_ntohs(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_ttl(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_protocol(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_checksum(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_checksum_ntohs(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_src_addr(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_src_addr_ntohl(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, const char *prefix);
void cord_log_field_ipv4_dst_addr_ntohl(const cord_ipv4_hdr_t *ip, const char *prefix);

// IPv6 Field Loggers
void cord_log_field_ipv6_version(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_version_ntohl(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_traffic_class_ntohl(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_flow_label_ntohl(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_payload_length_ntohs(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_next_header(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const char *prefix);
void cord_log_field_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const char *prefix);

// TCP Field Loggers
void cord_log_field_tcp_src_port(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_src_port_ntohs(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_dst_port(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_dst_port_ntohs(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_seq_num(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_seq_num_ntohl(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_ack_num(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_ack_num_ntohl(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_doff(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_window(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_window_ntohs(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_checksum(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_checksum_ntohs(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_urgent_ptr_ntohs(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_fin(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_syn(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_rst(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_psh(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_ack(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_urg(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_ece(const cord_tcp_hdr_t *tcp, const char *prefix);
void cord_log_field_tcp_cwr(const cord_tcp_hdr_t *tcp, const char *prefix);

// UDP Field Loggers
void cord_log_field_udp_src_port(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_src_port_ntohs(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_dst_port(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_dst_port_ntohs(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_length(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_length_ntohs(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_checksum(const cord_udp_hdr_t *udp, const char *prefix);
void cord_log_field_udp_checksum_ntohs(const cord_udp_hdr_t *udp, const char *prefix);

// SCTP Field Loggers
void cord_log_field_sctp_src_port(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_src_port_ntohs(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_dst_port(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_dst_port_ntohs(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_vtag(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_vtag_ntohl(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_checksum(const cord_sctp_hdr_t *sctp, const char *prefix);
void cord_log_field_sctp_checksum_ntohl(const cord_sctp_hdr_t *sctp, const char *prefix);

// ICMP Field Loggers
void cord_log_field_icmp_type(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_code(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_checksum(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_checksum_ntohs(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_id(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_id_ntohs(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_sequence(const cord_icmp_hdr_t *icmp, const char *prefix);
void cord_log_field_icmp_sequence_ntohs(const cord_icmp_hdr_t *icmp, const char *prefix);

#endif // CORD_ACTION_H