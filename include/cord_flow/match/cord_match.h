#ifndef CORD_MATCH_H
#define CORD_MATCH_H

#include <cord_flow/cord_type.h>

// Protocol Header Getter Functions
static struct ethhdr* cord_get_eth_hdr(const void *buffer);
static struct iphdr* cord_get_ipv4_hdr(const void *buffer);
static struct ip6_hdr* cord_get_ipv6_hdr(const void *buffer);
static struct tcphdr* cord_get_tcp_hdr(const struct iphdr *ip);
static struct tcphdr* cord_get_tcp_hdr_ipv6(const struct ip6_hdr *ip6);
static struct udphdr* cord_get_udp_hdr(const struct iphdr *ip);
static struct udphdr* cord_get_udp_hdr_ipv6(const struct ip6_hdr *ip6);
#ifdef ENABLE_SCTP_PROTOCOL
static struct sctphdr* cord_get_sctp_hdr(const struct iphdr *ip);
static struct sctphdr* cord_get_sctp_hdr_ipv6(const struct ip6_hdr *ip6);
#endif

// Protocol Field Pointer Getters - Ethernet
static uint8_t* cord_get_eth_dst_addr(const struct ethhdr *eth);
static uint8_t* cord_get_eth_src_addr(const struct ethhdr *eth);
static uint16_t* cord_get_eth_type(const struct ethhdr *eth);

// Protocol Field Pointer Getters - IPv4
static uint8_t* cord_get_ipv4_version_ihl(const struct iphdr *ip);
static uint8_t* cord_get_ipv4_tos(const struct iphdr *ip);
static uint16_t* cord_get_ipv4_total_length(const struct iphdr *ip);
static uint16_t* cord_get_ipv4_id(const struct iphdr *ip);
static uint16_t* cord_get_ipv4_frag_off(const struct iphdr *ip);
static uint8_t* cord_get_ipv4_ttl(const struct iphdr *ip);
static uint8_t* cord_get_ipv4_protocol(const struct iphdr *ip);
static uint16_t* cord_get_ipv4_checksum(const struct iphdr *ip);
static uint32_t* cord_get_ipv4_src_addr(const struct iphdr *ip);
static uint32_t* cord_get_ipv4_dst_addr(const struct iphdr *ip);

// Protocol Field Pointer Getters - IPv6
static uint32_t* cord_get_ipv6_vtc_flow(const struct ip6_hdr *ip6);
static uint16_t* cord_get_ipv6_payload_length(const struct ip6_hdr *ip6);
static uint8_t* cord_get_ipv6_next_header(const struct ip6_hdr *ip6);
static uint8_t* cord_get_ipv6_hop_limit(const struct ip6_hdr *ip6);
static struct in6_addr* cord_get_ipv6_src_addr(const struct ip6_hdr *ip6);
static struct in6_addr* cord_get_ipv6_dst_addr(const struct ip6_hdr *ip6);

// Protocol Field Pointer Getters - TCP
static uint16_t* cord_get_tcp_src_port(const struct tcphdr *tcp);
static uint16_t* cord_get_tcp_dst_port(const struct tcphdr *tcp);
static uint32_t* cord_get_tcp_seq_num(const struct tcphdr *tcp);
static uint32_t* cord_get_tcp_ack_num(const struct tcphdr *tcp);
static uint8_t* cord_get_tcp_data_offset_flags(const struct tcphdr *tcp);
static uint16_t* cord_get_tcp_window(const struct tcphdr *tcp);
static uint16_t* cord_get_tcp_checksum(const struct tcphdr *tcp);
static uint16_t* cord_get_tcp_urgent_ptr(const struct tcphdr *tcp);

// Protocol Field Pointer Getters - UDP
static uint16_t* cord_get_udp_src_port(const struct udphdr *udp);
static uint16_t* cord_get_udp_dst_port(const struct udphdr *udp);
static uint16_t* cord_get_udp_length(const struct udphdr *udp);
static uint16_t* cord_get_udp_checksum(const struct udphdr *udp);

#ifdef ENABLE_SCTP_PROTOCOL
// Protocol Field Pointer Getters - SCTP
static uint16_t* cord_get_sctp_src_port(const struct sctphdr *sctp);
static uint16_t* cord_get_sctp_dst_port(const struct sctphdr *sctp);
static uint32_t* cord_get_sctp_vtag(const struct sctphdr *sctp);
static uint32_t* cord_get_sctp_checksum(const struct sctphdr *sctp);
#endif

// L2 Ethernet Match Functions
static bool cord_match_eth_dst_addr(const struct ethhdr *eth, const uint8_t *addr);
static bool cord_match_eth_src_addr(const struct ethhdr *eth, const uint8_t *addr);
static bool cord_match_eth_type(const struct ethhdr *eth, uint16_t eth_type);
static bool cord_match_eth_broadcast(const struct ethhdr *eth);
static bool cord_match_eth_multicast(const struct ethhdr *eth);
static bool cord_match_eth_unicast(const struct ethhdr *eth);
static bool cord_match_eth_crc(const void *buffer, size_t frame_len, uint32_t expected_crc);

