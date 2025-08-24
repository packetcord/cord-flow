#ifndef CORD_MATCH_H
#define CORD_MATCH_H

#include <cord_type.h>

// Protocol Header Getter Functions
struct ethhdr* cord_get_eth_hdr(const void *buffer);
struct iphdr* cord_get_ipv4_hdr(const void *buffer);
struct ip6_hdr* cord_get_ipv6_hdr(const void *buffer);
struct tcphdr* cord_get_tcp_hdr(const struct iphdr *ip);
struct tcphdr* cord_get_tcp_hdr_ipv6(const struct ip6_hdr *ip6);
struct udphdr* cord_get_udp_hdr(const struct iphdr *ip);
struct udphdr* cord_get_udp_hdr_ipv6(const struct ip6_hdr *ip6);
#ifdef ENABLE_SCTP_PROTOCOL
struct sctphdr* cord_get_sctp_hdr(const struct iphdr *ip);
struct sctphdr* cord_get_sctp_hdr_ipv6(const struct ip6_hdr *ip6);
#endif

// Protocol Field Value Getters - Ethernet
void cord_get_eth_dst_addr(const struct ethhdr *eth, uint8_t *dst);
void cord_get_eth_src_addr(const struct ethhdr *eth, uint8_t *src);
uint16_t cord_get_eth_type_value(const struct ethhdr *eth);

// Protocol Field Value Getters - IPv4
uint8_t cord_get_ipv4_version_value(const struct iphdr *ip);
uint8_t cord_get_ipv4_ihl_value(const struct iphdr *ip);
uint8_t cord_get_ipv4_tos_value(const struct iphdr *ip);
uint16_t cord_get_ipv4_total_length_value(const struct iphdr *ip);
uint16_t cord_get_ipv4_id_value(const struct iphdr *ip);
uint16_t cord_get_ipv4_frag_off_value(const struct iphdr *ip);
uint8_t cord_get_ipv4_ttl_value(const struct iphdr *ip);
uint8_t cord_get_ipv4_protocol_value(const struct iphdr *ip);
uint16_t cord_get_ipv4_checksum_value(const struct iphdr *ip);
uint32_t cord_get_ipv4_src_addr_value(const struct iphdr *ip);
uint32_t cord_get_ipv4_dst_addr_value(const struct iphdr *ip);

// Protocol Field Value Getters - IPv6
uint32_t cord_get_ipv6_vtc_flow_value(const struct ip6_hdr *ip6);
uint16_t cord_get_ipv6_payload_length_value(const struct ip6_hdr *ip6);
uint8_t cord_get_ipv6_next_header_value(const struct ip6_hdr *ip6);
uint8_t cord_get_ipv6_hop_limit_value(const struct ip6_hdr *ip6);
void cord_get_ipv6_src_addr(const struct ip6_hdr *ip6, struct in6_addr *dst);
void cord_get_ipv6_dst_addr(const struct ip6_hdr *ip6, struct in6_addr *dst);

// Protocol Field Value Getters - TCP
uint16_t cord_get_tcp_src_port_value(const struct tcphdr *tcp);
uint16_t cord_get_tcp_dst_port_value(const struct tcphdr *tcp);
uint32_t cord_get_tcp_seq_num_value(const struct tcphdr *tcp);
uint32_t cord_get_tcp_ack_num_value(const struct tcphdr *tcp);
uint8_t cord_get_tcp_doff_value(const struct tcphdr *tcp);
uint16_t cord_get_tcp_window_value(const struct tcphdr *tcp);
uint16_t cord_get_tcp_checksum_value(const struct tcphdr *tcp);
uint16_t cord_get_tcp_urgent_ptr_value(const struct tcphdr *tcp);

// Protocol Field Value Getters - UDP
uint16_t cord_get_udp_src_port_value(const struct udphdr *udp);
uint16_t cord_get_udp_dst_port_value(const struct udphdr *udp);
uint16_t cord_get_udp_length_value(const struct udphdr *udp);
uint16_t cord_get_udp_checksum_value(const struct udphdr *udp);

#ifdef ENABLE_SCTP_PROTOCOL
// Protocol Field Value Getters - SCTP
uint16_t cord_get_sctp_src_port_value(const struct sctphdr *sctp);
uint16_t cord_get_sctp_dst_port_value(const struct sctphdr *sctp);
uint32_t cord_get_sctp_vtag_value(const struct sctphdr *sctp);
uint32_t cord_get_sctp_checksum_value(const struct sctphdr *sctp);
#endif

// L2 Ethernet Match Functions
bool cord_match_eth_dst_addr(const struct ethhdr *eth, const uint8_t *addr);
bool cord_match_eth_src_addr(const struct ethhdr *eth, const uint8_t *addr);
bool cord_match_eth_type(const struct ethhdr *eth, uint16_t eth_type);
bool cord_match_eth_broadcast(const struct ethhdr *eth);
bool cord_match_eth_multicast(const struct ethhdr *eth);
bool cord_match_eth_unicast(const struct ethhdr *eth);
bool cord_match_eth_crc(const void *buffer, size_t frame_len, uint32_t expected_crc);

