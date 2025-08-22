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

static struct ethhdr* cord_get_eth_hdr(const void *buffer)
{
    return (struct ethhdr*)buffer;
}

static struct iphdr* cord_get_ipv4_hdr(const void *buffer)
{
    return (struct iphdr*)((uint8_t*)buffer + sizeof(struct ethhdr));
}

static struct ip6_hdr* cord_get_ipv6_hdr(const void *buffer)
{
    return (struct ip6_hdr*)((uint8_t*)buffer + sizeof(struct ethhdr));
}

static struct tcphdr* cord_get_tcp_hdr(const struct iphdr *ip)
{
    return (struct tcphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

static struct tcphdr* cord_get_tcp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct tcphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}

static struct udphdr* cord_get_udp_hdr(const struct iphdr *ip)
{
    return (struct udphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

static struct udphdr* cord_get_udp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct udphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}

#ifdef ENABLE_SCTP_PROTOCOL
static struct sctphdr* cord_get_sctp_hdr(const struct iphdr *ip)
{
    return (struct sctphdr*)((uint8_t*)ip + (ip->ihl << 2));
}

static struct sctphdr* cord_get_sctp_hdr_ipv6(const struct ip6_hdr *ip6)
{
    return (struct sctphdr*)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
}
#endif

// ===============================
// Protocol Field Pointer Getters - Ethernet
// ===============================

static uint8_t* cord_get_eth_dst_addr(const struct ethhdr *eth)
{
    return (uint8_t*)eth->h_dest;
}

static uint8_t* cord_get_eth_src_addr(const struct ethhdr *eth)
{
    return (uint8_t*)eth->h_source;
}

static uint16_t* cord_get_eth_type(const struct ethhdr *eth)
{
    return (uint16_t*)&eth->h_proto;
}

// ===============================
// Protocol Field Pointer Getters - IPv4
// ===============================

static uint32_t* cord_get_ipv4_ihl(const struct iphdr *ip)
{
    return (uint32_t*)&ip->ihl;
}

static uint8_t* cord_get_ipv4_tos(const struct iphdr *ip)
{
    return (uint8_t*)&ip->tos;
}

static uint16_t* cord_get_ipv4_total_length(const struct iphdr *ip)
{
    return (uint16_t*)&ip->tot_len;
}

static uint16_t* cord_get_ipv4_id(const struct iphdr *ip)
{
    return (uint16_t*)&ip->id;
}

static uint16_t* cord_get_ipv4_frag_off(const struct iphdr *ip)
{
    return (uint16_t*)&ip->frag_off;
}

static uint8_t* cord_get_ipv4_ttl(const struct iphdr *ip)
{
    return (uint8_t*)&ip->ttl;
}

static uint8_t* cord_get_ipv4_protocol(const struct iphdr *ip)
{
    return (uint8_t*)&ip->protocol;
}

static uint16_t* cord_get_ipv4_checksum(const struct iphdr *ip)
{
    return (uint16_t*)&ip->check;
}

static uint32_t* cord_get_ipv4_src_addr(const struct iphdr *ip)
{
    return (uint32_t*)&ip->saddr;
}

static uint32_t* cord_get_ipv4_dst_addr(const struct iphdr *ip)
{
    return (uint32_t*)&ip->daddr;
}

// ===============================
// Protocol Field Pointer Getters - IPv6
// ===============================

static uint32_t* cord_get_ipv6_vtc_flow(const struct ip6_hdr *ip6)
{
    return (uint32_t*)&ip6->ip6_flow;
}

static uint16_t* cord_get_ipv6_payload_length(const struct ip6_hdr *ip6)
{
    return (uint16_t*)&ip6->ip6_plen;
}

static uint8_t* cord_get_ipv6_next_header(const struct ip6_hdr *ip6)
{
    return (uint8_t*)&ip6->ip6_nxt;
}

static uint8_t* cord_get_ipv6_hop_limit(const struct ip6_hdr *ip6)
{
    return (uint8_t*)&ip6->ip6_hlim;
}

static struct in6_addr* cord_get_ipv6_src_addr(const struct ip6_hdr *ip6)
{
    return (struct in6_addr*)&ip6->ip6_src;
}

static struct in6_addr* cord_get_ipv6_dst_addr(const struct ip6_hdr *ip6)
{
    return (struct in6_addr*)&ip6->ip6_dst;
}

// ===============================
// Protocol Field Pointer Getters - TCP
// ===============================

static uint16_t* cord_get_tcp_src_port(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->source;
}

static uint16_t* cord_get_tcp_dst_port(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->dest;
}

static uint32_t* cord_get_tcp_seq_num(const struct tcphdr *tcp)
{
    return (uint32_t*)&tcp->seq;
}

static uint32_t* cord_get_tcp_ack_num(const struct tcphdr *tcp)
{
    return (uint32_t*)&tcp->ack_seq;
}

static uint16_t* cord_get_tcp_doff(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->doff;
}

static uint16_t* cord_get_tcp_window(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->window;
}

static uint16_t* cord_get_tcp_checksum(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->check;
}

static uint16_t* cord_get_tcp_urgent_ptr(const struct tcphdr *tcp)
{
    return (uint16_t*)&tcp->urg_ptr;
}

// ===============================
// Protocol Field Pointer Getters - UDP
// ===============================

static uint16_t* cord_get_udp_src_port(const struct udphdr *udp)
{
    return (uint16_t*)&udp->source;
}

static uint16_t* cord_get_udp_dst_port(const struct udphdr *udp)
{
    return (uint16_t*)&udp->dest;
}

static uint16_t* cord_get_udp_length(const struct udphdr *udp)
{
    return (uint16_t*)&udp->len;
}

static uint16_t* cord_get_udp_checksum(const struct udphdr *udp)
{
    return (uint16_t*)&udp->check;
}

#ifdef ENABLE_SCTP_PROTOCOL
// ===============================
// Protocol Field Pointer Getters - SCTP
// ===============================

static uint16_t* cord_get_sctp_src_port(const struct sctphdr *sctp)
{
    return (uint16_t*)&sctp->source;
}

static uint16_t* cord_get_sctp_dst_port(const struct sctphdr *sctp)
{
    return (uint16_t*)&sctp->dest;
}

static uint32_t* cord_get_sctp_vtag(const struct sctphdr *sctp)
{
    return (uint32_t*)&sctp->vtag;
}

static uint32_t* cord_get_sctp_checksum(const struct sctphdr *sctp)
{
    return (uint32_t*)&sctp->checksum;
}
#endif

// ===============================
// L2 Ethernet Match Functions
// ===============================

static bool cord_match_eth_dst_addr(const struct ethhdr *eth, const uint8_t *addr)
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

static bool cord_match_eth_src_addr(const struct ethhdr *eth, const uint8_t *addr)
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

static bool cord_match_eth_type(const struct ethhdr *eth, uint16_t eth_type)
{
    return ntohs(eth->h_proto) == eth_type;
}

static bool cord_match_eth_broadcast(const struct ethhdr *eth)
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

static bool cord_match_eth_multicast(const struct ethhdr *eth)
{
    return (eth->h_dest[0] & 0x01) != 0;
}

static bool cord_match_eth_unicast(const struct ethhdr *eth)
{
    return (eth->h_dest[0] & 0x01) == 0;
}

static bool cord_match_eth_crc(const void *buffer, size_t frame_len, uint32_t expected_crc)
{
    if (frame_len < 4) return FALSE;
    
    const uint8_t *frame = (const uint8_t*)buffer;
    uint32_t received_crc = *(uint32_t*)(frame + frame_len - 4);
    
    return ntohl(received_crc) == expected_crc;
}

// ===============================
// L3 IPv4 Match Functions
// ===============================

static bool cord_match_ipv4_version(const struct iphdr *ip)
{
    return ip->version == 4;
}

static bool cord_match_ipv4_ihl(const struct iphdr *ip, uint8_t ihl)
{
    return ip->ihl == ihl;
}

static bool cord_match_ipv4_tos(const struct iphdr *ip, uint8_t tos)
{
    return ip->tos == tos;
}

static bool cord_match_ipv4_dscp(const struct iphdr *ip, uint8_t dscp)
{
    return (ip->tos >> 2) == dscp;
}

static bool cord_match_ipv4_ecn(const struct iphdr *ip, uint8_t ecn)
{
    return (ip->tos & 0x03) == ecn;
}

static bool cord_match_ipv4_total_length(const struct iphdr *ip, uint16_t length)
{
    return ntohs(ip->tot_len) == length;
}

static bool cord_match_ipv4_id(const struct iphdr *ip, uint16_t id)
{
    return ntohs(ip->id) == id;
}

static bool cord_match_ipv4_flags(const struct iphdr *ip, uint16_t flags)
{
    return (ntohs(ip->frag_off) >> 13) == flags;
}

static bool cord_match_ipv4_frag_offset(const struct iphdr *ip, uint16_t offset)
{
    return (ntohs(ip->frag_off) & 0x1FFF) == offset;
}

static bool cord_match_ipv4_ttl(const struct iphdr *ip, uint8_t ttl)
{
    return ip->ttl == ttl;
}

static bool cord_match_ipv4_protocol(const struct iphdr *ip, uint8_t protocol)
{
    return ip->protocol == protocol;
}

static bool cord_match_ipv4_checksum(const struct iphdr *ip, uint16_t checksum)
{
    return ntohs(ip->check) == checksum;
}

static bool cord_match_ipv4_src_addr(const struct iphdr *ip, uint32_t addr)
{
    return ip->saddr == addr;
}

static bool cord_match_ipv4_dst_addr(const struct iphdr *ip, uint32_t addr)
{
    return ip->daddr == addr;
}

static bool cord_match_ipv4_src_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask)
{
    return (ip->saddr & mask) == subnet;
}

static bool cord_match_ipv4_dst_subnet(const struct iphdr *ip, uint32_t subnet, uint32_t mask)
{
    return (ip->daddr & mask) == subnet;
}

// ===============================
// L3 IPv6 Match Functions
// ===============================

static bool cord_match_ipv6_version(const struct ip6_hdr *ip6)
{
    return ((ntohl(ip6->ip6_flow) >> 28) & 0x0F) == 6;
}

static bool cord_match_ipv6_traffic_class(const struct ip6_hdr *ip6, uint8_t tc)
{
    return ((ntohl(ip6->ip6_flow) >> 20) & 0xFF) == tc;
}

static bool cord_match_ipv6_flow_label(const struct ip6_hdr *ip6, uint32_t flow)
{
    return (ntohl(ip6->ip6_flow) & 0x000FFFFF) == flow;
}

static bool cord_match_ipv6_payload_length(const struct ip6_hdr *ip6, uint16_t length)
{
    return ntohs(ip6->ip6_plen) == length;
}

static bool cord_match_ipv6_next_header(const struct ip6_hdr *ip6, uint8_t next_hdr)
{
    return ip6->ip6_nxt == next_hdr;
}

static bool cord_match_ipv6_hop_limit(const struct ip6_hdr *ip6, uint8_t hop_limit)
{
    return ip6->ip6_hlim == hop_limit;
}

static bool cord_match_ipv6_src_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr)
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

