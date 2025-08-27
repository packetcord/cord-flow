/**
 * @file cord_match.c
 * @brief Zero-copy protocol header parsing and matching implementation
 * 
 * High-performance packet processing functions with strict zero-copy principles.
 * All functions operate directly on packet buffers without any memory copying.
 * Proper network byte order handling and portable protocol header support.
 */

#include <match/cord_match.h>

// =============================================================================
// PROTOCOL HEADER GETTER FUNCTIONS - Zero Copy Implementation
// =============================================================================

// Layer 2 Protocol Headers
cord_eth_hdr_t* cord_get_eth_hdr(const void *buffer)
{
    return (cord_eth_hdr_t*)buffer;
}

cord_vlan_hdr_t* cord_get_vlan_hdr(const cord_eth_hdr_t *eth_hdr)
{
    uint16_t eth_type = cord_ntohs(eth_hdr->h_proto);
    if (eth_type == CORD_ETH_P_8021Q || eth_type == CORD_ETH_P_8021AD) {
        return (cord_vlan_hdr_t*)((uint8_t*)eth_hdr + sizeof(cord_eth_hdr_t));
    }
    return NULL;
}

cord_mpls_hdr_t* cord_get_mpls_hdr(const void *buffer, uint16_t offset)
{
    return (cord_mpls_hdr_t*)((uint8_t*)buffer + offset);
}

cord_arp_hdr_t* cord_get_arp_hdr(const cord_eth_hdr_t *eth_hdr)
{
    if (cord_ntohs(eth_hdr->h_proto) != CORD_ETH_P_ARP) {
        return NULL;
    }
    return (cord_arp_hdr_t*)((uint8_t*)eth_hdr + sizeof(cord_eth_hdr_t));
}

// Layer 3 Protocol Headers  
cord_ipv4_hdr_t* cord_get_ipv4_hdr(const void *buffer)
{
    return (cord_ipv4_hdr_t*)buffer;
}

cord_ipv4_hdr_t* cord_get_ipv4_hdr_l3(const void *buffer)
{
    return (cord_ipv4_hdr_t*)buffer;
}

cord_ipv4_hdr_t* cord_get_ipv4_hdr_from_eth(const cord_eth_hdr_t *eth_hdr)
{
    if (cord_ntohs(eth_hdr->h_proto) != CORD_ETH_P_IP) {
        return NULL;
    }
    return (cord_ipv4_hdr_t*)((uint8_t*)eth_hdr + sizeof(cord_eth_hdr_t));
}

cord_ipv6_hdr_t* cord_get_ipv6_hdr(const void *buffer)
{
    return (cord_ipv6_hdr_t*)buffer;
}

cord_ipv6_hdr_t* cord_get_ipv6_hdr_from_eth(const cord_eth_hdr_t *eth_hdr)
{
    if (cord_ntohs(eth_hdr->h_proto) != CORD_ETH_P_IPV6) {
        return NULL;
    }
    return (cord_ipv6_hdr_t*)((uint8_t*)eth_hdr + sizeof(cord_eth_hdr_t));
}

cord_icmp_hdr_t* cord_get_icmp_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_ICMP) {
        return NULL;
    }
    return (cord_icmp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_icmpv6_hdr_t* cord_get_icmpv6_hdr(const cord_ipv6_hdr_t *ip6_hdr)
{
    if (ip6_hdr->nexthdr != CORD_IPPROTO_ICMPV6) {
        return NULL;
    }
    return (cord_icmpv6_hdr_t*)((uint8_t*)ip6_hdr + sizeof(cord_ipv6_hdr_t));
}

// Layer 4 Protocol Headers
cord_tcp_hdr_t* cord_get_tcp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_TCP) {
        return NULL;
    }
    return (cord_tcp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_tcp_hdr_t* cord_get_tcp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr)
{
    if (ip6_hdr->nexthdr != CORD_IPPROTO_TCP) {
        return NULL;
    }
    return (cord_tcp_hdr_t*)((uint8_t*)ip6_hdr + sizeof(cord_ipv6_hdr_t));
}

cord_udp_hdr_t* cord_get_udp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_UDP) {
        return NULL;
    }
    return (cord_udp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_udp_hdr_t* cord_get_udp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr)
{
    if (ip6_hdr->nexthdr != CORD_IPPROTO_UDP) {
        return NULL;
    }
    return (cord_udp_hdr_t*)((uint8_t*)ip6_hdr + sizeof(cord_ipv6_hdr_t));
}

cord_sctp_hdr_t* cord_get_sctp_hdr_ipv4(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_SCTP) {
        return NULL;
    }
    return (cord_sctp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_sctp_hdr_t* cord_get_sctp_hdr_ipv6(const cord_ipv6_hdr_t *ip6_hdr)
{
    if (ip6_hdr->nexthdr != CORD_IPPROTO_SCTP) {
        return NULL;
    }
    return (cord_sctp_hdr_t*)((uint8_t*)ip6_hdr + sizeof(cord_ipv6_hdr_t));
}

// Tunneling Protocol Headers
cord_gre_hdr_t* cord_get_gre_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_GRE) {
        return NULL;
    }
    return (cord_gre_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_vxlan_hdr_t* cord_get_vxlan_hdr(const cord_udp_hdr_t *udp_hdr)
{
    // VXLAN typically uses port 4789
    if (cord_ntohs(udp_hdr->dest) != 4789) {
        return NULL;
    }
    return (cord_vxlan_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

cord_gtpu_hdr_t* cord_get_gtpu_hdr(const cord_udp_hdr_t *udp_hdr)
{
    // GTP-U uses port 2152
    if (cord_ntohs(udp_hdr->dest) != 2152) {
        return NULL;
    }
    return (cord_gtpu_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

// VoIP Protocol Headers
cord_rtp_hdr_t* cord_get_rtp_hdr(const cord_udp_hdr_t *udp_hdr)
{
    cord_rtp_hdr_t *rtp = (cord_rtp_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
    // Basic RTP version check
    if (CORD_RTP_GET_VERSION(rtp) != 2) {
        return NULL;
    }
    return rtp;
}

cord_rtcp_hdr_t* cord_get_rtcp_hdr(const cord_udp_hdr_t *udp_hdr)
{
    cord_rtcp_hdr_t *rtcp = (cord_rtcp_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
    // Basic RTCP version check and packet type range
    if (CORD_RTCP_GET_VERSION(rtcp) != 2 || 
        rtcp->packet_type < CORD_RTCP_TYPE_SR || 
        rtcp->packet_type > CORD_RTCP_TYPE_APP) {
        return NULL;
    }
    return rtcp;
}

// Routing Protocol Headers
cord_ospf_hdr_t* cord_get_ospf_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_OSPF) {
        return NULL;
    }
    return (cord_ospf_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_bgp_hdr_t* cord_get_bgp_hdr(const cord_tcp_hdr_t *tcp_hdr)
{
    uint16_t src_port = cord_ntohs(tcp_hdr->source);
    uint16_t dst_port = cord_ntohs(tcp_hdr->dest);
    // BGP uses port 179
    if (src_port != 179 && dst_port != 179) {
        return NULL;
    }
    return (cord_bgp_hdr_t*)((uint8_t*)tcp_hdr + (CORD_TCP_GET_DOFF(tcp_hdr) * 4));
}

// =============================================================================
// COMPREHENSIVE ROUTING PROTOCOL IMPLEMENTATIONS  
// =============================================================================

// OSPF Protocol Functions
cord_ospf_hello_t* cord_get_ospf_hello(const cord_ospf_hdr_t *ospf_hdr)
{
    if (ospf_hdr->type != CORD_OSPF_TYPE_HELLO) {
        return NULL;
    }
    return (cord_ospf_hello_t*)ospf_hdr;
}

cord_ospf_db_desc_t* cord_get_ospf_db_desc(const cord_ospf_hdr_t *ospf_hdr)
{
    if (ospf_hdr->type != CORD_OSPF_TYPE_DB_DESC) {
        return NULL;
    }
    return (cord_ospf_db_desc_t*)ospf_hdr;
}

cord_ospf_ls_req_t* cord_get_ospf_ls_req(const cord_ospf_hdr_t *ospf_hdr)
{
    if (ospf_hdr->type != CORD_OSPF_TYPE_LS_REQ) {
        return NULL;
    }
    return (cord_ospf_ls_req_t*)ospf_hdr;
}

cord_ospf_ls_upd_t* cord_get_ospf_ls_upd(const cord_ospf_hdr_t *ospf_hdr)
{
    if (ospf_hdr->type != CORD_OSPF_TYPE_LS_UPD) {
        return NULL;
    }
    return (cord_ospf_ls_upd_t*)ospf_hdr;
}

cord_ospf_ls_ack_t* cord_get_ospf_ls_ack(const cord_ospf_hdr_t *ospf_hdr)
{
    if (ospf_hdr->type != CORD_OSPF_TYPE_LS_ACK) {
        return NULL;
    }
    return (cord_ospf_ls_ack_t*)ospf_hdr;
}

cord_ospf_lsa_hdr_t* cord_get_ospf_lsa_hdr(const void *lsa_data)
{
    return (cord_ospf_lsa_hdr_t*)lsa_data;
}

cord_ospf_router_lsa_t* cord_get_ospf_router_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_ROUTER) {
        return NULL;
    }
    return (cord_ospf_router_lsa_t*)lsa_hdr;
}

cord_ospf_network_lsa_t* cord_get_ospf_network_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_NETWORK) {
        return NULL;
    }
    return (cord_ospf_network_lsa_t*)lsa_hdr;
}

cord_ospf_summary_lsa_t* cord_get_ospf_summary_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_SUMMARY_NET && 
        lsa_hdr->ls_type != CORD_OSPF_LSA_SUMMARY_ASBR) {
        return NULL;
    }
    return (cord_ospf_summary_lsa_t*)lsa_hdr;
}

cord_ospf_external_lsa_t* cord_get_ospf_external_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_EXTERNAL) {
        return NULL;
    }
    return (cord_ospf_external_lsa_t*)lsa_hdr;
}

