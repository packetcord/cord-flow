#ifndef CORD_MATCH_H
#define CORD_MATCH_H

#include <cord_type.h>
#include <protocol_headers/cord_protocol_headers.h>

/**
 * @file cord_match.h
 * @brief Zero-copy protocol header parsing and matching functions
 * 
 * This module provides high-performance, zero-copy protocol header parsing
 * and field matching functions. All functions operate directly on packet
 * buffers without copying data.
 * 
 * Key principles:
 * - Zero-copy: All functions work with pointers to packet data
 * - Portable: Uses our own protocol header definitions
 * - High-performance: Optimized for packet processing pipelines
 * - Type-safe: Uses proper portable types and byte order handling
 */

// =============================================================================
// PROTOCOL HEADER GETTER FUNCTIONS - Zero Copy
// =============================================================================

// Layer 2 Protocol Headers
cord_eth_hdr_t* cord_get_eth_hdr(const void *buffer);
cord_vlan_hdr_t* cord_get_vlan_hdr(const cord_eth_hdr_t *eth_hdr);
cord_mpls_hdr_t* cord_get_mpls_hdr(const void *buffer, uint16_t offset);
cord_arp_hdr_t* cord_get_arp_hdr(const cord_eth_hdr_t *eth_hdr);

// Layer 3 Protocol Headers  
cord_ipv4_hdr_t* cord_get_ipv4_hdr(const void *buffer);
cord_ipv4_hdr_t* cord_get_ipv4_hdr_from_eth(const cord_eth_hdr_t *eth_hdr);
cord_ipv6_hdr_t* cord_get_ipv6_hdr(const void *buffer);
cord_ipv6_hdr_t* cord_get_ipv6_hdr_from_eth(const cord_eth_hdr_t *eth_hdr);
cord_icmp_hdr_t* cord_get_icmp_hdr(const cord_ipv4_hdr_t *ip_hdr);
cord_icmpv6_hdr_t* cord_get_icmpv6_hdr(const cord_ipv6_hdr_t *ip6_hdr);

// Layer 4 Protocol Headers
cord_tcp_hdr_t* cord_get_tcp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_tcp_hdr_t* cord_get_tcp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr);
cord_udp_hdr_t* cord_get_udp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_udp_hdr_t* cord_get_udp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr);
cord_sctp_hdr_t* cord_get_sctp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_sctp_hdr_t* cord_get_sctp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr);

// Tunneling Protocol Headers
cord_gre_hdr_t* cord_get_gre_hdr(const cord_ipv4_hdr_t *ip_hdr);
cord_vxlan_hdr_t* cord_get_vxlan_hdr(const cord_udp_hdr_t *udp_hdr);
cord_gtpu_hdr_t* cord_get_gtpu_hdr(const cord_udp_hdr_t *udp_hdr);

// VoIP Protocol Headers
cord_rtp_hdr_t* cord_get_rtp_hdr(const cord_udp_hdr_t *udp_hdr);
cord_rtcp_hdr_t* cord_get_rtcp_hdr(const cord_udp_hdr_t *udp_hdr);

