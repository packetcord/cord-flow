#include "protocol_headers/cord_protocol_common.h"
#include <conntrack/cord_conntrack.h>
#include <match/cord_match.h>
#include <action/cord_action.h>
#include <stdint.h>

cord_connection_tracker_t connection_tracker_singleton = {.sources_index = 0};

//
// Hash
//
uint64_t cord_ipv4_tcp_connection_hash(cord_ipv4_hdr_t *ipv4_header, cord_tcp_hdr_t *tcp_header)
{
    uint64_t hash = 0;
    hash |= ((uint64_t) (tcp_header->source) << 32);
    hash |= ipv4_header->saddr.addr;

#if (HASH_LOG_ENABLED == 1)
    CORD_LOG("[CordConnTrack] cord_ipv4_tcp_connection_hash() : IPv4: %s | TCP src port: %u | Hash: %lu\n",
             inet_ntoa(*(struct in_addr *) &ipv4_header->saddr), ntohs(tcp_header->source), hash);
#endif

    return hash;
}

uint64_t cord_ipv4_udp_connection_hash(cord_ipv4_hdr_t *ipv4_header, cord_udp_hdr_t *udp_header)
{
    uint64_t hash = 0;
    hash |= ((uint64_t) (udp_header->source) << 32);
    hash |= ipv4_header->saddr.addr;

#if (HASH_LOG_ENABLED == 1)
    CORD_LOG("[CordConnTrack] cord_ipv4_udp_connection_hash() : IPv4: %s | UDP src port: %u | Hash: %lu\n",
             inet_ntoa(*(struct in_addr *) &ipv4_header->saddr), ntohs(udp_header->source), hash);
#endif

    return hash;
}

uint64_t cord_ipv6_tcp_connection_hash(cord_ipv6_hdr_t *ipv6_header, cord_tcp_hdr_t *tcp_header)
{
    uint64_t ipv6_src_last_48_bits = (*(uint64_t *) (&(ipv6_header->saddr.addr))) & 0x0000FFFFFFFFFFFF;
    uint64_t hash = ((uint64_t) (tcp_header->source) << 48);
    hash |= ipv6_src_last_48_bits;

#if (HASH_LOG_ENABLED == 1)
    char address_str[INET6_ADDRSTRLEN];
    const char *ip6_src_address =
        inet_ntop(AF_INET6, (struct in6_addr *) &ipv6_header->ip6_src, address_str, sizeof(address_str));

    CORD_LOG("[CordConnTrack] cord_ipv6_tcp_connection_hash() : IPv6: %s | TCP src port: %u | Hash: %lu\n",
             ip6_src_address, ntohs(tcp_header->source), hash);
#endif

    return hash;
}

uint64_t cord_ipv6_udp_connection_hash(cord_ipv6_hdr_t *ipv6_header, cord_udp_hdr_t *udp_header)
{
    uint64_t ipv6_src_last_48_bits = (*(uint64_t *) (&(ipv6_header->saddr.addr))) & 0x0000FFFFFFFFFFFF;
    uint64_t hash = ((uint64_t) (udp_header->source) << 48);
    hash |= ipv6_src_last_48_bits;

#if (HASH_LOG_ENABLED == 1)
    char address_str[INET6_ADDRSTRLEN];
    const char *ip6_src_address =
        inet_ntop(AF_INET6, (struct in6_addr *) &ipv6_header->ip6_src, address_str, sizeof(address_str));

    CORD_LOG("[CordConnTrack] cord_ipv6_udp_connection_hash() IPv6: %s | UDP src port: %u | Hash: %lu\n",
             ip6_src_address, ntohs(udp_header->source), hash);
#endif

    return hash;
}

void cord_show_connection_hashes(cord_connection_tracker_t *connections, uint32_t elements_count)
{
#if (CONNTRACK_LOG_ENABLED == 1)
    for (uint32_t i = 0; i < elements_count; i++)
        CORD_LOG("[CordConnTrack] cord_show_connection_hashes() : Connection[%u] contains hash: %lu\n", i,
                 connections->sources[i].connection_hash);
#endif
}

bool cord_source_hash_detected(cord_connection_tracker_t *connections, uint64_t current_hash,
                               uint32_t *hash_found_index)
{
    for (uint32_t i = 0; i < CONNTRACK_MAX_CONNTRACK_SOURCES; i++)
    {
        if (current_hash == connections->sources[i].connection_hash)
        {
            *hash_found_index = i;
            return true;
        }
    }

    *hash_found_index = 0;
    return false;
}