static bool cord_match_ipv6_dst_addr(const struct ip6_hdr *ip6, const struct in6_addr *addr)
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

static bool cord_match_ipv6_src_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len)
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

static bool cord_match_ipv6_dst_prefix(const struct ip6_hdr *ip6, const struct in6_addr *prefix, uint8_t prefix_len)
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

static bool cord_match_tcp_src_port(const struct tcphdr *tcp, uint16_t port)
{
    return ntohs(tcp->source) == port;
}

static bool cord_match_tcp_dst_port(const struct tcphdr *tcp, uint16_t port)
{
    return ntohs(tcp->dest) == port;
}

static bool cord_match_tcp_port_range(const struct tcphdr *tcp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(tcp->source) : ntohs(tcp->dest);
    return port >= min_port && port <= max_port;
}

static bool cord_match_tcp_seq_num(const struct tcphdr *tcp, uint32_t seq)
{
    return ntohl(tcp->seq) == seq;
}

static bool cord_match_tcp_ack_num(const struct tcphdr *tcp, uint32_t ack)
{
    return ntohl(tcp->ack_seq) == ack;
}

static bool cord_match_tcp_data_offset(const struct tcphdr *tcp, uint8_t offset)
{
    return tcp->doff == offset;
}

static bool cord_match_tcp_flags(const struct tcphdr *tcp, uint8_t flags)
{
    uint8_t tcp_flags = ((uint8_t*)tcp)[13];
    return tcp_flags == flags;
}