// L3 IPv4 Match Functions
static bool cord_match_ipv4_version(const struct iphdr *ip);
static bool cord_match_ipv4_ihl(const struct iphdr *ip, uint8_t ihl);
static bool cord_match_ipv4_tos(const struct iphdr *ip, uint8_t tos);
static bool cord_match_ipv4_dscp(const struct iphdr *ip, uint8_t dscp);
static bool cord_match_ipv4_ecn(const struct iphdr *ip, uint8_t ecn);
static bool cord_match_ipv4_total_length(const struct iphdr *ip, uint16_t length);
static bool cord_match_ipv4_id(const struct iphdr *ip, uint16_t id);
static bool cord_match_ipv4_flags(const struct iphdr *ip, uint16_t flags);
static bool cord_match_ipv4_frag_offset(const struct iphdr *ip, uint16_t offset);
static bool cord_match_ipv4_ttl(const struct iphdr *ip, uint8_t ttl);
static bool cord_match_ipv4_protocol(const struct iphdr *ip, uint8_t protocol);
static bool cord_match_ipv4_checksum(const struct iphdr *ip, uint16_t checksum);
static bool cord_match_ipv4_src_addr(const struct iphdr *ip, uint32_t addr);
static bool cord_match_ipv4_dst_addr(const struct iphdr *ip, uint32_t addr);
static bool cord_match_ipv4_src_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask);
static bool cord_match_ipv4_dst_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask);

// L3 IPv6 Match Functions
static bool cord_match_ipv6_version(const struct ip6_hdr *ip6);
static bool cord_match_ipv6_traffic_class(const struct ip6_hdr *ip6, uint8_t tc);
static bool cord_match_ipv6_flow_label(const struct ip6_hdr *ip6, uint32_t flow);
static bool cord_match_ipv6_payload_length(const struct ip6_hdr *ip6, uint16_t length);
static bool cord_match_ipv6_next_header(const struct ip6_hdr *ip6, uint8_t next_hdr);
static bool cord_match_ipv6_hop_limit(const struct ip6_hdr *ip6, uint8_t hop_limit);
static bool cord_match_ipv6_src_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr);
static bool cord_match_ipv6_dst_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr);
static bool cord_match_ipv6_src_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len);
static bool cord_match_ipv6_dst_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len);

// L4 TCP Match Functions
static bool cord_match_tcp_src_port(const struct tcphdr *tcp, uint16_t port);
static bool cord_match_tcp_dst_port(const struct tcphdr *tcp, uint16_t port);
static bool cord_match_tcp_port_range(const struct tcphdr *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
static bool cord_match_tcp_seq_num(const struct tcphdr *tcp, uint32_t seq);
static bool cord_match_tcp_ack_num(const struct tcphdr *tcp, uint32_t ack);
static bool cord_match_tcp_data_offset(const struct tcphdr *tcp, uint8_t offset);
static bool cord_match_tcp_flags(const struct tcphdr *tcp, uint8_t flags);
static bool cord_match_tcp_window(const struct tcphdr *tcp, uint16_t window);
static bool cord_match_tcp_checksum(const struct tcphdr *tcp, uint16_t checksum);
static bool cord_match_tcp_urgent_ptr(const struct tcphdr *tcp, uint16_t urg_ptr);
static bool cord_match_tcp_syn(const struct tcphdr *tcp);
static bool cord_match_tcp_ack(const struct tcphdr *tcp);
static bool cord_match_tcp_fin(const struct tcphdr *tcp);
static bool cord_match_tcp_rst(const struct tcphdr *tcp);
static bool cord_match_tcp_psh(const struct tcphdr *tcp);
static bool cord_match_tcp_urg(const struct tcphdr *tcp);

// L4 UDP Match Functions
static bool cord_match_udp_src_port(const struct udphdr *udp, uint16_t port);
static bool cord_match_udp_dst_port(const struct udphdr *udp, uint16_t port);
static bool cord_match_udp_port_range(const struct udphdr *udp, uint16_t min_port, uint16_t max_port, bool check_src);
static bool cord_match_udp_length(const struct udphdr *udp, uint16_t length);
static bool cord_match_udp_checksum(const struct udphdr *udp, uint16_t checksum);

#ifdef ENABLE_SCTP_PROTOCOL
// L4 SCTP Match Functions  
static bool cord_match_sctp_src_port(const struct sctphdr *sctp, uint16_t port);
static bool cord_match_sctp_dst_port(const struct sctphdr *sctp, uint16_t port);
static bool cord_match_sctp_port_range(const struct sctphdr *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
static bool cord_match_sctp_vtag(const struct sctphdr *sctp, uint32_t vtag);
static bool cord_match_sctp_checksum(const struct sctphdr *sctp, uint32_t checksum);
#endif

#endif // CORD_MATCH_H