//
// Conntrack
//
void cord_init_conntrack(cord_connection_tracker_t *connections)
{
#if (CONNTRACK_LOG_ENABLED == 1)
    bool new_buffer_flag = true;
#endif

    if (connections->sources[connections->sources_index].iov != NULL)
    {
#if (CONNTRACK_LOG_ENABLED == 1)
        new_buffer_flag = false;
        CORD_LOG("[CordConnTrack] cord_init_conntrack() : Slot already in use, calling free() in slot %u.\n",
                 connections->sources_index);
#endif

        if (connections->sources[connections->sources_index].iov != NULL)
        {
            for (uint32_t i = 0; i < CONNTRACK_FRAME_COUNT; i++)
                if (connections->sources[connections->sources_index].iov[i].iov_base != NULL)
                    free(connections->sources[connections->sources_index].iov[i].iov_base);

            free(connections->sources[connections->sources_index].iov);
        }
        connections->sources[connections->sources_index].frame_index = 0;
        connections->sources[connections->sources_index].connection_oriented = false;
    }

#if (CONNTRACK_LOG_ENABLED == 1)
    if (new_buffer_flag)
        CORD_LOG("[CordConnTrack] cord_init_conntrack() : Allocating buffer in slot %u.\n", connections->sources_index);
    else
        CORD_LOG("[CordConnTrack] cord_init_conntrack() : Re-allocating buffer in slot %u.\n",
                 connections->sources_index);
#endif

    connections->sources[connections->sources_index].iov = (struct iovec *) malloc(
        CONNTRACK_FRAME_COUNT * sizeof(*(connections->sources[connections->sources_index].iov)));
    for (uint32_t i = 0; i < CONNTRACK_FRAME_COUNT; i++)
    {
        connections->sources[connections->sources_index].iov[i].iov_base = (uint8_t *) malloc(CONNTRACK_RX_BUFFER_SIZE);
        connections->sources[connections->sources_index].iov[i].iov_len = 0;
    }
}

void cord_append_packet_to_connection(cord_connection_tracker_t *connections, uint32_t conntrack_hash_index,
                                      uint8_t *buffer, int buf_len)
{
    if (!(atomic_load(&(connections->sources[conntrack_hash_index].locked))))
    {
        atomic_store(&(connections->sources[conntrack_hash_index].locked), true);

#if (CONNTRACK_LOG_ENABLED == 1)
        CORD_LOG(
            "[CordConnTrack] cord_append_packet_to_connection() : Adding packet at Slot %u, Frame Num %u, Len %u\n",
            conntrack_hash_index, connections->sources[conntrack_hash_index].frame_index, buf_len);
#endif

        uint32_t n = connections->sources[conntrack_hash_index].frame_index;
        connections->sources[conntrack_hash_index].iov[n].iov_len = buf_len;
        memcpy((void *) connections->sources[conntrack_hash_index].iov[n].iov_base, (const void *) buffer,
               (size_t) buf_len);
        connections->sources[conntrack_hash_index].frame_index =
            (connections->sources[conntrack_hash_index].frame_index + 1) % CONNTRACK_FRAME_COUNT;

        atomic_store(&(connections->sources[conntrack_hash_index].locked), false);
    }
}