cord_ospf_nssa_lsa_t* cord_get_ospf_nssa_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_NSSA) {
        return NULL;
    }
    return (cord_ospf_nssa_lsa_t*)lsa_hdr;
}

cord_ospf_opaque_lsa_t* cord_get_ospf_opaque_lsa(const cord_ospf_lsa_hdr_t *lsa_hdr)
{
    if (lsa_hdr->ls_type != CORD_OSPF_LSA_LINK_LOCAL && 
        lsa_hdr->ls_type != CORD_OSPF_LSA_AREA_LOCAL &&
        lsa_hdr->ls_type != CORD_OSPF_LSA_AS_EXTERNAL) {
        return NULL;
    }
    return (cord_ospf_opaque_lsa_t*)lsa_hdr;
}

// BGP Protocol Functions
cord_bgp_open_t* cord_get_bgp_open(const cord_bgp_hdr_t *bgp_hdr)
{
    if (bgp_hdr->type != CORD_BGP_TYPE_OPEN) {
        return NULL;
    }
    return (cord_bgp_open_t*)bgp_hdr;
}

cord_bgp_update_t* cord_get_bgp_update(const cord_bgp_hdr_t *bgp_hdr)
{
    if (bgp_hdr->type != CORD_BGP_TYPE_UPDATE) {
        return NULL;
    }
    return (cord_bgp_update_t*)bgp_hdr;
}

cord_bgp_notification_t* cord_get_bgp_notification(const cord_bgp_hdr_t *bgp_hdr)
{
    if (bgp_hdr->type != CORD_BGP_TYPE_NOTIFICATION) {
        return NULL;
    }
    return (cord_bgp_notification_t*)bgp_hdr;
}

cord_bgp_keepalive_t* cord_get_bgp_keepalive(const cord_bgp_hdr_t *bgp_hdr)
{
    if (bgp_hdr->type != CORD_BGP_TYPE_KEEPALIVE) {
        return NULL;
    }
    return (cord_bgp_keepalive_t*)bgp_hdr;
}

cord_bgp_path_attr_t* cord_get_bgp_path_attr(const void *attr_data)
{
    return (cord_bgp_path_attr_t*)attr_data;
}

cord_bgp_origin_attr_t* cord_get_bgp_origin_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_ORIGIN) {
        return NULL;
    }
    return (cord_bgp_origin_attr_t*)attr;
}

cord_bgp_as_path_attr_t* cord_get_bgp_as_path_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_AS_PATH) {
        return NULL;
    }
    return (cord_bgp_as_path_attr_t*)attr;
}

cord_bgp_next_hop_attr_t* cord_get_bgp_next_hop_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_NEXT_HOP) {
        return NULL;
    }
    return (cord_bgp_next_hop_attr_t*)attr;
}

cord_bgp_med_attr_t* cord_get_bgp_med_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_MED) {
        return NULL;
    }
    return (cord_bgp_med_attr_t*)attr;
}

cord_bgp_local_pref_attr_t* cord_get_bgp_local_pref_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_LOCAL_PREF) {
        return NULL;
    }
    return (cord_bgp_local_pref_attr_t*)attr;
}

cord_bgp_communities_attr_t* cord_get_bgp_communities_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_COMMUNITIES) {
        return NULL;
    }
    return (cord_bgp_communities_attr_t*)attr;
}

cord_bgp_mp_reach_attr_t* cord_get_bgp_mp_reach_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_MP_REACH_NLRI) {
        return NULL;
    }
    return (cord_bgp_mp_reach_attr_t*)attr;
}

cord_bgp_mp_unreach_attr_t* cord_get_bgp_mp_unreach_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_MP_UNREACH_NLRI) {
        return NULL;
    }
    return (cord_bgp_mp_unreach_attr_t*)attr;
}

cord_bgp_extended_communities_attr_t* cord_get_bgp_extended_communities_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_EXT_COMMUNITIES) {
        return NULL;
    }
    return (cord_bgp_extended_communities_attr_t*)attr;
}

cord_bgp_large_communities_attr_t* cord_get_bgp_large_communities_attr(const cord_bgp_path_attr_t *attr)
{
    if (attr->type_code != CORD_BGP_ATTR_LARGE_COMM) {
        return NULL;
    }
    return (cord_bgp_large_communities_attr_t*)attr;
}

