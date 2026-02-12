#ifndef CORD_MATCH_H
#define CORD_MATCH_H

#include <cord_type.h>
#include <protocol_headers/cord_protocol_headers.h>

// Layer 2 Protocol Headers
cord_eth_hdr_t* cord_header_eth(const void *buffer);
cord_vlan_hdr_t* cord_header_vlan(const cord_eth_hdr_t *eth_hdr);
cord_mpls_hdr_t* cord_header_mpls(const void *buffer, uint16_t offset);
cord_arp_hdr_t* cord_header_arp(const cord_eth_hdr_t *eth_hdr);

// Layer 3 Protocol Headers
cord_ipv4_hdr_t* cord_header_ipv4(const void *buffer);
cord_ipv4_hdr_t* cord_header_ipv4_from_eth(const cord_eth_hdr_t *eth_hdr);
cord_ipv6_hdr_t* cord_header_ipv6(const void *buffer);
cord_ipv6_hdr_t* cord_header_ipv6_from_eth(const cord_eth_hdr_t *eth_hdr);
cord_icmp_hdr_t* cord_header_icmp(const cord_ipv4_hdr_t *ip_hdr);
cord_icmpv6_hdr_t* cord_header_icmpv6(const cord_ipv6_hdr_t *ip6_hdr);

// Layer 4 Protocol Headers
cord_tcp_hdr_t* cord_header_tcp_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_tcp_hdr_t* cord_header_tcp_ipv6(const cord_ipv6_hdr_t *ip6_hdr);
cord_udp_hdr_t* cord_header_udp_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_udp_hdr_t* cord_header_udp_ipv6(const cord_ipv6_hdr_t *ip6_hdr);
cord_sctp_hdr_t* cord_header_sctp_ipv4(const cord_ipv4_hdr_t *ip_hdr);
cord_sctp_hdr_t* cord_header_sctp_ipv6(const cord_ipv6_hdr_t *ip6_hdr);

// Tunneling Protocol Headers
cord_gre_hdr_t* cord_header_gre(const cord_ipv4_hdr_t *ip_hdr);
cord_vxlan_hdr_t* cord_header_vxlan(const cord_udp_hdr_t *udp_hdr);
cord_gtpu_hdr_t* cord_header_gtpu(const cord_udp_hdr_t *udp_hdr);

// VoIP Protocol Headers
cord_rtp_hdr_t* cord_header_rtp(const cord_udp_hdr_t *udp_hdr);
cord_rtcp_hdr_t* cord_header_rtcp(const cord_udp_hdr_t *udp_hdr);

