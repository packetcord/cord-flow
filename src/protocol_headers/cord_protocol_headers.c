/**
 * @file cord_protocol_headers.c
 * @brief Implementation of protocol header utility functions
 * 
 * This source file contains implementations for protocol header parsing,
 * validation, and manipulation functions that are too complex for inline
 * implementation or need to be available as non-inline functions.
 */

#include <protocol_headers/cord_protocol_headers.h>

// Checksum and validation functions
uint16_t cord_ipv4_checksum(const cord_ipv4_hdr_t *ip_hdr)
{
    const uint8_t *data = (const uint8_t*)ip_hdr;
    uint32_t sum = 0;
    uint8_t ihl = CORD_IPV4_GET_IHL(ip_hdr);
    uint16_t header_len = ihl * 4;
    
    // Checksum all 16-bit words in header using byte-wise access
    for (int i = 0; i < header_len; i += 2) {
        if (i != 10) { // Skip checksum field itself (bytes 10-11)
            uint16_t word = (data[i] << 8) | data[i + 1];
            sum += word;
        }
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return cord_htons(~sum);
}

bool cord_ipv4_checksum_valid(const cord_ipv4_hdr_t *ip_hdr)
{
    return cord_ipv4_checksum(ip_hdr) == ip_hdr->check;
}

uint16_t cord_ipv4_payload_length(const cord_ipv4_hdr_t *ip_hdr)
{
    return cord_ntohs(ip_hdr->tot_len) - (CORD_IPV4_GET_IHL(ip_hdr) * 4);
}

uint16_t cord_ipv6_payload_length(const cord_ipv6_hdr_t *ip6_hdr)
{
    return cord_ntohs(ip6_hdr->payload_len);
}

bool cord_ipv4_is_multicast(uint32_t addr)
{
    return (cord_ntohl(addr) & 0xF0000000) == 0xE0000000;
}

bool cord_ipv4_is_broadcast(uint32_t addr)
{
    return addr == 0xFFFFFFFF;
}

bool cord_mac_is_multicast(const cord_mac_addr_t *mac_addr)
{
    return (mac_addr->addr[0] & 0x01) != 0;
}

bool cord_mac_is_broadcast(const cord_mac_addr_t *mac_addr)
{
    return (mac_addr->addr[0] == 0xFF && mac_addr->addr[1] == 0xFF && 
            mac_addr->addr[2] == 0xFF && mac_addr->addr[3] == 0xFF &&
            mac_addr->addr[4] == 0xFF && mac_addr->addr[5] == 0xFF);
}

// More complex checksum calculations and validations

/**
 * @brief Calculate TCP checksum with pseudo-header for IPv4
 * @param ip_hdr IPv4 header
 * @param tcp_hdr TCP header
 * @param data_len Length of TCP data
 * @return Calculated TCP checksum
 */
uint16_t cord_tcp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr, 
                                const cord_tcp_hdr_t *tcp_hdr, 
                                uint16_t data_len)
{
    uint32_t sum = 0;
    uint16_t tcp_len = CORD_TCP_HEADER_LEN(tcp_hdr) + data_len;
    
    // IPv4 pseudo-header
    sum += (ip_hdr->saddr.addr >> 16) & 0xFFFF;
    sum += ip_hdr->saddr.addr & 0xFFFF;
    sum += (ip_hdr->daddr.addr >> 16) & 0xFFFF;
    sum += ip_hdr->daddr.addr & 0xFFFF;
    sum += cord_htons(CORD_IPPROTO_TCP);
    sum += cord_htons(tcp_len);
    
    // TCP header and data using byte-wise access
    const uint8_t *data = (const uint8_t*)tcp_hdr;
    uint16_t len = tcp_len;
    int i = 0;
    
    while (len > 1) {
        uint16_t word = (data[i] << 8) | data[i + 1];
        sum += word;
        i += 2;
        len -= 2;
    }
    
    // Handle odd byte
    if (len == 1) {
        sum += data[i] << 8;
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

/**
 * @brief Calculate UDP checksum with pseudo-header for IPv4
 * @param ip_hdr IPv4 header
 * @param udp_hdr UDP header
 * @return Calculated UDP checksum
 */
uint16_t cord_udp_checksum_ipv4(const cord_ipv4_hdr_t *ip_hdr, 
                                const cord_udp_hdr_t *udp_hdr)
{
    uint32_t sum = 0;
    uint16_t udp_len = cord_ntohs(udp_hdr->len);
    
    // IPv4 pseudo-header
    sum += (ip_hdr->saddr.addr >> 16) & 0xFFFF;
    sum += ip_hdr->saddr.addr & 0xFFFF;
    sum += (ip_hdr->daddr.addr >> 16) & 0xFFFF;
    sum += ip_hdr->daddr.addr & 0xFFFF;
    sum += cord_htons(CORD_IPPROTO_UDP);
    sum += udp_hdr->len; // Already in network byte order
    
    // UDP header and data using byte-wise access
    const uint8_t *data = (const uint8_t*)udp_hdr;
    uint16_t len = udp_len;
    int i = 0;
    
    while (len > 1) {
        uint16_t word = (data[i] << 8) | data[i + 1];
        sum += word;
        i += 2;
        len -= 2;
    }
    
    // Handle odd byte
    if (len == 1) {
        sum += data[i] << 8;
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

/**
 * @brief Calculate ICMP checksum
 * @param icmp_hdr ICMP header
 * @param data_len Length of ICMP data
 * @return Calculated ICMP checksum
 */
uint16_t cord_icmp_checksum(const cord_icmp_hdr_t *icmp_hdr, uint16_t data_len)
{
    uint32_t sum = 0;
    const uint8_t *data = (const uint8_t*)icmp_hdr;
    uint16_t total_len = sizeof(cord_icmp_hdr_t) + data_len;
    int i = 0;
    
    while (i < total_len - 1) {
        if (i != 2) { // Skip checksum field (bytes 2-3)
            uint16_t word = (data[i] << 8) | data[i + 1];
            sum += word;
        }
        i += 2;
    }
    
    // Handle odd byte
    if (total_len & 1) {
        sum += data[total_len - 1] << 8;
    }
    
    // Add carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

/**
 * @brief Parse IPv6 extension headers and find next header
 * @param ip6_hdr IPv6 header
 * @param next_hdr Output: next header type
 * @param hdr_len Output: total extension headers length
 * @return Pointer to payload after extension headers
 */
void* cord_ipv6_parse_ext_headers(const cord_ipv6_hdr_t *ip6_hdr, 
                                  uint8_t *next_hdr, 
                                  uint16_t *hdr_len)
{
    uint8_t *ptr = (uint8_t*)ip6_hdr + sizeof(cord_ipv6_hdr_t);
    uint8_t nh = ip6_hdr->nexthdr;
    uint16_t len = 0;
    
    while (1) {
        switch (nh) {
            case CORD_IPPROTO_HOPOPTS:
            case CORD_IPPROTO_ROUTING:
            case CORD_IPPROTO_DSTOPTS: {
                cord_ipv6_opt_hdr_t *opt_hdr = (cord_ipv6_opt_hdr_t*)ptr;
                nh = opt_hdr->nexthdr;
                len += (opt_hdr->hdrlen + 1) * 8;
                ptr += (opt_hdr->hdrlen + 1) * 8;
                break;
            }
            case CORD_IPPROTO_FRAGMENT: {
                cord_ipv6_frag_hdr_t *frag_hdr = (cord_ipv6_frag_hdr_t*)ptr;
                nh = frag_hdr->nexthdr;
                len += sizeof(cord_ipv6_frag_hdr_t);
                ptr += sizeof(cord_ipv6_frag_hdr_t);
                break;
            }
            default:
                // No more extension headers
                *next_hdr = nh;
                *hdr_len = len;
                return ptr;
        }
    }
}

/**
 * @brief Convert IPv4 address to string representation
 * @param addr IPv4 address in network byte order
 * @param buf Output buffer (must be at least 16 bytes)
 * @return Pointer to buffer
 */
char* cord_ipv4_to_string(uint32_t addr, char *buf)
{
    uint32_t host_addr = cord_ntohl(addr);
    uint8_t *bytes = (uint8_t*)&host_addr;
    
    // Simple sprintf implementation without external dependencies
    char *p = buf;
    for (int i = 3; i >= 0; i--) {
        uint8_t byte = bytes[i];
        if (byte >= 100) {
            *p++ = '0' + (byte / 100);
            byte %= 100;
        }
        if (byte >= 10 || *(p-1) != buf[0]) {
            *p++ = '0' + (byte / 10);
            byte %= 10;
        }
        *p++ = '0' + byte;
        if (i > 0) *p++ = '.';
    }
    *p = '\0';
    return buf;
}

/**
 * @brief Convert MAC address to string representation
 * @param mac_addr MAC address
 * @param buf Output buffer (must be at least 18 bytes)
 * @return Pointer to buffer
 */
char* cord_mac_to_string(const cord_mac_addr_t *mac_addr, char *buf)
{
    const char hex[] = "0123456789abcdef";
    char *p = buf;
    
    for (int i = 0; i < 6; i++) {
        uint8_t byte = mac_addr->addr[i];
        *p++ = hex[byte >> 4];
        *p++ = hex[byte & 0x0F];
        if (i < 5) *p++ = ':';
    }
    *p = '\0';
    return buf;
}

/**
 * @brief Simple packet validation - check for common malformed packets
 * @param buffer Packet buffer
 * @param buf_len Buffer length
 * @return true if packet appears valid, false otherwise
 */
bool cord_packet_basic_validation(const void *buffer, size_t buf_len)
{
    if (!buffer || buf_len < sizeof(cord_eth_hdr_t)) {
        return false;
    }
    
    const cord_eth_hdr_t *eth = (const cord_eth_hdr_t*)buffer;
    uint16_t eth_type = cord_ntohs(eth->h_proto);
    
    if (eth_type == CORD_ETH_P_IP) {
        if (buf_len < sizeof(cord_eth_hdr_t) + sizeof(cord_ipv4_hdr_t)) {
            return false;
        }
        
        const cord_ipv4_hdr_t *ip = (const cord_ipv4_hdr_t*)((uint8_t*)buffer + sizeof(cord_eth_hdr_t));
        
        // Check IPv4 version
        if (CORD_IPV4_GET_VERSION(ip) != 4) {
            return false;
        }
        
        // Check IHL
        uint8_t ihl = CORD_IPV4_GET_IHL(ip);
        if (ihl < 5 || ihl > 15) {
            return false;
        }
        
        // Check total length
        uint16_t ip_len = cord_ntohs(ip->tot_len);
        if (ip_len < (ihl * 4) || ip_len > (buf_len - sizeof(cord_eth_hdr_t))) {
            return false;
        }
        
        return true;
    }
    
    if (eth_type == CORD_ETH_P_IPV6) {
        if (buf_len < sizeof(cord_eth_hdr_t) + sizeof(cord_ipv6_hdr_t)) {
            return false;
        }
        
        const cord_ipv6_hdr_t *ip6 = (const cord_ipv6_hdr_t*)((uint8_t*)buffer + sizeof(cord_eth_hdr_t));
        
        // Check IPv6 version
        if (CORD_IPV6_GET_VERSION(ip6) != 6) {
            return false;
        }
        
        // Check payload length
        uint16_t payload_len = cord_ntohs(ip6->payload_len);
        if (payload_len > (buf_len - sizeof(cord_eth_hdr_t) - sizeof(cord_ipv6_hdr_t))) {
            return false;
        }
        
        return true;
    }
    
    // For other protocols, basic length check is sufficient
    return true;
}

/**
 * @brief Extract VLAN tags from Ethernet frame
 * @param buffer Packet buffer
 * @param vlan_tags Array to store VLAN tags (output)
 * @param max_tags Maximum number of tags to extract
 * @return Number of VLAN tags found
 */
uint8_t cord_extract_vlan_tags(const void *buffer, uint16_t *vlan_tags, uint8_t max_tags)
{
    const uint8_t *ptr = (const uint8_t*)buffer + 12; // Skip source/dest MAC
    uint8_t tag_count = 0;
    
    while (tag_count < max_tags) {
        uint16_t ethertype = cord_ntohs(*(const uint16_t*)ptr);
        
        if (ethertype == CORD_ETH_P_8021Q || ethertype == CORD_ETH_P_8021AD) {
            ptr += 2; // Skip ethertype
            uint16_t tci = cord_ntohs(*(const uint16_t*)ptr);
            vlan_tags[tag_count++] = CORD_VLAN_GET_VID(tci);
            ptr += 2; // Skip TCI
        } else {
            break;
        }
    }
    
    return tag_count;
}

/**
 * @brief Calculate Ethernet frame CRC32
 * @param buffer Frame buffer (without FCS)
 * @param frame_len Frame length (without FCS)
 * @return Calculated CRC32
 */
uint32_t cord_ethernet_crc32(const void *buffer, size_t frame_len)
{
    // CRC-32 IEEE 802.3 polynomial: 0x04C11DB7
    static const uint32_t crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
        0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
        0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
        0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
        0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
        0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
        0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
        0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
        0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
        0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
        0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
        0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
        0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
        0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
        0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
        0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
        0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
        0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
        0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
        0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
        0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
        0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *data = (const uint8_t*)buffer;
    
    for (size_t i = 0; i < frame_len; i++) {
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    
    return ~crc;
}