// RIP Protocol Functions
cord_rip_hdr_t* cord_get_rip_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // RIP uses port 520
    if (src_port != CORD_PORT_RIP && dst_port != CORD_PORT_RIP) {
        return NULL;
    }
    return (cord_rip_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

cord_rip_msg_t* cord_get_rip_msg(const cord_udp_hdr_t *udp_hdr)
{
    return (cord_rip_msg_t*)cord_get_rip_hdr(udp_hdr);
}

cord_rip_v1_entry_t* cord_get_rip_v1_entry(const cord_rip_msg_t *rip_msg, uint16_t index)
{
    if (!rip_msg || rip_msg->hdr.version != CORD_RIP_VERSION_1) {
        return NULL;
    }
    return (cord_rip_v1_entry_t*)((uint8_t*)rip_msg + sizeof(cord_rip_hdr_t) + 
                                  (index * sizeof(cord_rip_v1_entry_t)));
}

cord_rip_v2_entry_t* cord_get_rip_v2_entry(const cord_rip_msg_t *rip_msg, uint16_t index)
{
    if (!rip_msg || rip_msg->hdr.version != CORD_RIP_VERSION_2) {
        return NULL;
    }
    return (cord_rip_v2_entry_t*)((uint8_t*)rip_msg + sizeof(cord_rip_hdr_t) + 
                                  (index * sizeof(cord_rip_v2_entry_t)));
}

cord_rip_v2_auth_t* cord_get_rip_v2_auth(const cord_rip_msg_t *rip_msg, uint16_t index)
{
    if (!rip_msg || rip_msg->hdr.version != CORD_RIP_VERSION_2) {
        return NULL;
    }
    cord_rip_v2_auth_t *auth = (cord_rip_v2_auth_t*)((uint8_t*)rip_msg + sizeof(cord_rip_hdr_t) + 
                                                     (index * sizeof(cord_rip_v2_auth_t)));
    // Check if this is actually an authentication entry
    if (cord_ntohs(auth->address_family) != CORD_RIP_AF_AUTH) {
        return NULL;
    }
    return auth;
}

cord_ripng_hdr_t* cord_get_ripng_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // RIPng uses port 521
    if (src_port != CORD_PORT_RIPNG && dst_port != CORD_PORT_RIPNG) {
        return NULL;
    }
    return (cord_ripng_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

cord_ripng_entry_t* cord_get_ripng_entry(const cord_ripng_hdr_t *ripng_hdr, uint16_t index)
{
    if (!ripng_hdr || ripng_hdr->version != CORD_RIPNG_VERSION) {
        return NULL;
    }
    return (cord_ripng_entry_t*)((uint8_t*)ripng_hdr + sizeof(cord_ripng_hdr_t) + 
                                 (index * sizeof(cord_ripng_entry_t)));
}

// IS-IS Protocol Functions
cord_isis_common_hdr_t* cord_get_isis_common_hdr(const void *buffer)
{
    cord_isis_common_hdr_t *hdr = (cord_isis_common_hdr_t*)buffer;
    // Verify IS-IS protocol discriminator
    if (hdr->irpd != CORD_ISIS_PROTO_DISCRIMINATOR) {
        return NULL;
    }
    return hdr;
}

cord_isis_p2p_hello_t* cord_get_isis_p2p_hello(const cord_isis_common_hdr_t *common_hdr)
{
    if (common_hdr->pdu_type != CORD_ISIS_PDU_PTP_IIH) {
        return NULL;
    }
    return (cord_isis_p2p_hello_t*)common_hdr;
}

cord_isis_lan_hello_t* cord_get_isis_lan_hello(const cord_isis_common_hdr_t *common_hdr)
{
    if (common_hdr->pdu_type != CORD_ISIS_PDU_L1_LAN_IIH &&
        common_hdr->pdu_type != CORD_ISIS_PDU_L2_LAN_IIH) {
        return NULL;
    }
    return (cord_isis_lan_hello_t*)common_hdr;
}

cord_isis_lsp_t* cord_get_isis_lsp(const cord_isis_common_hdr_t *common_hdr)
{
    if (common_hdr->pdu_type != CORD_ISIS_PDU_L1_LSP &&
        common_hdr->pdu_type != CORD_ISIS_PDU_L2_LSP) {
        return NULL;
    }
    return (cord_isis_lsp_t*)common_hdr;
}

cord_isis_csnp_t* cord_get_isis_csnp(const cord_isis_common_hdr_t *common_hdr)
{
    if (common_hdr->pdu_type != CORD_ISIS_PDU_L1_CSNP &&
        common_hdr->pdu_type != CORD_ISIS_PDU_L2_CSNP) {
        return NULL;
    }
    return (cord_isis_csnp_t*)common_hdr;
}

cord_isis_psnp_t* cord_get_isis_psnp(const cord_isis_common_hdr_t *common_hdr)
{
    if (common_hdr->pdu_type != CORD_ISIS_PDU_L1_PSNP &&
        common_hdr->pdu_type != CORD_ISIS_PDU_L2_PSNP) {
        return NULL;
    }
    return (cord_isis_psnp_t*)common_hdr;
}

cord_isis_tlv_t* cord_get_isis_tlv(const void *tlv_data)
{
    return (cord_isis_tlv_t*)tlv_data;
}

cord_isis_area_addr_tlv_t* cord_get_isis_area_addr_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_AREA_ADDR) {
        return NULL;
    }
    return (cord_isis_area_addr_tlv_t*)tlv;
}

cord_isis_iis_neighbors_tlv_t* cord_get_isis_iis_neighbors_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_IIS_NEIGHBORS) {
        return NULL;
    }
    return (cord_isis_iis_neighbors_tlv_t*)tlv;
}

cord_isis_auth_tlv_t* cord_get_isis_auth_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_AUTHENTICATION) {
        return NULL;
    }
    return (cord_isis_auth_tlv_t*)tlv;
}

cord_isis_lsp_entries_tlv_t* cord_get_isis_lsp_entries_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_LSP_ENTRIES) {
        return NULL;
    }
    return (cord_isis_lsp_entries_tlv_t*)tlv;
}

cord_isis_extended_is_reach_tlv_t* cord_get_isis_extended_is_reach_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_EXTENDED_IS_REACH) {
        return NULL;
    }
    return (cord_isis_extended_is_reach_tlv_t*)tlv;
}

cord_isis_ip_internal_reach_tlv_t* cord_get_isis_ip_internal_reach_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_IP_INTERNAL_REACH) {
        return NULL;
    }
    return (cord_isis_ip_internal_reach_tlv_t*)tlv;
}

cord_isis_ip_external_reach_tlv_t* cord_get_isis_ip_external_reach_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_IP_EXTERNAL_REACH) {
        return NULL;
    }
    return (cord_isis_ip_external_reach_tlv_t*)tlv;
}

cord_isis_extended_ip_reach_tlv_t* cord_get_isis_extended_ip_reach_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_EXTENDED_IP_REACH) {
        return NULL;
    }
    return (cord_isis_extended_ip_reach_tlv_t*)tlv;
}

cord_isis_ipv6_reach_tlv_t* cord_get_isis_ipv6_reach_tlv(const cord_isis_tlv_t *tlv)
{
    if (tlv->type != CORD_ISIS_TLV_IPV6_REACH) {
        return NULL;
    }
    return (cord_isis_ipv6_reach_tlv_t*)tlv;
}