static bool cord_match_tcp_window(const struct tcphdr *tcp, uint16_t window)
{
    return ntohs(tcp->window) == window;
}

static bool cord_match_tcp_checksum(const struct tcphdr *tcp, uint16_t checksum)
{
    return ntohs(tcp->check) == checksum;
}

static bool cord_match_tcp_urgent_ptr(const struct tcphdr *tcp, uint16_t urg_ptr)
{
    return ntohs(tcp->urg_ptr) == urg_ptr;
}

static bool cord_match_tcp_syn(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x02) != 0;
}

static bool cord_match_tcp_ack(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x10) != 0;
}

static bool cord_match_tcp_fin(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x01) != 0;
}

static bool cord_match_tcp_rst(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x04) != 0;
}

static bool cord_match_tcp_psh(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x08) != 0;
}

static bool cord_match_tcp_urg(const struct tcphdr *tcp)
{
    return (((uint8_t*)tcp)[13] & 0x20) != 0;
}

// ===============================
// L4 UDP Match Functions
// ===============================

static bool cord_match_udp_src_port(const struct udphdr *udp, uint16_t port)
{
    return ntohs(udp->source) == port;
}

static bool cord_match_udp_dst_port(const struct udphdr *udp, uint16_t port)
{
    return ntohs(udp->dest) == port;
}

