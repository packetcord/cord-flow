#ifndef CORD_TYPE_H
#define CORD_TYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#ifdef ENABLE_SCTP_PROTOCOL
#include <netinet/sctp.h>
#endif
#include <net/ethernet.h>
#include <linux/if_ether.h>
#include <stdint.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <stdbool.h>
#define TRUE  true
#define FALSE false

#define UNUSED_PARAM ( 0 )
#define UNUSED_ARG   ( 0 )

#define CORD_LOG    printf
#define CORD_CLOSE  close

#define NEW_ON_HEAP(Type, ...)                 \
({                                                     \
    __typeof__(Type) *obj = malloc(sizeof(Type));      \
    if (!obj) { perror("malloc"); exit(EXIT_FAILURE); }\
    Type##_ctor(obj, __VA_ARGS__);                     \
    (void *)obj; \
})

#define NEW_ON_STACK(Type, ...)     \
(*({                                \
    Type _tmp;                      \
    Type##_ctor(&_tmp, __VA_ARGS__);\
    &_tmp;                          \
}))

#define DESTROY_ON_HEAP(Type, name) \
({                                  \
    if (name)                       \
    {                               \
        Type##_dtor(name);          \
    }                               \
})

#define DESTROY_ON_STACK(Type, name)\
({                                  \
    if (name)                       \
    {                               \
        name = NULL;                \
    }                               \
})

// Branch prediction hints
#ifndef cord_likely
#ifdef __GNUC__
#define cord_likely(x)   __builtin_expect(!!(x), 1)
#define cord_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define cord_likely(x)   (x)
#define cord_unlikely(x) (x)
#endif
#endif

// Portable byte order macros
#ifndef __BYTE_ORDER
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#ifdef __ARMEL__
#define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined(__ARMEB__)
#define __BYTE_ORDER __BIG_ENDIAN
#elif defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif

// Portable network byte order conversion
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cord_ntohs(x) ((uint16_t)((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define cord_htons(x) cord_ntohs(x)
#define cord_ntohl(x) ((uint32_t)((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff)))
#define cord_htonl(x) cord_ntohl(x)
#define cord_ntohll(x) ((uint64_t)((((uint64_t)cord_ntohl((x) & 0xffffffff)) << 32) | cord_ntohl(((x) >> 32) & 0xffffffff)))
#define cord_htonll(x) cord_ntohll(x)
#else
#define cord_ntohs(x) (x)
#define cord_htons(x) (x)
#define cord_ntohl(x) (x)
#define cord_htonl(x) (x)
#define cord_ntohll(x) (x)
#define cord_htonll(x) (x)
#endif

// Packing attribute for structs
#ifdef __GNUC__
#define CORD_PACKED __attribute__((packed))
#elif defined(_MSC_VER)
#define CORD_PACKED
#pragma pack(push, 1)
#else
#define CORD_PACKED
#endif

#endif // CORD_TYPE_H