// EIGRP Protocol Functions
cord_eigrp_hdr_t* cord_get_eigrp_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_EIGRP) {
        return NULL;
    }
    return (cord_eigrp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

cord_eigrp_tlv_t* cord_get_eigrp_tlv(const void *tlv_data)
{
    return (cord_eigrp_tlv_t*)tlv_data;
}

// PIM Protocol Functions
cord_pim_hdr_t* cord_get_pim_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_PORT_PIM) {
        return NULL;
    }
    return (cord_pim_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

// First Hop Redundancy Protocol Functions
cord_hsrp_hdr_t* cord_get_hsrp_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // HSRP uses port 1985
    if (src_port != CORD_PORT_HSRP && dst_port != CORD_PORT_HSRP) {
        return NULL;
    }
    return (cord_hsrp_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

cord_vrrp_hdr_t* cord_get_vrrp_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_PORT_VRRP) {
        return NULL;
    }
    return (cord_vrrp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

// Network Management Protocol Functions
cord_bfd_hdr_t* cord_get_bfd_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // BFD uses ports 3784 (control) or 3785 (echo)
    if ((src_port != CORD_PORT_BFD_CONTROL && src_port != CORD_PORT_BFD_ECHO) &&
        (dst_port != CORD_PORT_BFD_CONTROL && dst_port != CORD_PORT_BFD_ECHO)) {
        return NULL;
    }
    return (cord_bfd_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
}

cord_ldp_hdr_t* cord_get_ldp_hdr(const cord_tcp_hdr_t *tcp_hdr)
{
    uint16_t src_port = cord_ntohs(tcp_hdr->source);
    uint16_t dst_port = cord_ntohs(tcp_hdr->dest);
    // LDP uses port 646
    if (src_port != CORD_PORT_LDP && dst_port != CORD_PORT_LDP) {
        return NULL;
    }
    return (cord_ldp_hdr_t*)((uint8_t*)tcp_hdr + (CORD_TCP_GET_DOFF(tcp_hdr) * 4));
}

cord_rsvp_hdr_t* cord_get_rsvp_hdr(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_PORT_RSVP) {
        return NULL;
    }
    return (cord_rsvp_hdr_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
}

// IGMP Protocol Functions
cord_igmpv3_query_t* cord_get_igmpv3_query(const cord_ipv4_hdr_t *ip_hdr)
{
    if (ip_hdr->protocol != CORD_IPPROTO_IGMP) {
        return NULL;
    }
    cord_igmpv3_query_t *igmp = (cord_igmpv3_query_t*)((uint8_t*)ip_hdr + (CORD_IPV4_GET_IHL(ip_hdr) * 4));
    // Check if this is an IGMPv3 query
    if (igmp->type != CORD_IGMP_TYPE_MEMBERSHIP_QUERY) {
        return NULL;
    }
    return igmp;
}

// =============================================================================
// DHCP AND IPv6 NEIGHBOR DISCOVERY PROTOCOL IMPLEMENTATIONS
// =============================================================================

// DHCP Protocol Functions
cord_dhcp_hdr_t* cord_get_dhcp_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // DHCP uses ports 67 (server) or 68 (client)
    if ((src_port != CORD_PORT_DHCP_SERVER && src_port != CORD_PORT_DHCP_CLIENT) &&
        (dst_port != CORD_PORT_DHCP_SERVER && dst_port != CORD_PORT_DHCP_CLIENT)) {
        return NULL;
    }
    cord_dhcp_hdr_t *dhcp = (cord_dhcp_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
    // Verify DHCP magic cookie
    if (cord_ntohl(dhcp->magic) != CORD_DHCP_MAGIC_COOKIE) {
        return NULL;
    }
    return dhcp;
}

cord_dhcp_option_t* cord_get_dhcp_option(const cord_dhcp_hdr_t *dhcp_hdr, uint16_t offset)
{
    if (!dhcp_hdr) {
        return NULL;
    }
    return (cord_dhcp_option_t*)((uint8_t*)dhcp_hdr->options + offset);
}

cord_dhcpv6_hdr_t* cord_get_dhcpv6_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // DHCPv6 uses ports 546 (client) or 547 (server)
    if ((src_port != CORD_PORT_DHCPV6_SERVER && src_port != CORD_PORT_DHCPV6_CLIENT) &&
        (dst_port != CORD_PORT_DHCPV6_SERVER && dst_port != CORD_PORT_DHCPV6_CLIENT)) {
        return NULL;
    }
    cord_dhcpv6_hdr_t *dhcpv6 = (cord_dhcpv6_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
    // Check if it's a relay message (types 12 or 13)
    if (dhcpv6->msg_type == CORD_DHCPV6_RELAY_FORW || dhcpv6->msg_type == CORD_DHCPV6_RELAY_REPL) {
        return NULL; // Use cord_get_dhcpv6_relay_hdr() for relay messages
    }
    return dhcpv6;
}

cord_dhcpv6_relay_hdr_t* cord_get_dhcpv6_relay_hdr(const cord_udp_hdr_t *udp_hdr)
{
    uint16_t src_port = cord_ntohs(udp_hdr->source);
    uint16_t dst_port = cord_ntohs(udp_hdr->dest);
    // DHCPv6 uses ports 546 (client) or 547 (server)
    if ((src_port != CORD_PORT_DHCPV6_SERVER && src_port != CORD_PORT_DHCPV6_CLIENT) &&
        (dst_port != CORD_PORT_DHCPV6_SERVER && dst_port != CORD_PORT_DHCPV6_CLIENT)) {
        return NULL;
    }
    cord_dhcpv6_relay_hdr_t *relay = (cord_dhcpv6_relay_hdr_t*)((uint8_t*)udp_hdr + sizeof(cord_udp_hdr_t));
    // Check if it's a relay message (types 12 or 13)
    if (relay->msg_type != CORD_DHCPV6_RELAY_FORW && relay->msg_type != CORD_DHCPV6_RELAY_REPL) {
        return NULL;
    }
    return relay;
}

cord_dhcpv6_option_t* cord_get_dhcpv6_option(const void *options_data, uint16_t offset)
{
    if (!options_data) {
        return NULL;
    }
    return (cord_dhcpv6_option_t*)((uint8_t*)options_data + offset);
}

// IPv6 Neighbor Discovery Protocol Functions
cord_ipv6_nd_router_solicit_t* cord_get_ipv6_nd_router_solicit(const cord_icmpv6_hdr_t *icmp6_hdr)
{
    if (icmp6_hdr->type != CORD_ICMPV6_ND_ROUTER_SOLICIT) {
        return NULL;
    }
    return (cord_ipv6_nd_router_solicit_t*)icmp6_hdr;
}

cord_ipv6_nd_router_advert_t* cord_get_ipv6_nd_router_advert(const cord_icmpv6_hdr_t *icmp6_hdr)
{
    if (icmp6_hdr->type != CORD_ICMPV6_ND_ROUTER_ADVERT) {
        return NULL;
    }
    return (cord_ipv6_nd_router_advert_t*)icmp6_hdr;
}

cord_ipv6_nd_neighbor_solicit_t* cord_get_ipv6_nd_neighbor_solicit(const cord_icmpv6_hdr_t *icmp6_hdr)
{
    if (icmp6_hdr->type != CORD_ICMPV6_ND_NEIGHBOR_SOLICIT) {
        return NULL;
    }
    return (cord_ipv6_nd_neighbor_solicit_t*)icmp6_hdr;
}

cord_ipv6_nd_neighbor_advert_t* cord_get_ipv6_nd_neighbor_advert(const cord_icmpv6_hdr_t *icmp6_hdr)
{
    if (icmp6_hdr->type != CORD_ICMPV6_ND_NEIGHBOR_ADVERT) {
        return NULL;
    }
    return (cord_ipv6_nd_neighbor_advert_t*)icmp6_hdr;
}

cord_ipv6_nd_redirect_t* cord_get_ipv6_nd_redirect(const cord_icmpv6_hdr_t *icmp6_hdr)
{
    if (icmp6_hdr->type != CORD_ICMPV6_ND_REDIRECT) {
        return NULL;
    }
    return (cord_ipv6_nd_redirect_t*)icmp6_hdr;
}

cord_ipv6_nd_opt_t* cord_get_ipv6_nd_option(const void *options_data, uint16_t offset)
{
    if (!options_data) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_t*)((uint8_t*)options_data + offset);
}

cord_ipv6_nd_opt_lladdr_t* cord_get_ipv6_nd_opt_lladdr(const cord_ipv6_nd_opt_t *opt)
{
    if (!opt || (opt->type != CORD_IPV6_ND_OPT_SOURCE_LLADDR && opt->type != CORD_IPV6_ND_OPT_TARGET_LLADDR)) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_lladdr_t*)opt;
}

cord_ipv6_nd_opt_prefix_info_t* cord_get_ipv6_nd_opt_prefix_info(const cord_ipv6_nd_opt_t *opt)
{
    if (!opt || opt->type != CORD_IPV6_ND_OPT_PREFIX_INFO) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_prefix_info_t*)opt;
}

