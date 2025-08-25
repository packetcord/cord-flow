#include <match/cord_match.h>

// SIMD includes for different architectures
#ifdef X86_64_SIMD_MATCH_ENABLED
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef ARM_NEON_MATCH_ENABLED
#include <arm_neon.h>
#endif

// ===============================
// Protocol Header Getter Functions
// ===============================

struct ethhdr* cord_get_eth_hdr(const void *buffer)
{
    return (struct ethhdr*)buffer;
}

struct iphdr* cord_get_ipv4_hdr(const void *buffer)
{
    return (struct iphdr*)((uint8_t*)buffer + sizeof(struct ethhdr));
}

struct iphdr* cord_get_ipv4_hdr_l3(const void *buffer)
{
    return (struct iphdr*)buffer;
}

struct ip6_hdr* cord_get_ipv6_hdr(const void *buffer)
{
    return (struct ip6_hdr*)((uint8_t*)buffer + sizeof(struct ethhdr));
}

struct tcphdr* cord_get_tcp_hdr(const struct iphdr *ip)
{
    return (struct tcphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

struct tcphdr* cord_get_tcp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct tcphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}

struct udphdr* cord_get_udp_hdr(const struct iphdr *ip)
{
    return (struct udphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

struct udphdr* cord_get_udp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct udphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}

#ifdef ENABLE_SCTP_PROTOCOL
struct sctphdr* cord_get_sctp_hdr(const struct iphdr *ip)
{
    return (struct sctphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

struct sctphdr* cord_get_sctp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct sctphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}
#endif

// ===============================
// Protocol Field Pointer Getters - Ethernet
// ===============================

void cord_get_eth_dst_addr(const struct ethhdr *eth, uint8_t *dst)
{
    for (int i = 0; i < 6; i++) {
        dst[i] = eth->h_dest[i];
    }
}

void cord_get_eth_src_addr(const struct ethhdr *eth, uint8_t *src)
{
    for (int i = 0; i < 6; i++) {
        src[i] = eth->h_source[i];
    }
}

uint16_t cord_get_eth_type_ntohs(const struct ethhdr *eth)
{
    return ntohs(eth->h_proto);
}

// ===============================
// Protocol Field Pointer Getters - IPv4
// ===============================

uint8_t cord_get_ipv4_version(const struct iphdr *ip)
{
    return ip->version;
}

uint8_t cord_get_ipv4_ihl(const struct iphdr *ip)
{
    return ip->ihl;
}

uint8_t cord_get_ipv4_tos(const struct iphdr *ip)
{
    return ip->tos;
}

uint16_t cord_get_ipv4_total_length_ntohs(const struct iphdr *ip)
{
    return ntohs(ip->tot_len);
}

uint16_t cord_get_ipv4_id_ntohs(const struct iphdr *ip)
{
    return ntohs(ip->id);
}

uint16_t cord_get_ipv4_frag_off_ntohs(const struct iphdr *ip)
{
    return ntohs(ip->frag_off);
}

uint8_t cord_get_ipv4_ttl(const struct iphdr *ip)
{
    return ip->ttl;
}

uint8_t cord_get_ipv4_protocol(const struct iphdr *ip)
{
    return ip->protocol;
}

uint16_t cord_get_ipv4_checksum_ntohs(const struct iphdr *ip)
{
    return ntohs(ip->check);
}

uint32_t cord_get_ipv4_src_addr(const struct iphdr *ip)
{
    return ip->saddr;
}

uint32_t cord_get_ipv4_dst_addr(const struct iphdr *ip)
{
    return ip->daddr;
}

uint32_t cord_get_ipv4_src_addr_ntohl(const struct iphdr *ip)
{
    return ntohl(ip->saddr);
}

uint32_t cord_get_ipv4_dst_addr_ntohl(const struct iphdr *ip)
{
    return ntohl(ip->daddr);
}

// ===============================
// Protocol Field Pointer Getters - IPv6
// ===============================

uint32_t cord_get_ipv6_vtc_flow_ntohl(const struct ip6_hdr *ip6)
{
    return ntohl(ip6->ip6_flow);
}

uint16_t cord_get_ipv6_payload_length_ntohs(const struct ip6_hdr *ip6)
{
    return ntohs(ip6->ip6_plen);
}

uint8_t cord_get_ipv6_next_header(const struct ip6_hdr *ip6)
{
    return ip6->ip6_nxt;
}

uint8_t cord_get_ipv6_hop_limit(const struct ip6_hdr *ip6)
{
    return ip6->ip6_hlim;
}

void cord_get_ipv6_src_addr(const struct ip6_hdr *ip6, struct in6_addr *dst)
{
    for (int i = 0; i < 16; i++) {
        ((uint8_t*)dst)[i] = ((uint8_t*)&ip6->ip6_src)[i];
    }
}

void cord_get_ipv6_dst_addr(const struct ip6_hdr *ip6, struct in6_addr *dst)
{
    for (int i = 0; i < 16; i++) {
        ((uint8_t*)dst)[i] = ((uint8_t*)&ip6->ip6_dst)[i];
    }
}

// ===============================
// Protocol Field Pointer Getters - TCP
// ===============================

uint16_t cord_get_tcp_src_port_ntohs(const struct tcphdr *tcp)
{
    return ntohs(tcp->source);
}

uint16_t cord_get_tcp_dst_port_ntohs(const struct tcphdr *tcp)
{
    return ntohs(tcp->dest);
}

uint32_t cord_get_tcp_seq_num_ntohl(const struct tcphdr *tcp)
{
    return ntohl(tcp->seq);
}

uint32_t cord_get_tcp_ack_num_ntohl(const struct tcphdr *tcp)
{
    return ntohl(tcp->ack_seq);
}

uint8_t cord_get_tcp_doff(const struct tcphdr *tcp)
{
    return tcp->doff;
}

uint16_t cord_get_tcp_window_ntohs(const struct tcphdr *tcp)
{
    return ntohs(tcp->window);
}

uint16_t cord_get_tcp_checksum_ntohs(const struct tcphdr *tcp)
{
    return ntohs(tcp->check);
}

uint16_t cord_get_tcp_urgent_ptr_ntohs(const struct tcphdr *tcp)
{
    return ntohs(tcp->urg_ptr);
}

// ===============================
// Protocol Field Pointer Getters - UDP
// ===============================

uint16_t cord_get_udp_src_port_ntohs(const struct udphdr *udp)
{
    return ntohs(udp->source);
}

uint16_t cord_get_udp_dst_port_ntohs(const struct udphdr *udp)
{
    return ntohs(udp->dest);
}

uint16_t cord_get_udp_length_ntohs(const struct udphdr *udp)
{
    return ntohs(udp->len);
}

uint16_t cord_get_udp_checksum_ntohs(const struct udphdr *udp)
{
    return ntohs(udp->check);
}

#ifdef ENABLE_SCTP_PROTOCOL
// ===============================
// Protocol Field Pointer Getters - SCTP
// ===============================

uint16_t cord_get_sctp_src_port_ntohs(const struct sctphdr *sctp)
{
    return ntohs(sctp->source);
}

uint16_t cord_get_sctp_dst_port_ntohs(const struct sctphdr *sctp)
{
    return ntohs(sctp->dest);
}

uint32_t cord_get_sctp_vtag_ntohl(const struct sctphdr *sctp)
{
    return ntohl(sctp->vtag);
}

uint32_t cord_get_sctp_checksum_ntohl(const struct sctphdr *sctp)
{
    return ntohl(sctp->checksum);
}
#endif

// ===============================
// L2 Ethernet Match Functions
// ===============================

bool cord_match_eth_dst_addr(const struct ethhdr *eth, const uint8_t *addr)
{
#ifdef X86_64_SIMD_MATCH_ENABLED
    __m64 eth_addr = *(__m64*)eth->h_dest;
    __m64 cmp_addr = *(__m64*)addr;
    
    __m64 mask = _mm_set_pi8(0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    __m64 masked_eth = _mm_and_si64(eth_addr, mask);
    __m64 masked_cmp = _mm_and_si64(cmp_addr, mask);
    
    __m64 result = _mm_cmpeq_pi8(masked_eth, masked_cmp);
    _mm_empty();
    
    return (_mm_movemask_pi8(result) & 0x3F) == 0x3F;
#elif defined(ARM_NEON_MATCH_ENABLED)
    uint8x8_t eth_addr = vld1_u8((const uint8_t*)eth->h_dest);
    uint8x8_t cmp_addr = vld1_u8(addr);
    uint8x8_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
    
    eth_addr = vand_u8(eth_addr, mask);
    cmp_addr = vand_u8(cmp_addr, mask);
    
    uint8x8_t result = vceq_u8(eth_addr, cmp_addr);
    uint64_t result_64 = vget_lane_u64(vreinterpret_u64_u8(result), 0);
    
    return (result_64 & 0x0000FFFFFFFFFFFF) == 0x0000FFFFFFFFFFFF;
#else
    const uint8_t *eth_addr = (const uint8_t*)eth->h_dest;
    for (int i = 0; i < 6; i++) {
        if (eth_addr[i] != addr[i]) return FALSE;
    }
    return TRUE;
#endif
}

bool cord_match_eth_src_addr(const struct ethhdr *eth, const uint8_t *addr)
{
#ifdef X86_64_SIMD_MATCH_ENABLED
    __m64 eth_addr = *(__m64*)eth->h_source;
    __m64 cmp_addr = *(__m64*)addr;
    
    __m64 mask = _mm_set_pi8(0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    __m64 masked_eth = _mm_and_si64(eth_addr, mask);
    __m64 masked_cmp = _mm_and_si64(cmp_addr, mask);
    
    __m64 result = _mm_cmpeq_pi8(masked_eth, masked_cmp);
    _mm_empty();
    
    return (_mm_movemask_pi8(result) & 0x3F) == 0x3F;
#elif defined(ARM_NEON_MATCH_ENABLED)
    uint8x8_t eth_addr = vld1_u8((const uint8_t*)eth->h_source);
    uint8x8_t cmp_addr = vld1_u8(addr);
    uint8x8_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
    
    eth_addr = vand_u8(eth_addr, mask);
    cmp_addr = vand_u8(cmp_addr, mask);
    
    uint8x8_t result = vceq_u8(eth_addr, cmp_addr);
    uint64_t result_64 = vget_lane_u64(vreinterpret_u64_u8(result), 0);
    
    return (result_64 & 0x0000FFFFFFFFFFFF) == 0x0000FFFFFFFFFFFF;
#else
    const uint8_t *eth_addr = (const uint8_t*)eth->h_source;
    for (int i = 0; i < 6; i++) {
        if (eth_addr[i] != addr[i]) return FALSE;
    }
    return TRUE;
#endif
}

bool cord_match_eth_type(const struct ethhdr *eth, uint16_t eth_type)
{
    return ntohs(eth->h_proto) == eth_type;
}

bool cord_match_eth_broadcast(const struct ethhdr *eth)
{
#ifdef X86_64_SIMD_MATCH_ENABLED
    __m64 eth_addr = *(__m64*)eth->h_dest;
    __m64 broadcast = _mm_set_pi8(0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    
    __m64 mask = _mm_set_pi8(0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    __m64 masked_eth = _mm_and_si64(eth_addr, mask);
    
    __m64 result = _mm_cmpeq_pi8(masked_eth, broadcast);
    _mm_empty();
    
    return (_mm_movemask_pi8(result) & 0x3F) == 0x3F;
#elif defined(ARM_NEON_MATCH_ENABLED)
    uint8x8_t eth_addr = vld1_u8((const uint8_t*)eth->h_dest);
    uint8x8_t broadcast = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
    uint8x8_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
    
    eth_addr = vand_u8(eth_addr, mask);
    
    uint8x8_t result = vceq_u8(eth_addr, broadcast);
    uint64_t result_64 = vget_lane_u64(vreinterpret_u64_u8(result), 0);
    
    return (result_64 & 0x0000FFFFFFFFFFFF) == 0x0000FFFFFFFFFFFF;
#else
    const uint8_t *addr = (const uint8_t*)eth->h_dest;
    for (int i = 0; i < 6; i++) {
        if (addr[i] != 0xFF) return FALSE;
    }
    return TRUE;
#endif
}

bool cord_match_eth_multicast(const struct ethhdr *eth)
{
    return (eth->h_dest[0] & 0x01) != 0;
}

bool cord_match_eth_unicast(const struct ethhdr *eth)
{
    return (eth->h_dest[0] & 0x01) == 0;
}

bool cord_match_eth_crc(const void *buffer, size_t frame_len, uint32_t expected_crc)
{
    if (frame_len < 4) return FALSE;
    
    const uint8_t *frame = (const uint8_t*)buffer;
    uint32_t received_crc = *(uint32_t*)(frame + frame_len - 4);
    
    return ntohl(received_crc) == expected_crc;
}

// ===============================
// L3 IPv4 Match Functions
// ===============================

bool cord_match_ipv4_version(const struct iphdr *ip)
{
    return ip->version == 4;
}

bool cord_match_ipv4_ihl(const struct iphdr *ip, uint8_t ihl)
{
    return ip->ihl == ihl;
}

bool cord_match_ipv4_tos(const struct iphdr *ip, uint8_t tos)
{
    return ip->tos == tos;
}

bool cord_match_ipv4_dscp(const struct iphdr *ip, uint8_t dscp)
{
    return (ip->tos >> 2) == dscp;
}

bool cord_match_ipv4_ecn(const struct iphdr *ip, uint8_t ecn)
{
    return (ip->tos & 0x03) == ecn;
}

bool cord_match_ipv4_total_length(const struct iphdr *ip, uint16_t length)
{
    return ntohs(ip->tot_len) == length;
}

bool cord_match_ipv4_id(const struct iphdr *ip, uint16_t id)
{
    return ntohs(ip->id) == id;
}

bool cord_match_ipv4_flags(const struct iphdr *ip, uint16_t flags)
{
    return (ntohs(ip->frag_off) >> 13) == flags;
}

bool cord_match_ipv4_frag_offset(const struct iphdr *ip, uint16_t offset)
{
    return (ntohs(ip->frag_off) & 0x1FFF) == offset;
}

bool cord_match_ipv4_ttl(const struct iphdr *ip, uint8_t ttl)
{
    return ip->ttl == ttl;
}

bool cord_match_ipv4_protocol(const struct iphdr *ip, uint8_t protocol)
{
    return ip->protocol == protocol;
}

bool cord_match_ipv4_checksum(const struct iphdr *ip, uint16_t checksum)
{
    return ntohs(ip->check) == checksum;
}

bool cord_match_ipv4_src_addr(const struct iphdr *ip, uint32_t addr)
{
    return ip->saddr == addr;
}

bool cord_match_ipv4_dst_addr(const struct iphdr *ip, uint32_t addr)
{
    return ip->daddr == addr;
}

bool cord_match_ipv4_src_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask)
{
    return (ip->saddr & mask) == subnet;
}

bool cord_match_ipv4_dst_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask)
{
    return (ip->daddr & mask) == subnet;
}

// ===============================
// L3 IPv6 Match Functions
// ===============================

bool cord_match_ipv6_version(const struct ip6_hdr *ip6)
{
    return ((ntohl(ip6->ip6_flow) >> 28) & 0x0F) == 6;
}

bool cord_match_ipv6_traffic_class(const struct ip6_hdr *ip6, uint8_t tc)
{
    return ((ntohl(ip6->ip6_flow) >> 20) & 0xFF) == tc;
}

bool cord_match_ipv6_flow_label(const struct ip6_hdr *ip6, uint32_t flow)
{
    return (ntohl(ip6->ip6_flow) & 0x000FFFFF) == flow;
}

bool cord_match_ipv6_payload_length(const struct ip6_hdr *ip6, uint16_t length)
{
    return ntohs(ip6->ip6_plen) == length;
}

bool cord_match_ipv6_next_header(const struct ip6_hdr *ip6, uint8_t next_hdr)
{
    return ip6->ip6_nxt == next_hdr;
}

bool cord_match_ipv6_hop_limit(const struct ip6_hdr *ip6, uint8_t hop_limit)
{
    return ip6->ip6_hlim == hop_limit;
}

bool cord_match_ipv6_src_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr)
{
#ifdef X86_64_SIMD_MATCH_ENABLED
    __m128i ip6_addr = _mm_loadu_si128((const __m128i*)&ip6->ip6_src);
    __m128i cmp_addr = _mm_loadu_si128((const __m128i*)addr);
    
    __m128i result = _mm_cmpeq_epi8(ip6_addr, cmp_addr);
    
    return _mm_movemask_epi8(result) == 0xFFFF;
#elif defined(ARM_NEON_MATCH_ENABLED)
    uint8x16_t ip6_addr = vld1q_u8((const uint8_t*)&ip6->ip6_src);
    uint8x16_t cmp_addr = vld1q_u8((const uint8_t*)addr);
    
    uint8x16_t result = vceqq_u8(ip6_addr, cmp_addr);
    
    uint64x2_t result_64 = vreinterpretq_u64_u8(result);
    uint64_t low = vgetq_lane_u64(result_64, 0);
    uint64_t high = vgetq_lane_u64(result_64, 1);
    
    return (low == 0xFFFFFFFFFFFFFFFF) && (high == 0xFFFFFFFFFFFFFFFF);
#else
    const uint8_t *ip6_addr = (const uint8_t*)&ip6->ip6_src;
    const uint8_t *cmp_addr = (const uint8_t*)addr;
    for (int i = 0; i < 16; i++) {
        if (ip6_addr[i] != cmp_addr[i]) return FALSE;
    }
    return TRUE;
#endif
}

bool cord_match_ipv6_dst_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr)
{
#ifdef X86_64_SIMD_MATCH_ENABLED
    __m128i ip6_addr = _mm_loadu_si128((const __m128i*)&ip6->ip6_dst);
    __m128i cmp_addr = _mm_loadu_si128((const __m128i*)addr);
    
    __m128i result = _mm_cmpeq_epi8(ip6_addr, cmp_addr);
    
    return _mm_movemask_epi8(result) == 0xFFFF;
#elif defined(ARM_NEON_MATCH_ENABLED)
    uint8x16_t ip6_addr = vld1q_u8((const uint8_t*)&ip6->ip6_dst);
    uint8x16_t cmp_addr = vld1q_u8((const uint8_t*)addr);
    
    uint8x16_t result = vceqq_u8(ip6_addr, cmp_addr);
    
    uint64x2_t result_64 = vreinterpretq_u64_u8(result);
    uint64_t low = vgetq_lane_u64(result_64, 0);
    uint64_t high = vgetq_lane_u64(result_64, 1);
    
    return (low == 0xFFFFFFFFFFFFFFFF) && (high == 0xFFFFFFFFFFFFFFFF);
#else
    const uint8_t *ip6_addr = (const uint8_t*)&ip6->ip6_dst;
    const uint8_t *cmp_addr = (const uint8_t*)addr;
    for (int i = 0; i < 16; i++) {
        if (ip6_addr[i] != cmp_addr[i]) return FALSE;
    }
    return TRUE;
#endif
}

bool cord_match_ipv6_src_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len)
{
    if (prefix_len > 128) return FALSE;
    
    const uint8_t *ip6_addr = (const uint8_t*)&ip6->ip6_src;
    const uint8_t *prefix_addr = (const uint8_t*)prefix;
    
    uint8_t bytes_to_check = prefix_len / 8;
    uint8_t bits_to_check = prefix_len % 8;
    
#ifdef X86_64_SIMD_MATCH_ENABLED
    if (bytes_to_check >= 16) {
        __m128i ip6_chunk = _mm_loadu_si128((const __m128i*)ip6_addr);
        __m128i prefix_chunk = _mm_loadu_si128((const __m128i*)prefix_addr);
        
        __m128i result = _mm_cmpeq_epi8(ip6_chunk, prefix_chunk);
        if (_mm_movemask_epi8(result) != 0xFFFF) return FALSE;
        
        bytes_to_check -= 16;
        ip6_addr += 16;
        prefix_addr += 16;
    }
#elif defined(ARM_NEON_MATCH_ENABLED)
    if (bytes_to_check >= 16) {
        uint8x16_t ip6_chunk = vld1q_u8(ip6_addr);
        uint8x16_t prefix_chunk = vld1q_u8(prefix_addr);
        
        uint8x16_t result = vceqq_u8(ip6_chunk, prefix_chunk);
        
        uint64x2_t result_64 = vreinterpretq_u64_u8(result);
        uint64_t low = vgetq_lane_u64(result_64, 0);
        uint64_t high = vgetq_lane_u64(result_64, 1);
        
        if ((low != 0xFFFFFFFFFFFFFFFF) || (high != 0xFFFFFFFFFFFFFFFF))
            return FALSE;
        
        bytes_to_check -= 16;
        ip6_addr += 16;
        prefix_addr += 16;
    }
#endif
    
    for (int i = 0; i < bytes_to_check; i++) {
        if (ip6_addr[i] != prefix_addr[i]) return FALSE;
    }
    
    if (bits_to_check > 0) {
        uint8_t mask = 0xFF << (8 - bits_to_check);
        if ((ip6_addr[bytes_to_check] & mask) != (prefix_addr[bytes_to_check] & mask))
            return FALSE;
    }
    
    return TRUE;
}

bool cord_match_ipv6_dst_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len)
{
    if (prefix_len > 128) return FALSE;
    
    const uint8_t *ip6_addr = (const uint8_t*)&ip6->ip6_dst;
    const uint8_t *prefix_addr = (const uint8_t*)prefix;
    
    uint8_t bytes_to_check = prefix_len / 8;
    uint8_t bits_to_check = prefix_len % 8;
    
#ifdef X86_64_SIMD_MATCH_ENABLED
    if (bytes_to_check >= 16) {
        __m128i ip6_chunk = _mm_loadu_si128((const __m128i*)ip6_addr);
        __m128i prefix_chunk = _mm_loadu_si128((const __m128i*)prefix_addr);
        
        __m128i result = _mm_cmpeq_epi8(ip6_chunk, prefix_chunk);
        if (_mm_movemask_epi8(result) != 0xFFFF) return FALSE;
        
        bytes_to_check -= 16;
        ip6_addr += 16;
        prefix_addr += 16;
    }
#elif defined(ARM_NEON_MATCH_ENABLED)
    if (bytes_to_check >= 16) {
        uint8x16_t ip6_chunk = vld1q_u8(ip6_addr);
        uint8x16_t prefix_chunk = vld1q_u8(prefix_addr);
        
        uint8x16_t result = vceqq_u8(ip6_chunk, prefix_chunk);
        
        uint64x2_t result_64 = vreinterpretq_u64_u8(result);
        uint64_t low = vgetq_lane_u64(result_64, 0);
        uint64_t high = vgetq_lane_u64(result_64, 1);
        
        if ((low != 0xFFFFFFFFFFFFFFFF) || (high != 0xFFFFFFFFFFFFFFFF))
            return FALSE;
        
        bytes_to_check -= 16;
        ip6_addr += 16;
        prefix_addr += 16;
    }
#endif
    
    for (int i = 0; i < bytes_to_check; i++) {
        if (ip6_addr[i] != prefix_addr[i]) return FALSE;
    }
    
    if (bits_to_check > 0) {
        uint8_t mask = 0xFF << (8 - bits_to_check);
        if ((ip6_addr[bytes_to_check] & mask) != (prefix_addr[bytes_to_check] & mask))
            return FALSE;
    }
    
    return TRUE;
}

// ===============================
// L4 TCP Match Functions
// ===============================

bool cord_match_tcp_src_port(const struct tcphdr *tcp, uint16_t port)
{
    return ntohs(tcp->source) == port;
}

bool cord_match_tcp_dst_port(const struct tcphdr *tcp, uint16_t port)
{
    return ntohs(tcp->dest) == port;
}

bool cord_match_tcp_port_range(const struct tcphdr *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(tcp->source) : ntohs(tcp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_tcp_seq_num(const struct tcphdr *tcp, uint32_t seq)
{
    return ntohl(tcp->seq) == seq;
}

bool cord_match_tcp_ack_num(const struct tcphdr *tcp, uint32_t ack)
{
    return ntohl(tcp->ack_seq) == ack;
}

bool cord_match_tcp_data_offset(const struct tcphdr *tcp, uint8_t offset)
{
    return tcp->doff == offset;
}

bool cord_match_tcp_flags(const struct tcphdr *tcp, uint8_t flags)
{
    uint8_t tcp_flags = ((uint8_t*)tcp)[13];
    return tcp_flags == flags;
}

bool cord_match_tcp_window(const struct tcphdr *tcp, uint16_t window)
{
    return ntohs(tcp->window) == window;
}

bool cord_match_tcp_checksum(const struct tcphdr *tcp, uint16_t checksum)
{
    return ntohs(tcp->check) == checksum;
}

bool cord_match_tcp_urgent_ptr(const struct tcphdr *tcp, uint16_t urg_ptr)
{
    return ntohs(tcp->urg_ptr) == urg_ptr;
}

bool cord_match_tcp_syn(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x02) != 0;
}

bool cord_match_tcp_ack(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x10) != 0;
}

bool cord_match_tcp_fin(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x01) != 0;
}

