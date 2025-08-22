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

#endif // CORD_TYPE_H