cord_ipv6_nd_opt_mtu_t* cord_get_ipv6_nd_opt_mtu(const cord_ipv6_nd_opt_t *opt)
{
    if (!opt || opt->type != CORD_IPV6_ND_OPT_MTU) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_mtu_t*)opt;
}

cord_ipv6_nd_opt_rdnss_t* cord_get_ipv6_nd_opt_rdnss(const cord_ipv6_nd_opt_t *opt)
{
    if (!opt || opt->type != CORD_IPV6_ND_OPT_RDNSS) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_rdnss_t*)opt;
}

cord_ipv6_nd_opt_dnssl_t* cord_get_ipv6_nd_opt_dnssl(const cord_ipv6_nd_opt_t *opt)
{
    if (!opt || opt->type != CORD_IPV6_ND_OPT_DNSSL) {
        return NULL;
    }
    return (cord_ipv6_nd_opt_dnssl_t*)opt;
}

// =============================================================================
// PROTOCOL FIELD GETTERS - Network Byte Order Handled
// =============================================================================

// Ethernet Field Getters
void cord_get_eth_dst_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *dst)
{
    *dst = eth->h_dest;  // Direct assignment, no byte order conversion needed for MAC
}

void cord_get_eth_src_addr(const cord_eth_hdr_t *eth, cord_mac_addr_t *src)
{
    *src = eth->h_source;  // Direct assignment, no byte order conversion needed for MAC
}

uint16_t cord_get_eth_type(const cord_eth_hdr_t *eth)
{
    return cord_ntohs(eth->h_proto);  // Convert to host byte order
}

// VLAN Field Getters
uint8_t cord_get_vlan_pcp(const cord_vlan_hdr_t *vlan)
{
    return CORD_VLAN_GET_PCP(cord_ntohs(vlan->tci));
}

uint8_t cord_get_vlan_dei(const cord_vlan_hdr_t *vlan)
{
    return CORD_VLAN_GET_DEI(cord_ntohs(vlan->tci));
}

uint16_t cord_get_vlan_vid(const cord_vlan_hdr_t *vlan)
{
    return CORD_VLAN_GET_VID(cord_ntohs(vlan->tci));
}

// IPv4 Field Getters
uint8_t cord_get_ipv4_version(const cord_ipv4_hdr_t *ip)
{
    return CORD_IPV4_GET_VERSION(ip);
}

uint8_t cord_get_ipv4_ihl(const cord_ipv4_hdr_t *ip)
{
    return CORD_IPV4_GET_IHL(ip);
}

uint8_t cord_get_ipv4_tos(const cord_ipv4_hdr_t *ip)
{
    return ip->tos;
}

uint8_t cord_get_ipv4_dscp(const cord_ipv4_hdr_t *ip)
{
    return CORD_IPV4_GET_DSCP(ip);
}

uint8_t cord_get_ipv4_ecn(const cord_ipv4_hdr_t *ip)
{
    return CORD_IPV4_GET_ECN(ip);
}

uint16_t cord_get_ipv4_total_length(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohs(ip->tot_len);
}

uint16_t cord_get_ipv4_id(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohs(ip->id);
}

uint16_t cord_get_ipv4_frag_off(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohs(ip->frag_off);
}

uint8_t cord_get_ipv4_ttl(const cord_ipv4_hdr_t *ip)
{
    return ip->ttl;
}

uint8_t cord_get_ipv4_protocol(const cord_ipv4_hdr_t *ip)
{
    return ip->protocol;
}

uint16_t cord_get_ipv4_checksum(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohs(ip->check);
}

uint32_t cord_get_ipv4_src_addr(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohl(ip->saddr.addr);
}

uint32_t cord_get_ipv4_dst_addr(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohl(ip->daddr.addr);
}

uint16_t cord_get_ipv4_total_length_ntohs(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohs(ip->tot_len);
}

uint8_t cord_get_ipv4_header_length(const cord_ipv4_hdr_t *ip)
{
    return cord_get_ipv4_ihl(ip) << 2;
}

uint32_t cord_get_ipv4_src_addr_ntohl(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohl(ip->saddr.addr);
}

uint32_t cord_get_ipv4_dst_addr_ntohl(const cord_ipv4_hdr_t *ip)
{
    return cord_ntohl(ip->daddr.addr);
}

uint32_t cord_get_ipv4_src_addr_l3(const cord_ipv4_hdr_t *ip)
{
    return ip->saddr.addr;
}

uint32_t cord_get_ipv4_dst_addr_l3(const cord_ipv4_hdr_t *ip)
{
    return ip->daddr.addr;
}

// IPv6 Field Getters  
uint8_t cord_get_ipv6_version(const cord_ipv6_hdr_t *ip6)
{
    return CORD_IPV6_GET_VERSION(ip6);
}

uint8_t cord_get_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6)
{
    return CORD_IPV6_GET_TCLASS(ip6);
}

uint32_t cord_get_ipv6_flow_label(const cord_ipv6_hdr_t *ip6)
{
    return CORD_IPV6_GET_FLOWLABEL(ip6);
}

uint16_t cord_get_ipv6_payload_length(const cord_ipv6_hdr_t *ip6)
{
    return cord_ntohs(ip6->payload_len);
}

uint8_t cord_get_ipv6_next_header(const cord_ipv6_hdr_t *ip6)
{
    return ip6->nexthdr;
}

uint8_t cord_get_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6)
{
    return ip6->hop_limit;
}

void cord_get_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *src)
{
    *src = ip6->saddr;  // Direct assignment for IPv6 addresses
}

void cord_get_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, cord_ipv6_addr_t *dst)
{
    *dst = ip6->daddr;  // Direct assignment for IPv6 addresses
}

// TCP Field Getters
uint16_t cord_get_tcp_src_port(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->source);
}

uint16_t cord_get_tcp_dst_port(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest);
}

uint32_t cord_get_tcp_seq_num(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohl(tcp->seq);
}

uint32_t cord_get_tcp_ack_num(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohl(tcp->ack_seq);
}

uint8_t cord_get_tcp_doff(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_DOFF(tcp);
}

uint16_t cord_get_tcp_window(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->window);
}

uint16_t cord_get_tcp_checksum(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->check);
}

uint16_t cord_get_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->urg_ptr);
}

bool cord_get_tcp_fin(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_FIN(tcp);
}

bool cord_get_tcp_syn(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_SYN(tcp);
}

bool cord_get_tcp_rst(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_RST(tcp);
}

bool cord_get_tcp_psh(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_PSH(tcp);
}

bool cord_get_tcp_ack(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_ACK(tcp);
}

bool cord_get_tcp_urg(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_URG(tcp);
}

bool cord_get_tcp_ece(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_ECE(tcp);
}

bool cord_get_tcp_cwr(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_CWR(tcp);
}

// UDP Field Getters
uint16_t cord_get_udp_src_port(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source);
}

uint16_t cord_get_udp_dst_port(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->dest);
}

uint16_t cord_get_udp_length(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->len);
}

uint16_t cord_get_udp_checksum(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->check);
}

// SCTP Field Getters
uint16_t cord_get_sctp_src_port(const cord_sctp_hdr_t *sctp)
{
    return cord_ntohs(sctp->source);
}

uint16_t cord_get_sctp_dst_port(const cord_sctp_hdr_t *sctp)
{
    return cord_ntohs(sctp->dest);
}

uint32_t cord_get_sctp_vtag(const cord_sctp_hdr_t *sctp)
{
    return cord_ntohl(sctp->vtag);
}

uint32_t cord_get_sctp_checksum(const cord_sctp_hdr_t *sctp)
{
    return cord_ntohl(sctp->checksum);
}