// Routing Protocol Headers
cord_ospf_hdr_t* cord_get_ospf_hdr(const cord_ipv4_hdr_t *ip_hdr);
cord_ospf_hello_t* cord_get_ospf_hello(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_db_desc_t* cord_get_ospf_db_desc(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_req_t* cord_get_ospf_ls_req(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_upd_t* cord_get_ospf_ls_upd(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_ack_t* cord_get_ospf_ls_ack(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_lsa_hdr_t* cord_get_ospf_lsa_hdr(const void *lsa_data);
cord_ospf_router_lsa_t* cord_get_ospf_router_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_network_lsa_t* cord_get_ospf_network_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_summary_lsa_t* cord_get_ospf_summary_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_external_lsa_t* cord_get_ospf_external_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_nssa_lsa_t* cord_get_ospf_nssa_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_opaque_lsa_t* cord_get_ospf_opaque_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);

// BGP Protocol Headers
cord_bgp_hdr_t* cord_get_bgp_hdr(const cord_tcp_hdr_t *tcp_hdr);
cord_bgp_open_t* cord_get_bgp_open(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_update_t* cord_get_bgp_update(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_notification_t* cord_get_bgp_notification(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_keepalive_t* cord_get_bgp_keepalive(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_path_attr_t* cord_get_bgp_path_attr(const void *attr_data);
cord_bgp_origin_attr_t* cord_get_bgp_origin_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_as_path_attr_t* cord_get_bgp_as_path_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_next_hop_attr_t* cord_get_bgp_next_hop_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_med_attr_t* cord_get_bgp_med_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_local_pref_attr_t* cord_get_bgp_local_pref_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_communities_attr_t* cord_get_bgp_communities_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_mp_reach_attr_t* cord_get_bgp_mp_reach_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_mp_unreach_attr_t* cord_get_bgp_mp_unreach_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_extended_communities_attr_t* cord_get_bgp_extended_communities_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_large_communities_attr_t* cord_get_bgp_large_communities_attr(const cord_bgp_path_attr_t *attr);

// RIP Protocol Headers
cord_rip_hdr_t* cord_get_rip_hdr(const cord_udp_hdr_t *udp_hdr);
cord_rip_msg_t* cord_get_rip_msg(const cord_udp_hdr_t *udp_hdr);
cord_rip_v1_entry_t* cord_get_rip_v1_entry(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_rip_v2_entry_t* cord_get_rip_v2_entry(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_rip_v2_auth_t* cord_get_rip_v2_auth(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_ripng_hdr_t* cord_get_ripng_hdr(const cord_udp_hdr_t *udp_hdr);
cord_ripng_entry_t* cord_get_ripng_entry(const cord_ripng_hdr_t *ripng_hdr, uint16_t index);

// IS-IS Protocol Headers
cord_isis_common_hdr_t* cord_get_isis_common_hdr(const void *buffer);
cord_isis_p2p_hello_t* cord_get_isis_p2p_hello(const cord_isis_common_hdr_t *common_hdr);
cord_isis_lan_hello_t* cord_get_isis_lan_hello(const cord_isis_common_hdr_t *common_hdr);
cord_isis_lsp_t* cord_get_isis_lsp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_csnp_t* cord_get_isis_csnp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_psnp_t* cord_get_isis_psnp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_tlv_t* cord_get_isis_tlv(const void *tlv_data);
cord_isis_area_addr_tlv_t* cord_get_isis_area_addr_tlv(const cord_isis_tlv_t *tlv);
cord_isis_iis_neighbors_tlv_t* cord_get_isis_iis_neighbors_tlv(const cord_isis_tlv_t *tlv);
cord_isis_auth_tlv_t* cord_get_isis_auth_tlv(const cord_isis_tlv_t *tlv);
cord_isis_lsp_entries_tlv_t* cord_get_isis_lsp_entries_tlv(const cord_isis_tlv_t *tlv);
cord_isis_extended_is_reach_tlv_t* cord_get_isis_extended_is_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ip_internal_reach_tlv_t* cord_get_isis_ip_internal_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ip_external_reach_tlv_t* cord_get_isis_ip_external_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_extended_ip_reach_tlv_t* cord_get_isis_extended_ip_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ipv6_reach_tlv_t* cord_get_isis_ipv6_reach_tlv(const cord_isis_tlv_t *tlv);

// EIGRP Protocol Headers
cord_eigrp_hdr_t* cord_get_eigrp_hdr(const cord_ipv4_hdr_t *ip_hdr);
cord_eigrp_tlv_t* cord_get_eigrp_tlv(const void *tlv_data);

// PIM Protocol Headers
cord_pim_hdr_t* cord_get_pim_hdr(const cord_ipv4_hdr_t *ip_hdr);

// First Hop Redundancy Protocol Headers
cord_hsrp_hdr_t* cord_get_hsrp_hdr(const cord_udp_hdr_t *udp_hdr);
cord_vrrp_hdr_t* cord_get_vrrp_hdr(const cord_ipv4_hdr_t *ip_hdr);

// Network Management Protocol Headers
cord_bfd_hdr_t* cord_get_bfd_hdr(const cord_udp_hdr_t *udp_hdr);
cord_ldp_hdr_t* cord_get_ldp_hdr(const cord_tcp_hdr_t *tcp_hdr);
cord_rsvp_hdr_t* cord_get_rsvp_hdr(const cord_ipv4_hdr_t *ip_hdr);

// IGMP Protocol Headers
cord_igmpv3_query_t* cord_get_igmpv3_query(const cord_ipv4_hdr_t *ip_hdr);

// DHCP Protocol Headers
cord_dhcp_hdr_t* cord_get_dhcp_hdr(const cord_udp_hdr_t *udp_hdr);
cord_dhcp_option_t* cord_get_dhcp_option(const cord_dhcp_hdr_t *dhcp_hdr, uint16_t offset);
cord_dhcpv6_hdr_t* cord_get_dhcpv6_hdr(const cord_udp_hdr_t *udp_hdr);
cord_dhcpv6_relay_hdr_t* cord_get_dhcpv6_relay_hdr(const cord_udp_hdr_t *udp_hdr);
cord_dhcpv6_option_t* cord_get_dhcpv6_option(const void *options_data, uint16_t offset);

// IPv6 Neighbor Discovery Protocol Headers
cord_ipv6_nd_router_solicit_t* cord_get_ipv6_nd_router_solicit(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_router_advert_t* cord_get_ipv6_nd_router_advert(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_neighbor_solicit_t* cord_get_ipv6_nd_neighbor_solicit(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_neighbor_advert_t* cord_get_ipv6_nd_neighbor_advert(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_redirect_t* cord_get_ipv6_nd_redirect(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_opt_t* cord_get_ipv6_nd_option(const void *options_data, uint16_t offset);
cord_ipv6_nd_opt_lladdr_t* cord_get_ipv6_nd_opt_lladdr(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_prefix_info_t* cord_get_ipv6_nd_opt_prefix_info(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_mtu_t* cord_get_ipv6_nd_opt_mtu(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_rdnss_t* cord_get_ipv6_nd_opt_rdnss(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_dnssl_t* cord_get_ipv6_nd_opt_dnssl(const cord_ipv6_nd_opt_t *opt);

// =============================================================================
// PROTOCOL FIELD GETTERS - Network Byte Order Handled
// =============================================================================

// Ethernet Field Getters
void cord_get_eth_dst_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *dst);
void cord_get_eth_src_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *src);
uint16_t cord_get_eth_type(const cord_eth_hdr_t *eth);

// VLAN Field Getters
uint8_t cord_get_vlan_pcp(const cord_vlan_hdr_t *vlan);
uint8_t cord_get_vlan_dei(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_vlan_vid(const cord_vlan_hdr_t *vlan);

// IPv4 Field Getters
uint8_t cord_get_ipv4_version(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_ihl(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_tos(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_dscp(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_ecn(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_ipv4_total_length(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_ipv4_id(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_ipv4_frag_off(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_ttl(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_ipv4_protocol(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_ipv4_checksum(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_ipv4_src_addr(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_ipv4_dst_addr(const cord_ipv4_hdr_t *ip);

// IPv6 Field Getters  
uint8_t cord_get_ipv6_version(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6);
uint32_t cord_get_ipv6_flow_label(const cord_ipv6_hdr_t *ip6);
uint16_t cord_get_ipv6_payload_length(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_ipv6_next_header(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6);
void cord_get_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *src);
void cord_get_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *dst);

// TCP Field Getters
uint16_t cord_get_tcp_src_port(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_tcp_dst_port(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_tcp_seq_num(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_tcp_ack_num(const cord_tcp_hdr_t *tcp);
uint8_t cord_get_tcp_doff(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_tcp_window(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_tcp_checksum(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_fin(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_syn(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_rst(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_psh(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_ack(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_urg(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_ece(const cord_tcp_hdr_t *tcp);
bool cord_get_tcp_cwr(const cord_tcp_hdr_t *tcp);

// UDP Field Getters
uint16_t cord_get_udp_src_port(const cord_udp_hdr_t *udp);
uint16_t cord_get_udp_dst_port(const cord_udp_hdr_t *udp);
uint16_t cord_get_udp_length(const cord_udp_hdr_t *udp);
uint16_t cord_get_udp_checksum(const cord_udp_hdr_t *udp);

// SCTP Field Getters
uint16_t cord_get_sctp_src_port(const cord_sctp_hdr_t *sctp);
uint16_t cord_get_sctp_dst_port(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_sctp_vtag(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_sctp_checksum(const cord_sctp_hdr_t *sctp);

// ICMP Field Getters
uint8_t cord_get_icmp_type(const cord_icmp_hdr_t *icmp);
uint8_t cord_get_icmp_code(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_icmp_checksum(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_icmp_id(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_icmp_sequence(const cord_icmp_hdr_t *icmp);

// =============================================================================
// PROTOCOL FIELD MATCHING FUNCTIONS
// =============================================================================

// L2 Ethernet Match Functions
bool cord_match_eth_dst_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr);
bool cord_match_eth_src_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr);
bool cord_match_eth_type(const cord_eth_hdr_t *eth, uint16_t eth_type);
bool cord_match_eth_broadcast(const cord_eth_hdr_t *eth);
bool cord_match_eth_multicast(const cord_eth_hdr_t *eth);
bool cord_match_eth_unicast(const cord_eth_hdr_t *eth);

// VLAN Match Functions
bool cord_match_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp);
bool cord_match_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei);
bool cord_match_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid);
bool cord_match_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid);

// L3 IPv4 Match Functions
bool cord_match_ipv4_version(const cord_ipv4_hdr_t *ip);
bool cord_match_ipv4_ihl(const cord_ipv4_hdr_t *ip, uint8_t ihl);
bool cord_match_ipv4_tos(const cord_ipv4_hdr_t *ip, uint8_t tos);
bool cord_match_ipv4_dscp(const cord_ipv4_hdr_t *ip, uint8_t dscp);
bool cord_match_ipv4_ecn(const cord_ipv4_hdr_t *ip, uint8_t ecn);
bool cord_match_ipv4_total_length(const cord_ipv4_hdr_t *ip, uint16_t length);
bool cord_match_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id);
bool cord_match_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags);
bool cord_match_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset);
bool cord_match_ipv4_ttl(const cord_ipv4_hdr_t *ip, uint8_t ttl);
bool cord_match_ipv4_protocol(const cord_ipv4_hdr_t *ip, uint8_t protocol);
bool cord_match_ipv4_checksum(const cord_ipv4_hdr_t *ip, uint16_t checksum);
bool cord_match_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_match_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr);
bool cord_match_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_match_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask);
bool cord_match_ipv4_fragmented(const cord_ipv4_hdr_t *ip);
bool cord_match_ipv4_first_fragment(const cord_ipv4_hdr_t *ip);
bool cord_match_ipv4_last_fragment(const cord_ipv4_hdr_t *ip);

// L3 IPv6 Match Functions
bool cord_match_ipv6_version(const cord_ipv6_hdr_t *ip6);
bool cord_match_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, uint8_t tc);
bool cord_match_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, uint32_t flow);
bool cord_match_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, uint16_t length);
bool cord_match_ipv6_next_header(const cord_ipv6_hdr_t *ip6, uint8_t next_hdr);
bool cord_match_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, uint8_t hop_limit);
bool cord_match_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_match_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr);
bool cord_match_ipv6_src_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);
bool cord_match_ipv6_dst_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len);

// L4 TCP Match Functions
bool cord_match_tcp_src_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_match_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port);
bool cord_match_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq);
bool cord_match_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack);
bool cord_match_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset);
bool cord_match_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window);
bool cord_match_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum);
bool cord_match_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr);
bool cord_match_tcp_syn(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_ack(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_fin(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_rst(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_psh(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_urg(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_ece(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_cwr(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_established(const cord_tcp_hdr_t *tcp);
bool cord_match_tcp_connection_request(const cord_tcp_hdr_t *tcp);

// L4 UDP Match Functions
bool cord_match_udp_src_port(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_match_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port);
bool cord_match_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_udp_length(const cord_udp_hdr_t *udp, uint16_t length);
bool cord_match_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum);

// L4 SCTP Match Functions  
bool cord_match_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_match_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port);
bool cord_match_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src);
bool cord_match_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag);
bool cord_match_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum);

// ICMP Match Functions
bool cord_match_icmp_type(const cord_icmp_hdr_t *icmp, uint8_t type);
bool cord_match_icmp_code(const cord_icmp_hdr_t *icmp, uint8_t code);
bool cord_match_icmp_echo_request(const cord_icmp_hdr_t *icmp);
bool cord_match_icmp_echo_reply(const cord_icmp_hdr_t *icmp);
bool cord_match_icmp_dest_unreachable(const cord_icmp_hdr_t *icmp);

// Tunneling Protocol Match Functions
bool cord_match_gre_checksum_present(const cord_gre_hdr_t *gre);
bool cord_match_gre_key_present(const cord_gre_hdr_t *gre);
bool cord_match_gre_sequence_present(const cord_gre_hdr_t *gre);
bool cord_match_gre_protocol(const cord_gre_hdr_t *gre, uint16_t protocol);

bool cord_match_vxlan_vni(const cord_vxlan_hdr_t *vxlan, uint32_t vni);
bool cord_match_vxlan_flags(const cord_vxlan_hdr_t *vxlan, uint8_t flags);

bool cord_match_gtpu_teid(const cord_gtpu_hdr_t *gtpu, uint32_t teid);
bool cord_match_gtpu_msg_type(const cord_gtpu_hdr_t *gtpu, uint8_t msg_type);

// VoIP Protocol Match Functions  
bool cord_match_rtp_version(const cord_rtp_hdr_t *rtp, uint8_t version);
bool cord_match_rtp_payload_type(const cord_rtp_hdr_t *rtp, uint8_t pt);
bool cord_match_rtp_ssrc(const cord_rtp_hdr_t *rtp, uint32_t ssrc);
bool cord_match_rtp_marker(const cord_rtp_hdr_t *rtp);

bool cord_match_rtcp_packet_type(const cord_rtcp_hdr_t *rtcp, uint8_t type);
bool cord_match_rtcp_version(const cord_rtcp_hdr_t *rtcp, uint8_t version);

// Advanced Protocol Analysis Functions
bool cord_match_is_dns_query(const cord_udp_hdr_t *udp);
bool cord_match_is_dns_response(const cord_udp_hdr_t *udp);
bool cord_match_is_dhcp_request(const cord_udp_hdr_t *udp);
bool cord_match_is_dhcp_response(const cord_udp_hdr_t *udp);
bool cord_match_is_http_request(const cord_tcp_hdr_t *tcp);
bool cord_match_is_http_response(const cord_tcp_hdr_t *tcp);
bool cord_match_is_https_traffic(const cord_tcp_hdr_t *tcp);
bool cord_match_is_ssh_traffic(const cord_tcp_hdr_t *tcp);

// =============================================================================
// PERFORMANCE UTILITIES
// =============================================================================

// Fast protocol detection (single pass through packet)
typedef struct cord_protocol_info {
    uint16_t eth_type;
    uint8_t ip_version;
    uint8_t ip_protocol;
    uint16_t l4_src_port;
    uint16_t l4_dst_port;
    uint32_t l3_src_addr;
    uint32_t l3_dst_addr;
    bool has_vlan;
    uint16_t vlan_vid;
    bool is_fragment;
    uint16_t payload_len;
} cord_protocol_info_t;

// Single function to extract all common protocol information
bool cord_match_extract_protocol_info(const void *buffer, size_t len, cord_protocol_info_t *info);

// Fast 5-tuple extraction for flow identification
typedef struct cord_flow_tuple {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;
} cord_flow_tuple_t;

bool cord_match_extract_flow_tuple(const void *buffer, size_t len, cord_flow_tuple_t *tuple);

// High-performance hash calculation for flow tables
uint32_t cord_match_hash_flow_tuple(const cord_flow_tuple_t *tuple);

// =============================================================================
// Layer 2 uni and multicast
// =============================================================================
bool cord_mac_is_multicast(const cord_mac_addr_t *mac_addr);
bool cord_mac_is_broadcast(const cord_mac_addr_t *mac_addr);

#endif // CORD_MATCH_H