static bool cord_match_udp_port_range(const struct udphdr *udp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(udp->source) : ntohs(udp->dest);
    return port >= min_port && port <= max_port;
}

static bool cord_match_udp_length(const struct udphdr *udp, uint16_t length)
{
    return ntohs(udp->len) == length;
}

static bool cord_match_udp_checksum(const struct udphdr *udp, uint16_t checksum)
{
    return ntohs(udp->check) == checksum;
}

#ifdef ENABLE_SCTP_PROTOCOL
// ===============================
// L4 SCTP Match Functions
// ===============================

static bool cord_match_sctp_src_port(const struct sctphdr *sctp, uint16_t port)
{
    return ntohs(sctp->source) == port;
}

static bool cord_match_sctp_dst_port(const struct sctphdr *sctp, uint16_t port)
{
    return ntohs(sctp->dest) == port;
}

static bool cord_match_sctp_port_range(const struct sctphdr *sctp, uint16_t min_port, uint16_t max_port, bool check_src)
{
    uint16_t port = check_src ? ntohs(sctp->source) : ntohs(sctp->dest);
    return port >= min_port && port <= max_port;
}

static bool cord_match_sctp_vtag(const struct sctphdr *sctp, uint32_t vtag)
{
    return ntohl(sctp->vtag) == vtag;
}

static bool cord_match_sctp_checksum(const struct sctphdr *sctp, uint32_t checksum)
{
    return ntohl(sctp->checksum) == checksum;
}
#endif