// ICMP Field Getters
uint8_t cord_get_icmp_type(const cord_icmp_hdr_t *icmp)
{
    return icmp->type;
}

uint8_t cord_get_icmp_code(const cord_icmp_hdr_t *icmp)
{
    return icmp->code;
}

uint16_t cord_get_icmp_checksum(const cord_icmp_hdr_t *icmp)
{
    return cord_ntohs(icmp->checksum);
}

uint16_t cord_get_icmp_id(const cord_icmp_hdr_t *icmp)
{
    return cord_ntohs(icmp->un.echo.id);
}

uint16_t cord_get_icmp_sequence(const cord_icmp_hdr_t *icmp)
{
    return cord_ntohs(icmp->un.echo.sequence);
}

// =============================================================================
// PROTOCOL FIELD MATCHING FUNCTIONS - Zero Copy Comparisons  
// =============================================================================

// L2 Ethernet Match Functions
bool cord_match_eth_dst_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr)
{
    // Compare MAC addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 6; i++) {
        if (eth->h_dest.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_match_eth_src_addr(const cord_eth_hdr_t *eth, const cord_mac_addr_t *addr)
{
    // Compare MAC addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 6; i++) {
        if (eth->h_source.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_match_eth_type(const cord_eth_hdr_t *eth, uint16_t eth_type)
{
    return cord_ntohs(eth->h_proto) == eth_type;
}

bool cord_match_eth_broadcast(const cord_eth_hdr_t *eth)
{
    return cord_mac_is_broadcast(&eth->h_dest);
}

bool cord_match_eth_multicast(const cord_eth_hdr_t *eth)
{
    return cord_mac_is_multicast(&eth->h_dest);
}

bool cord_match_eth_unicast(const cord_eth_hdr_t *eth)
{
    return !cord_mac_is_multicast(&eth->h_dest);
}

// VLAN Match Functions
bool cord_match_vlan_pcp(const cord_vlan_hdr_t *vlan, uint8_t pcp)
{
    return cord_get_vlan_pcp(vlan) == pcp;
}

bool cord_match_vlan_dei(const cord_vlan_hdr_t *vlan, uint8_t dei)
{
    return cord_get_vlan_dei(vlan) == dei;
}

bool cord_match_vlan_vid(const cord_vlan_hdr_t *vlan, uint16_t vid)
{
    return cord_get_vlan_vid(vlan) == vid;
}

bool cord_match_vlan_vid_range(const cord_vlan_hdr_t *vlan, uint16_t min_vid, uint16_t max_vid)
{
    uint16_t vid = cord_get_vlan_vid(vlan);
    return vid >= min_vid && vid <= max_vid;
}

// L3 IPv4 Match Functions
bool cord_match_ipv4_version(const cord_ipv4_hdr_t *ip)
{
    return CORD_IPV4_GET_VERSION(ip) == 4;
}

bool cord_match_ipv4_ihl(const cord_ipv4_hdr_t *ip, uint8_t ihl)
{
    return CORD_IPV4_GET_IHL(ip) == ihl;
}

bool cord_match_ipv4_tos(const cord_ipv4_hdr_t *ip, uint8_t tos)
{
    return ip->tos == tos;
}

bool cord_match_ipv4_dscp(const cord_ipv4_hdr_t *ip, uint8_t dscp)
{
    return CORD_IPV4_GET_DSCP(ip) == dscp;
}

bool cord_match_ipv4_ecn(const cord_ipv4_hdr_t *ip, uint8_t ecn)
{
    return CORD_IPV4_GET_ECN(ip) == ecn;
}

bool cord_match_ipv4_total_length(const cord_ipv4_hdr_t *ip, uint16_t length)
{
    return cord_ntohs(ip->tot_len) == length;
}

bool cord_match_ipv4_id(const cord_ipv4_hdr_t *ip, uint16_t id)
{
    return cord_ntohs(ip->id) == id;
}

bool cord_match_ipv4_flags(const cord_ipv4_hdr_t *ip, uint16_t flags)
{
    return (cord_ntohs(ip->frag_off) & 0xE000) == flags;
}

bool cord_match_ipv4_frag_offset(const cord_ipv4_hdr_t *ip, uint16_t offset)
{
    return (cord_ntohs(ip->frag_off) & CORD_IPV4_OFFSET_MASK) == offset;
}

bool cord_match_ipv4_ttl(const cord_ipv4_hdr_t *ip, uint8_t ttl)
{
    return ip->ttl == ttl;
}

bool cord_match_ipv4_protocol(const cord_ipv4_hdr_t *ip, uint8_t protocol)
{
    return ip->protocol == protocol;
}

bool cord_match_ipv4_checksum(const cord_ipv4_hdr_t *ip, uint16_t checksum)
{
    return cord_ntohs(ip->check) == checksum;
}

bool cord_match_ipv4_src_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return ip->saddr.addr == cord_htonl(addr);  // Compare in network byte order
}

bool cord_match_ipv4_dst_addr(const cord_ipv4_hdr_t *ip, uint32_t addr)
{
    return ip->daddr.addr == cord_htonl(addr);  // Compare in network byte order
}

bool cord_match_ipv4_src_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_ntohl(ip->saddr.addr);
    return (addr & mask) == (subnet & mask);
}

bool cord_match_ipv4_dst_subnet(const cord_ipv4_hdr_t *ip, uint32_t subnet, uint32_t mask)
{
    uint32_t addr = cord_ntohl(ip->daddr.addr);
    return (addr & mask) == (subnet & mask);
}

bool cord_match_ipv4_fragmented(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
    return (frag_off & CORD_IPV4_MF) || (frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_match_ipv4_first_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
    return (frag_off & CORD_IPV4_MF) && !(frag_off & CORD_IPV4_OFFSET_MASK);
}

bool cord_match_ipv4_last_fragment(const cord_ipv4_hdr_t *ip)
{
    uint16_t frag_off = cord_ntohs(ip->frag_off);
    return !(frag_off & CORD_IPV4_MF) && (frag_off & CORD_IPV4_OFFSET_MASK);
}

// L3 IPv6 Match Functions
bool cord_match_ipv6_version(const cord_ipv6_hdr_t *ip6)
{
    return CORD_IPV6_GET_VERSION(ip6) == 6;
}

bool cord_match_ipv6_traffic_class(const cord_ipv6_hdr_t *ip6, uint8_t tc)
{
    return CORD_IPV6_GET_TCLASS(ip6) == tc;
}

bool cord_match_ipv6_flow_label(const cord_ipv6_hdr_t *ip6, uint32_t flow)
{
    return CORD_IPV6_GET_FLOWLABEL(ip6) == flow;
}

bool cord_match_ipv6_payload_length(const cord_ipv6_hdr_t *ip6, uint16_t length)
{
    return cord_ntohs(ip6->payload_len) == length;
}

bool cord_match_ipv6_next_header(const cord_ipv6_hdr_t *ip6, uint8_t next_hdr)
{
    return ip6->nexthdr == next_hdr;
}

bool cord_match_ipv6_hop_limit(const cord_ipv6_hdr_t *ip6, uint8_t hop_limit)
{
    return ip6->hop_limit == hop_limit;
}

bool cord_match_ipv6_src_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr)
{
    // Compare IPv6 addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 16; i++) {
        if (ip6->saddr.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_match_ipv6_dst_addr(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *addr)
{
    // Compare IPv6 addresses byte by byte (no memcmp - zero copy principle)
    for (int i = 0; i < 16; i++) {
        if (ip6->daddr.addr[i] != addr->addr[i]) {
            return false;
        }
    }
    return true;
}

bool cord_match_ipv6_src_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len)
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

bool cord_match_ipv6_dst_prefix(const cord_ipv6_hdr_t *ip6, const cord_ipv6_addr_t *prefix, uint8_t prefix_len)
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
bool cord_match_tcp_src_port(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_ntohs(tcp->source) == port;
}

bool cord_match_tcp_dst_port(const cord_tcp_hdr_t *tcp, uint16_t port)
{
    return cord_ntohs(tcp->dest) == port;
}

bool cord_match_tcp_port_range(const cord_tcp_hdr_t *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(tcp->source) : cord_ntohs(tcp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_tcp_seq_num(const cord_tcp_hdr_t *tcp, uint32_t seq)
{
    return cord_ntohl(tcp->seq) == seq;
}

bool cord_match_tcp_ack_num(const cord_tcp_hdr_t *tcp, uint32_t ack)
{
    return cord_ntohl(tcp->ack_seq) == ack;
}

bool cord_match_tcp_data_offset(const cord_tcp_hdr_t *tcp, uint8_t offset)
{
    return CORD_TCP_GET_DOFF(tcp) == offset;
}

bool cord_match_tcp_window(const cord_tcp_hdr_t *tcp, uint16_t window)
{
    return cord_ntohs(tcp->window) == window;
}

bool cord_match_tcp_checksum(const cord_tcp_hdr_t *tcp, uint16_t checksum)
{
    return cord_ntohs(tcp->check) == checksum;
}

bool cord_match_tcp_urgent_ptr(const cord_tcp_hdr_t *tcp, uint16_t urg_ptr)
{
    return cord_ntohs(tcp->urg_ptr) == urg_ptr;
}

bool cord_match_tcp_syn(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_SYN(tcp);
}

bool cord_match_tcp_ack(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_ACK(tcp);
}

bool cord_match_tcp_fin(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_FIN(tcp);
}

bool cord_match_tcp_rst(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_RST(tcp);
}

bool cord_match_tcp_psh(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_PSH(tcp);
}

bool cord_match_tcp_urg(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_URG(tcp);
}

bool cord_match_tcp_ece(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_ECE(tcp);
}

bool cord_match_tcp_cwr(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_CWR(tcp);
}

bool cord_match_tcp_established(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_ACK(tcp) && !CORD_TCP_GET_SYN(tcp);
}

bool cord_match_tcp_connection_request(const cord_tcp_hdr_t *tcp)
{
    return CORD_TCP_GET_SYN(tcp) && !CORD_TCP_GET_ACK(tcp);
}

// L4 UDP Match Functions
bool cord_match_udp_src_port(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_ntohs(udp->source) == port;
}

bool cord_match_udp_dst_port(const cord_udp_hdr_t *udp, uint16_t port)
{
    return cord_ntohs(udp->dest) == port;
}

bool cord_match_udp_port_range(const cord_udp_hdr_t *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(udp->source) : cord_ntohs(udp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_udp_length(const cord_udp_hdr_t *udp, uint16_t length)
{
    return cord_ntohs(udp->len) == length;
}

bool cord_match_udp_checksum(const cord_udp_hdr_t *udp, uint16_t checksum)
{
    return cord_ntohs(udp->check) == checksum;
}

// L4 SCTP Match Functions  
bool cord_match_sctp_src_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_ntohs(sctp->source) == port;
}

bool cord_match_sctp_dst_port(const cord_sctp_hdr_t *sctp, uint16_t port)
{
    return cord_ntohs(sctp->dest) == port;
}

bool cord_match_sctp_port_range(const cord_sctp_hdr_t *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? cord_ntohs(sctp->source) : cord_ntohs(sctp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_sctp_vtag(const cord_sctp_hdr_t *sctp, uint32_t vtag)
{
    return cord_ntohl(sctp->vtag) == vtag;
}

bool cord_match_sctp_checksum(const cord_sctp_hdr_t *sctp, uint32_t checksum)
{
    return cord_ntohl(sctp->checksum) == checksum;
}

// ICMP Match Functions
bool cord_match_icmp_type(const cord_icmp_hdr_t *icmp, uint8_t type)
{
    return icmp->type == type;
}

bool cord_match_icmp_code(const cord_icmp_hdr_t *icmp, uint8_t code)
{
    return icmp->code == code;
}

bool cord_match_icmp_echo_request(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_ECHO;
}

bool cord_match_icmp_echo_reply(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_ECHOREPLY;
}

bool cord_match_icmp_dest_unreachable(const cord_icmp_hdr_t *icmp)
{
    return icmp->type == CORD_ICMP_DEST_UNREACH;
}

// Tunneling Protocol Match Functions
bool cord_match_gre_checksum_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_CSUM(gre);
}

bool cord_match_gre_key_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_KEY(gre);
}

bool cord_match_gre_sequence_present(const cord_gre_hdr_t *gre)
{
    return CORD_GRE_GET_SEQ(gre);
}

bool cord_match_gre_protocol(const cord_gre_hdr_t *gre, uint16_t protocol)
{
    return cord_ntohs(gre->protocol) == protocol;
}

bool cord_match_vxlan_vni(const cord_vxlan_hdr_t *vxlan, uint32_t vni)
{
    return CORD_VXLAN_GET_VNI(vxlan) == vni;
}

bool cord_match_vxlan_flags(const cord_vxlan_hdr_t *vxlan, uint8_t flags)
{
    return vxlan->flags == flags;
}

bool cord_match_gtpu_teid(const cord_gtpu_hdr_t *gtpu, uint32_t teid)
{
    return cord_ntohl(gtpu->teid) == teid;
}

bool cord_match_gtpu_msg_type(const cord_gtpu_hdr_t *gtpu, uint8_t msg_type)
{
    return gtpu->message_type == msg_type;
}

// VoIP Protocol Match Functions  
bool cord_match_rtp_version(const cord_rtp_hdr_t *rtp, uint8_t version)
{
    return CORD_RTP_GET_VERSION(rtp) == version;
}

bool cord_match_rtp_payload_type(const cord_rtp_hdr_t *rtp, uint8_t pt)
{
    return CORD_RTP_GET_PT(rtp) == pt;
}

bool cord_match_rtp_ssrc(const cord_rtp_hdr_t *rtp, uint32_t ssrc)
{
    return cord_ntohl(rtp->ssrc) == ssrc;
}

bool cord_match_rtp_marker(const cord_rtp_hdr_t *rtp)
{
    return CORD_RTP_GET_MARKER(rtp);
}

bool cord_match_rtcp_packet_type(const cord_rtcp_hdr_t *rtcp, uint8_t type)
{
    return rtcp->packet_type == type;
}

bool cord_match_rtcp_version(const cord_rtcp_hdr_t *rtcp, uint8_t version)
{
    return CORD_RTCP_GET_VERSION(rtcp) == version;
}

// Advanced Protocol Analysis Functions
bool cord_match_is_dns_query(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->dest) == CORD_PORT_DNS || cord_ntohs(udp->source) == CORD_PORT_DNS;
}

bool cord_match_is_dns_response(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DNS;
}

bool cord_match_is_dhcp_request(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DHCP_CLIENT && cord_ntohs(udp->dest) == CORD_PORT_DHCP_SERVER;
}

bool cord_match_is_dhcp_response(const cord_udp_hdr_t *udp)
{
    return cord_ntohs(udp->source) == CORD_PORT_DHCP_SERVER && cord_ntohs(udp->dest) == CORD_PORT_DHCP_CLIENT;
}

bool cord_match_is_http_request(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_HTTP;
}

bool cord_match_is_http_response(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->source) == CORD_PORT_HTTP;
}

bool cord_match_is_https_traffic(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_HTTPS || cord_ntohs(tcp->source) == CORD_PORT_HTTPS;
}

bool cord_match_is_ssh_traffic(const cord_tcp_hdr_t *tcp)
{
    return cord_ntohs(tcp->dest) == CORD_PORT_SSH || cord_ntohs(tcp->source) == CORD_PORT_SSH;
}

// =============================================================================
// PERFORMANCE UTILITIES - Zero Copy Single Pass Processing
// =============================================================================

bool cord_match_extract_protocol_info(const void *buffer, size_t len, cord_protocol_info_t *info)
{
    if (!buffer || !info || len < sizeof(cord_eth_hdr_t)) {
        return false;
    }
    
    // Initialize info structure
    info->eth_type = 0;
    info->ip_version = 0;
    info->ip_protocol = 0;
    info->l4_src_port = 0;
    info->l4_dst_port = 0;
    info->l3_src_addr = 0;
    info->l3_dst_addr = 0;
    info->has_vlan = false;
    info->vlan_vid = 0;
    info->is_fragment = false;
    info->payload_len = 0;
    
    const cord_eth_hdr_t *eth = (const cord_eth_hdr_t*)buffer;
    uint16_t eth_type = cord_ntohs(eth->h_proto);
    info->eth_type = eth_type;
    
    uint16_t offset = sizeof(cord_eth_hdr_t);
    
    // Check for VLAN tags
    if (eth_type == CORD_ETH_P_8021Q || eth_type == CORD_ETH_P_8021AD) {
        if (len < offset + sizeof(cord_vlan_hdr_t)) return false;
        
        const cord_vlan_hdr_t *vlan = (const cord_vlan_hdr_t*)((uint8_t*)buffer + offset);
        info->has_vlan = true;
        info->vlan_vid = CORD_VLAN_GET_VID(cord_ntohs(vlan->tci));
        eth_type = cord_ntohs(vlan->h_proto);
        info->eth_type = eth_type;
        offset += sizeof(cord_vlan_hdr_t);
    }
    
    // Process L3 protocols
    if (eth_type == CORD_ETH_P_IP) {
        if (len < offset + sizeof(cord_ipv4_hdr_t)) return false;
        
        const cord_ipv4_hdr_t *ip = (const cord_ipv4_hdr_t*)((uint8_t*)buffer + offset);
        info->ip_version = CORD_IPV4_GET_VERSION(ip);
        info->ip_protocol = ip->protocol;
        info->l3_src_addr = cord_ntohl(ip->saddr.addr);
        info->l3_dst_addr = cord_ntohl(ip->daddr.addr);
        info->is_fragment = cord_match_ipv4_fragmented(ip);
        
        uint8_t ihl = CORD_IPV4_GET_IHL(ip);
        offset += ihl * 4;
        
        // Process L4 protocols
        if (ip->protocol == CORD_IPPROTO_TCP && len >= offset + sizeof(cord_tcp_hdr_t)) {
            const cord_tcp_hdr_t *tcp = (const cord_tcp_hdr_t*)((uint8_t*)buffer + offset);
            info->l4_src_port = cord_ntohs(tcp->source);
            info->l4_dst_port = cord_ntohs(tcp->dest);
        } else if (ip->protocol == CORD_IPPROTO_UDP && len >= offset + sizeof(cord_udp_hdr_t)) {
            const cord_udp_hdr_t *udp = (const cord_udp_hdr_t*)((uint8_t*)buffer + offset);
            info->l4_src_port = cord_ntohs(udp->source);
            info->l4_dst_port = cord_ntohs(udp->dest);
        }
        
        info->payload_len = cord_ntohs(ip->tot_len) - (ihl * 4);
        
    } else if (eth_type == CORD_ETH_P_IPV6) {
        if (len < offset + sizeof(cord_ipv6_hdr_t)) return false;
        
        const cord_ipv6_hdr_t *ip6 = (const cord_ipv6_hdr_t*)((uint8_t*)buffer + offset);
        info->ip_version = CORD_IPV6_GET_VERSION(ip6);
        info->ip_protocol = ip6->nexthdr;
        
        offset += sizeof(cord_ipv6_hdr_t);
        
        // Process L4 protocols
        if (ip6->nexthdr == CORD_IPPROTO_TCP && len >= offset + sizeof(cord_tcp_hdr_t)) {
            const cord_tcp_hdr_t *tcp = (const cord_tcp_hdr_t*)((uint8_t*)buffer + offset);
            info->l4_src_port = cord_ntohs(tcp->source);
            info->l4_dst_port = cord_ntohs(tcp->dest);
        } else if (ip6->nexthdr == CORD_IPPROTO_UDP && len >= offset + sizeof(cord_udp_hdr_t)) {
            const cord_udp_hdr_t *udp = (const cord_udp_hdr_t*)((uint8_t*)buffer + offset);
            info->l4_src_port = cord_ntohs(udp->source);
            info->l4_dst_port = cord_ntohs(udp->dest);
        }
        
        info->payload_len = cord_ntohs(ip6->payload_len);
    }
    
    return true;
}

bool cord_match_extract_flow_tuple(const void *buffer, size_t len, cord_flow_tuple_t *tuple)
{
    if (!buffer || !tuple || len < sizeof(cord_eth_hdr_t)) {
        return false;
    }
    
    tuple->src_addr = 0;
    tuple->dst_addr = 0;
    tuple->src_port = 0;
    tuple->dst_port = 0;
    tuple->protocol = 0;
    
    const cord_eth_hdr_t *eth = (const cord_eth_hdr_t*)buffer;
    uint16_t eth_type = cord_ntohs(eth->h_proto);
    uint16_t offset = sizeof(cord_eth_hdr_t);
    
    // Skip VLAN tags
    if (eth_type == CORD_ETH_P_8021Q || eth_type == CORD_ETH_P_8021AD) {
        if (len < offset + sizeof(cord_vlan_hdr_t)) return false;
        const cord_vlan_hdr_t *vlan = (const cord_vlan_hdr_t*)((uint8_t*)buffer + offset);
        eth_type = cord_ntohs(vlan->h_proto);
        offset += sizeof(cord_vlan_hdr_t);
    }
    
    if (eth_type == CORD_ETH_P_IP) {
        if (len < offset + sizeof(cord_ipv4_hdr_t)) return false;
        
        const cord_ipv4_hdr_t *ip = (const cord_ipv4_hdr_t*)((uint8_t*)buffer + offset);
        tuple->src_addr = cord_ntohl(ip->saddr.addr);
        tuple->dst_addr = cord_ntohl(ip->daddr.addr);
        tuple->protocol = ip->protocol;
        
        uint8_t ihl = CORD_IPV4_GET_IHL(ip);
        offset += ihl * 4;
        
        if ((ip->protocol == CORD_IPPROTO_TCP || ip->protocol == CORD_IPPROTO_UDP) && 
            len >= offset + 4) { // 4 bytes for src/dst ports
            const uint16_t *ports = (const uint16_t*)((uint8_t*)buffer + offset);
            tuple->src_port = cord_ntohs(ports[0]);
            tuple->dst_port = cord_ntohs(ports[1]);
        }
        
        return true;
    }
    
    return false;
}

// High-performance Jenkins hash for flow tuples
uint32_t cord_match_hash_flow_tuple(const cord_flow_tuple_t *tuple)
{
    uint32_t hash = 0;
    
    // Hash source address
    hash += tuple->src_addr;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    
    // Hash destination address
    hash += tuple->dst_addr;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    
    // Hash protocol (16-bit to align)
    uint32_t ports_proto = ((uint32_t)tuple->src_port << 16) | tuple->dst_port;
    hash += ports_proto;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    
    // Hash protocol
    hash += tuple->protocol;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    
    // Final mixing
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash;
}