// L3 IPv4 Match Functions
bool cord_match_ipv4_version(const struct iphdr *ip);
bool cord_match_ipv4_ihl(const struct iphdr *ip, uint8_t ihl);
bool cord_match_ipv4_tos(const struct iphdr *ip, uint8_t tos);
bool cord_match_ipv4_dscp(const struct iphdr *ip, uint8_t dscp);
bool cord_match_ipv4_ecn(const struct iphdr *ip, uint8_t ecn);
bool cord_match_ipv4_total_length(const struct iphdr *ip, uint16_t length);
bool cord_match_ipv4_id(const struct iphdr *ip, uint16_t id);
bool cord_match_ipv4_flags(const struct iphdr *ip, uint16_t flags);
bool cord_match_ipv4_frag_offset(const struct iphdr *ip, uint16_t offset);
bool cord_match_ipv4_ttl(const struct iphdr *ip, uint8_t ttl);
bool cord_match_ipv4_protocol(const struct iphdr *ip, uint8_t protocol);
bool cord_match_ipv4_checksum(const struct iphdr *ip, uint16_t checksum);
bool cord_match_ipv4_src_addr(const struct iphdr *ip, uint32_t addr);
bool cord_match_ipv4_dst_addr(const struct iphdr *ip, uint32_t addr);
bool cord_match_ipv4_src_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask);
bool cord_match_ipv4_dst_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask);

// L3 IPv6 Match Functions
bool cord_match_ipv6_version(const struct ip6_hdr *ip6);
bool cord_match_ipv6_traffic_class(const struct ip6_hdr *ip6, uint8_t tc);
bool cord_match_ipv6_flow_label(const struct ip6_hdr *ip6, uint32_t flow);
bool cord_match_ipv6_payload_length(const struct ip6_hdr *ip6, uint16_t length);
bool cord_match_ipv6_next_header(const struct ip6_hdr *ip6, uint8_t next_hdr);
bool cord_match_ipv6_hop_limit(const struct ip6_hdr *ip6, uint8_t hop_limit);
bool cord_match_ipv6_src_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr);
bool cord_match_ipv6_dst_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr);
bool cord_match_ipv6_src_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len);
bool cord_match_ipv6_dst_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len);

// L4 TCP Match Functions
bool cord_match_tcp_src_port(const struct tcphdr *tcp, uint16_t port);
bool cord_match_tcp_dst_port(const struct tcphdr *tcp, uint16_t port);
bool cord_match_tcp_port_range(const struct tcphdr *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_tcp_seq_num(const struct tcphdr *tcp, uint32_t seq);
bool cord_match_tcp_ack_num(const struct tcphdr *tcp, uint32_t ack);
bool cord_match_tcp_data_offset(const struct tcphdr *tcp, uint8_t offset);
bool cord_match_tcp_flags(const struct tcphdr *tcp, uint8_t flags);
bool cord_match_tcp_window(const struct tcphdr *tcp, uint16_t window);
bool cord_match_tcp_checksum(const struct tcphdr *tcp, uint16_t checksum);
bool cord_match_tcp_urgent_ptr(const struct tcphdr *tcp, uint16_t urg_ptr);
bool cord_match_tcp_syn(const struct tcphdr *tcp);
bool cord_match_tcp_ack(const struct tcphdr *tcp);
bool cord_match_tcp_fin(const struct tcphdr *tcp);
bool cord_match_tcp_rst(const struct tcphdr *tcp);
bool cord_match_tcp_psh(const struct tcphdr *tcp);
bool cord_match_tcp_urg(const struct tcphdr *tcp);

// L4 UDP Match Functions
bool cord_match_udp_src_port(const struct udphdr *udp, uint16_t port);
bool cord_match_udp_dst_port(const struct udphdr *udp, uint16_t port);
bool cord_match_udp_port_range(const struct udphdr *udp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_udp_length(const struct udphdr *udp, uint16_t length);
bool cord_match_udp_checksum(const struct udphdr *udp, uint16_t checksum);

#ifdef ENABLE_SCTP_PROTOCOL
// L4 SCTP Match Functions  
bool cord_match_sctp_src_port(const struct sctphdr *sctp, uint16_t port);
bool cord_match_sctp_dst_port(const struct sctphdr *sctp, uint16_t port);
bool cord_match_sctp_port_range(const struct sctphdr *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_sctp_vtag(const struct sctphdr *sctp, uint32_t vtag);
bool cord_match_sctp_checksum(const struct sctphdr *sctp, uint32_t checksum);
#endif

#endif // CORD_MATCH_H