bool cord_is_tcp_packet(uint8_t *buffer)
{
    cord_eth_hdr_t *eth = cord_header_eth(buffer);
    if (cord_get_field_eth_type_ntohs(eth) == CORD_ETH_P_IP)
    {
        cord_ipv4_hdr_t *ip = cord_header_ipv4_from_eth(eth);

        if (cord_compare_ipv4_protocol(ip, CORD_IPPROTO_TCP))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (cord_get_field_eth_type_ntohs(eth) == CORD_ETH_P_IPV6)
    {
        cord_ipv6_hdr_t *ipv6 = cord_header_ipv6_from_eth(eth);

        if (cord_compare_ipv6_next_header(ipv6, CORD_IPPROTO_TCP))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void cord_add_new_connection(cord_connection_tracker_t *connections, uint64_t current_hash, uint8_t *buffer,
                             int buf_len)
{
    connections->sources[connections->sources_index].connection_hash = current_hash;
    cord_init_conntrack(connections);
    cord_show_connection_hashes(connections, connections->sources_index + 1);

#if (CONNTRACK_LOG_ENABLED == 1)
    CORD_LOG("[CordConnTrack] cord_add_new_connection() : Reset by new connection");
#endif

    if (cord_is_tcp_packet(buffer))
    {
        connections->sources[connections->sources_index].connection_oriented = true;
    }

    cord_append_packet_to_connection(&connection_tracker_singleton, connections->sources_index, buffer, buf_len);
    connections->sources_index = (connections->sources_index + 1) % CONNTRACK_MAX_CONNTRACK_SOURCES;
}

//
// Arrangement
//
void cord_find_min(uint32_t *arr, size_t len, uint32_t *min_index, uint8_t *prev_min_arr)
{
    uint32_t min;
    if (len > 0)
        min = 0xFFFF;
    else
        return;

    for (uint32_t n = 0; n < len; n++)
    {
        if (prev_min_arr[n] != 1)
        {
            if (arr[n] <= min)
            {
                min = arr[n];
                *min_index = n;
            }
        }
    }

    prev_min_arr[*min_index] = 1;
}

void cord_tcp_find_min(struct iovec *arr, size_t len, uint32_t *min_index, uint8_t *prev_min_arr)
{
    uint32_t min;
    uint32_t tcp_packet_seq_num = 0;
    if (len > 0)
        min = 0xFFFFFFFF;
    else
        return;

    for (uint32_t n = 0; n < len; n++)
    {
        if (prev_min_arr[n] != 1)
        {
            // Get a pointer to the sequence number
            cord_eth_hdr_t *eth = cord_header_eth((cord_eth_hdr_t *) arr[n].iov_base);
            if (cord_get_field_eth_type_ntohs(eth) == CORD_ETH_P_IP)
            {
                cord_ipv4_hdr_t *ip = cord_header_ipv4_from_eth(eth);

                if (cord_compare_ipv4_protocol(ip, CORD_IPPROTO_TCP))
                {
                    int iphdr_len = cord_get_field_ipv4_header_length(ip);
                    cord_tcp_hdr_t *tcp = (cord_tcp_hdr_t *) ((uint8_t *) ip + iphdr_len);
                    tcp_packet_seq_num = tcp->seq;
                }
            }
            else if (cord_get_field_eth_type_ntohs(eth) == CORD_ETH_P_IPV6)
            {
                cord_ipv6_hdr_t *ipv6 = cord_header_ipv6_from_eth(eth);

                if (cord_compare_ipv6_next_header(ipv6, CORD_IPPROTO_TCP))
                {
                    cord_tcp_hdr_t *tcpv6 = (cord_tcp_hdr_t *) ((uint8_t *) ipv6 + sizeof(cord_ipv6_hdr_t));
                    tcp_packet_seq_num = tcpv6->seq;
                }
            }
            else
                return;

            if (tcp_packet_seq_num <= min)
            {
                min = tcp_packet_seq_num;
                *min_index = n;
            }
        }
    }

    prev_min_arr[*min_index] = 1;
}

void cord_asterisk_sort(uint32_t *arr, uint32_t **sorted_asterisk_arr, size_t len)
{
    uint8_t *prev_min_arr = (uint8_t *) calloc(len, sizeof(uint8_t));

    for (uint32_t n = 0; n < len; n++)
    {
        uint32_t min_index = 0;
        cord_find_min(arr, len, &min_index, prev_min_arr);
        sorted_asterisk_arr[n] = &arr[min_index];
    }

    free(prev_min_arr);
}

void cord_tcp_asterisk_sort(struct iovec *arr, struct iovec **sorted_asterisk_arr, size_t len)
{
    uint8_t *prev_min_arr = (uint8_t *) calloc(len, sizeof(uint8_t));

    for (uint32_t n = 0; n < len; n++)
    {
        uint32_t min_index = 0;
        cord_tcp_find_min(arr, len, &min_index, prev_min_arr);
        sorted_asterisk_arr[n] = &arr[min_index];
    }

    free(prev_min_arr);
}

void cord_concatenate_packet_payload(uint8_t **concat_result, size_t *concat_len, uint8_t *current, size_t current_len)
{
    if (current_len > 0)
    {
        if (*concat_result == NULL)
            *concat_result = (uint8_t *) malloc(sizeof(uint8_t) * current_len);
        else
            *concat_result = (uint8_t *) realloc(*concat_result, sizeof(uint8_t) * (*concat_len + current_len));

        memcpy(*concat_result + (*concat_len), current, current_len);
        *concat_len += current_len;
    }
}