bool cord_match_tcp_rst(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x04) != 0;
}

bool cord_match_tcp_psh(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x08) != 0;
}

bool cord_match_tcp_urg(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x20) != 0;
}

// ===============================
// L4 UDP Match Functions
// ===============================

bool cord_match_udp_src_port(const struct udphdr *udp, uint16_t port)
{
    return ntohs(udp->source) == port;
}

bool cord_match_udp_dst_port(const struct udphdr *udp, uint16_t port)
{
    return ntohs(udp->dest) == port;
}

bool cord_match_udp_port_range(const struct udphdr *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(udp->source) : ntohs(udp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_udp_length(const struct udphdr *udp, uint16_t length)
{
    return ntohs(udp->len) == length;
}

bool cord_match_udp_checksum(const struct udphdr *udp, uint16_t checksum)
{
    return ntohs(udp->check) == checksum;
}

#ifdef ENABLE_SCTP_PROTOCOL
// ===============================
// L4 SCTP Match Functions
// ===============================

bool cord_match_sctp_src_port(const struct sctphdr *sctp, uint16_t port)
{
    return ntohs(sctp->source) == port;
}

bool cord_match_sctp_dst_port(const struct sctphdr *sctp, uint16_t port)
{
    return ntohs(sctp->dest) == port;
}

bool cord_match_sctp_port_range(const struct sctphdr *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(sctp->source) : ntohs(sctp->dest);
    return port >= min_port && port <= max_port;
}

bool cord_match_sctp_vtag(const struct sctphdr *sctp, uint32_t vtag)
{
    return ntohl(sctp->vtag) == vtag;
}

bool cord_match_sctp_checksum(const struct sctphdr *sctp, uint32_t checksum)
{
    return ntohl(sctp->checksum) == checksum;
}
#endif