// Routing Protocol Headers
cord_ospf_hdr_t* cord_header_ospf(const cord_ipv4_hdr_t *ip_hdr);
cord_ospf_hello_t* cord_header_ospf_hello(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_db_desc_t* cord_header_ospf_db_desc(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_req_t* cord_header_ospf_ls_req(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_upd_t* cord_header_ospf_ls_upd(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_ls_ack_t* cord_header_ospf_ls_ack(const cord_ospf_hdr_t *ospf_hdr);
cord_ospf_lsa_hdr_t* cord_header_ospf_lsa(const void *lsa_data);
cord_ospf_router_lsa_t* cord_header_ospf_router_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_network_lsa_t* cord_header_ospf_network_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_summary_lsa_t* cord_header_ospf_summary_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_external_lsa_t* cord_header_ospf_external_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_nssa_lsa_t* cord_header_ospf_nssa_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);
cord_ospf_opaque_lsa_t* cord_header_ospf_opaque_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr);

// BGP Protocol Headers
cord_bgp_hdr_t* cord_header_bgp(const cord_tcp_hdr_t *tcp_hdr);
cord_bgp_open_t* cord_header_bgp_open(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_update_t* cord_header_bgp_update(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_notification_t* cord_header_bgp_notification(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_keepalive_t* cord_header_bgp_keepalive(const cord_bgp_hdr_t *bgp_hdr);
cord_bgp_path_attr_t* cord_header_bgp_path_attr(const void *attr_data);
cord_bgp_origin_attr_t* cord_header_bgp_origin_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_as_path_attr_t* cord_header_bgp_as_path_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_next_hop_attr_t* cord_header_bgp_next_hop_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_med_attr_t* cord_header_bgp_med_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_local_pref_attr_t* cord_header_bgp_local_pref_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_communities_attr_t* cord_header_bgp_communities_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_mp_reach_attr_t* cord_header_bgp_mp_reach_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_mp_unreach_attr_t* cord_header_bgp_mp_unreach_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_extended_communities_attr_t* cord_header_bgp_extended_communities_attr(const cord_bgp_path_attr_t *attr);
cord_bgp_large_communities_attr_t* cord_header_bgp_large_communities_attr(const cord_bgp_path_attr_t *attr);

// RIP Protocol Headers
cord_rip_hdr_t* cord_header_rip(const cord_udp_hdr_t *udp_hdr);
cord_rip_msg_t* cord_header_rip_msg(const cord_udp_hdr_t *udp_hdr);
cord_rip_v1_entry_t* cord_header_rip_v1_entry(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_rip_v2_entry_t* cord_header_rip_v2_entry(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_rip_v2_auth_t* cord_header_rip_v2_auth(const cord_rip_msg_t *rip_msg, uint16_t index);
cord_ripng_hdr_t* cord_header_ripng(const cord_udp_hdr_t *udp_hdr);
cord_ripng_entry_t* cord_header_ripng_entry(const cord_ripng_hdr_t *ripng_hdr, uint16_t index);

// IS-IS Protocol Headers
cord_isis_common_hdr_t* cord_header_isis_common(const void *buffer);
cord_isis_p2p_hello_t* cord_header_isis_p2p_hello(const cord_isis_common_hdr_t *common_hdr);
cord_isis_lan_hello_t* cord_header_isis_lan_hello(const cord_isis_common_hdr_t *common_hdr);
cord_isis_lsp_t* cord_header_isis_lsp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_csnp_t* cord_header_isis_csnp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_psnp_t* cord_header_isis_psnp(const cord_isis_common_hdr_t *common_hdr);
cord_isis_tlv_t* cord_header_isis_tlv(const void *tlv_data);
cord_isis_area_addr_tlv_t* cord_header_isis_area_addr_tlv(const cord_isis_tlv_t *tlv);
cord_isis_iis_neighbors_tlv_t* cord_header_isis_iis_neighbors_tlv(const cord_isis_tlv_t *tlv);
cord_isis_auth_tlv_t* cord_header_isis_auth_tlv(const cord_isis_tlv_t *tlv);
cord_isis_lsp_entries_tlv_t* cord_header_isis_lsp_entries_tlv(const cord_isis_tlv_t *tlv);
cord_isis_extended_is_reach_tlv_t* cord_header_isis_extended_is_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ip_internal_reach_tlv_t* cord_header_isis_ip_internal_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ip_external_reach_tlv_t* cord_header_isis_ip_external_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_extended_ip_reach_tlv_t* cord_header_isis_extended_ip_reach_tlv(const cord_isis_tlv_t *tlv);
cord_isis_ipv6_reach_tlv_t* cord_header_isis_ipv6_reach_tlv(const cord_isis_tlv_t *tlv);

// EIGRP Protocol Headers
cord_eigrp_hdr_t* cord_header_eigrp(const cord_ipv4_hdr_t *ip_hdr);
cord_eigrp_tlv_t* cord_header_eigrp_tlv(const void *tlv_data);

// PIM Protocol Headers
cord_pim_hdr_t* cord_header_pim(const cord_ipv4_hdr_t *ip_hdr);

// First Hop Redundancy Protocol Headers
cord_hsrp_hdr_t* cord_header_hsrp(const cord_udp_hdr_t *udp_hdr);
cord_vrrp_hdr_t* cord_header_vrrp(const cord_ipv4_hdr_t *ip_hdr);

// Network Management Protocol Headers
cord_bfd_hdr_t* cord_header_bfd(const cord_udp_hdr_t *udp_hdr);
cord_ldp_hdr_t* cord_header_ldp(const cord_tcp_hdr_t *tcp_hdr);
cord_rsvp_hdr_t* cord_header_rsvp(const cord_ipv4_hdr_t *ip_hdr);

// IGMP Protocol Headers
cord_igmpv3_query_t* cord_header_igmpv3_query(const cord_ipv4_hdr_t *ip_hdr);

// DHCP Protocol Headers
cord_dhcp_hdr_t* cord_header_dhcp(const cord_udp_hdr_t *udp_hdr);
cord_dhcp_option_t* cord_header_dhcp_option(const cord_dhcp_hdr_t *dhcp_hdr, uint16_t offset);
cord_dhcpv6_hdr_t* cord_header_dhcpv6(const cord_udp_hdr_t *udp_hdr);
cord_dhcpv6_relay_hdr_t* cord_header_dhcpv6_relay(const cord_udp_hdr_t *udp_hdr);
cord_dhcpv6_option_t* cord_header_dhcpv6_option(const void *options_data, uint16_t offset);

// IPv6 Neighbor Discovery Protocol Headers
cord_ipv6_nd_router_solicit_t* cord_header_ipv6_nd_router_solicit(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_router_advert_t* cord_header_ipv6_nd_router_advert(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_neighbor_solicit_t* cord_header_ipv6_nd_neighbor_solicit(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_neighbor_advert_t* cord_header_ipv6_nd_neighbor_advert(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_redirect_t* cord_header_ipv6_nd_redirect(const cord_icmpv6_hdr_t *icmp6_hdr);
cord_ipv6_nd_opt_t* cord_header_ipv6_nd_option(const void *options_data, uint16_t offset);
cord_ipv6_nd_opt_lladdr_t* cord_header_ipv6_nd_opt_lladdr(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_prefix_info_t* cord_header_ipv6_nd_opt_prefix_info(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_mtu_t* cord_header_ipv6_nd_opt_mtu(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_rdnss_t* cord_header_ipv6_nd_opt_rdnss(const cord_ipv6_nd_opt_t *opt);
cord_ipv6_nd_opt_dnssl_t* cord_header_ipv6_nd_opt_dnssl(const cord_ipv6_nd_opt_t *opt);

// Ethernet Field Getters
void cord_get_field_eth_dst_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *dst);
void cord_get_field_eth_src_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *src);
uint16_t cord_get_field_eth_type(const cord_eth_hdr_t *eth);
uint16_t cord_get_field_eth_type_ntohs(const cord_eth_hdr_t *eth);

// VLAN Field Getters
uint16_t cord_get_field_vlan_tci(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_field_vlan_tci_ntohs(const cord_vlan_hdr_t *vlan);
uint8_t cord_get_field_vlan_pcp(const cord_vlan_hdr_t *vlan);
uint8_t cord_get_field_vlan_pcp_ntohs(const cord_vlan_hdr_t *vlan);
uint8_t cord_get_field_vlan_dei(const cord_vlan_hdr_t *vlan);
uint8_t cord_get_field_vlan_dei_ntohs(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_field_vlan_vid(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_field_vlan_vid_ntohs(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_field_vlan_type(const cord_vlan_hdr_t *vlan);
uint16_t cord_get_field_vlan_type_ntohs(const cord_vlan_hdr_t *vlan);

// IPv4 Field Getters
uint8_t cord_get_field_ipv4_version(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_ihl(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_tos(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_dscp(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_ecn(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_total_length(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_total_length_ntohs(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_header_length(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_id(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_id_ntohs(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_frag_off(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_frag_off_ntohs(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_ttl(const cord_ipv4_hdr_t *ip);
uint8_t cord_get_field_ipv4_protocol(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_checksum(const cord_ipv4_hdr_t *ip);
uint16_t cord_get_field_ipv4_checksum_ntohs(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_field_ipv4_src_addr(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_field_ipv4_src_addr_ntohl(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_field_ipv4_dst_addr(const cord_ipv4_hdr_t *ip);
uint32_t cord_get_field_ipv4_dst_addr_ntohl(const cord_ipv4_hdr_t *ip);

// IPv6 Field Getters
uint8_t cord_get_field_ipv6_version(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_field_ipv6_version_ntohl(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_field_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_field_ipv6_traffic_class_ntohl(const cord_ipv6_hdr_t *ip6);
uint32_t cord_get_field_ipv6_flow_label(const cord_ipv6_hdr_t *ip6);
uint32_t cord_get_field_ipv6_flow_label_ntohl(const cord_ipv6_hdr_t *ip6);
uint16_t cord_get_field_ipv6_payload_length(const cord_ipv6_hdr_t *ip6);
uint16_t cord_get_field_ipv6_payload_length_ntohs(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_field_ipv6_next_header(const cord_ipv6_hdr_t *ip6);
uint8_t cord_get_field_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6);
void cord_get_field_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *src);
void cord_get_field_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *dst);

// TCP Field Getters
uint16_t cord_get_field_tcp_src_port(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_src_port_ntohs(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_dst_port(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_dst_port_ntohs(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_field_tcp_seq_num(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_field_tcp_seq_num_ntohl(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_field_tcp_ack_num(const cord_tcp_hdr_t *tcp);
uint32_t cord_get_field_tcp_ack_num_ntohl(const cord_tcp_hdr_t *tcp);
uint8_t cord_get_field_tcp_doff(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_window(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_window_ntohs(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_checksum(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_checksum_ntohs(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp);
uint16_t cord_get_field_tcp_urgent_ptr_ntohs(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_fin(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_syn(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_rst(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_psh(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_ack(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_urg(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_ece(const cord_tcp_hdr_t *tcp);
bool cord_get_field_tcp_cwr(const cord_tcp_hdr_t *tcp);

// UDP Field Getters
uint16_t cord_get_field_udp_src_port(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_src_port_ntohs(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_dst_port(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_dst_port_ntohs(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_length(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_length_ntohs(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_checksum(const cord_udp_hdr_t *udp);
uint16_t cord_get_field_udp_checksum_ntohs(const cord_udp_hdr_t *udp);

// SCTP Field Getters
uint16_t cord_get_field_sctp_src_port(const cord_sctp_hdr_t *sctp);
uint16_t cord_get_field_sctp_src_port_ntohs(const cord_sctp_hdr_t *sctp);
uint16_t cord_get_field_sctp_dst_port(const cord_sctp_hdr_t *sctp);
uint16_t cord_get_field_sctp_dst_port_ntohs(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_field_sctp_vtag(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_field_sctp_vtag_ntohl(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_field_sctp_checksum(const cord_sctp_hdr_t *sctp);
uint32_t cord_get_field_sctp_checksum_ntohl(const cord_sctp_hdr_t *sctp);

// ICMP Field Getters
uint8_t cord_get_field_icmp_type(const cord_icmp_hdr_t *icmp);
uint8_t cord_get_field_icmp_code(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_checksum(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_checksum_ntohs(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_id(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_id_ntohs(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_sequence(const cord_icmp_hdr_t *icmp);
uint16_t cord_get_field_icmp_sequence_ntohs(const cord_icmp_hdr_t *icmp);

#endif // CORD_MATCH_H