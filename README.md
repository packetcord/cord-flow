# CORD-FLOW

CORD-FLOW is a modular component of the PacketCord network programming framework. It provides abstractions and components for high-performance packet flow programming in C (receive-match-action-transmit logic over packet headers and payload). It has been designed to be suitable for purposes across different platforms and architectures—from servers, through embedded systems, to specialized network hardware.

## Directory tree
```bash
cord_flow/
├── CMakeLists.txt
├── include
│   └── cord_flow
│       ├── action
│       │   └── cord_action.h
│       ├── conntrack
│       │   └── cord_conntrack.h
│       ├── event_handler
│       │   ├── cord_custom_event_handler.h
│       │   ├── cord_dpdk_event_handler.h
│       │   ├── cord_event_handler.h
│       │   └── cord_linux_api_event_handler.h
│       ├── filter
│       │   └── cord_filter.h
│       ├── flow_point
│       │   ├── cord_dpdk_flow_point.h
│       │   ├── cord_flow_custom_flow_point.h
│       │   ├── cord_flow_point.h
│       │   ├── cord_l2_raw_socket_flow_point.h
│       │   ├── cord_l3_raw_socket_flow_point.h
│       │   ├── cord_l4_sctp_flow_point.h
│       │   ├── cord_l4_tcp_flow_point.h
│       │   ├── cord_l4_udp_flow_point.h
│       │   ├── cord_stack_inject_flow_point.h
│       │   └── cord_xdp_flow_point.h
│       ├── host_control
│       │   └── cord_host_control.h
│       ├── match
│       │   └── cord_match.h
│       ├── memory
│       │   └── cord_memory.h
│       ├── protocol_headers
│       │   └── cord_protocol_headers.h
│       └── table
│           └── cord_table.h
├── LICENSE
├── README.md
└── src
    ├── action
    │   └── cord_action.c
    ├── conntrack
    │   └── cord_conntrack.c
    ├── event_handler
    │   ├── cord_custom_event_handler.c
    │   ├── cord_dpdk_event_handler.c
    │   ├── cord_event_handler.c
    │   └── cord_linux_api_event_handler.c
    ├── filter
    │   └── cord_filter.c
    ├── flow_point
    │   ├── cord_dpdk_flow_point.c
    │   ├── cord_flow_custom_flow_point.c
    │   ├── cord_flow_point.c
    │   ├── cord_l2_raw_socket_flow_point.c
    │   ├── cord_l3_raw_socket_flow_point.c
    │   ├── cord_l4_sctp_flow_point.c
    │   ├── cord_l4_tcp_flow_point.c
    │   ├── cord_l4_udp_flow_point.c
    │   ├── cord_stack_inject_flow_point.c
    │   └── cord_xdp_flow_point.c
    ├── host_control
    │   └── cord_host_control.c
    ├── match
    │   └── cord_match.c
    ├── memory
    │   └── cord_memory.c
    ├── protocol_headers
    │   └── cord_protocol_headers.c
    └── table
        └── cord_table.c
```

## Components

### FlowPoint
Encapsulates different network ingress/egress points.
- **cord_flow_point**: Core abstraction
- **cord_dpdk_flow_point**: DPDK RX/TX ports and queues
- **cord_xdp_flow_point**: XDP hookpoints (AF_XDP / native)
- **cord_l2_raw_socket_flow_point**: Raw Ethernet sockets
- **cord_l3_raw_socket_flow_point**: Raw IP sockets
- **cord_stack_inject_flow_point**: Injecting into host stack
- **cord_flow_custom_flow_point**: User-defined/custom driver

### EventHandler
Handles polling, epolling, or busy-waiting on flow points:
- **cord_event_handler**: Core abstraction
- **cord_dpdk_event_handler**: Poll-mode or interrupt-based DPDK
- **cord_linux_api_event_handler**: epoll-based
- **cord_custom_event_handler**: User-defined/custom event handler

### Memory
Low-level utilities for packet buffer management:
- Hugepages
- Registers
- Alignment
- Cache Prefetching
- Branch Prediction
- IO vectors
- PACKET_MMAP (PACKET_RX_RING)
- TPACKET_V3
- PACKET_FANOUT

### ProtocolHeaders
Defines structs and parsing utilities for:
- OSI Layers L1 preambles, L2-L7 headers
- Ethernet, VLAN, IP, TCP, UDP, VXLAN, GRE, etc.
- Routing protocols

### ConnectionTracking
Logic for tracking L3/L4 connections:
- Stateless/statistical
- Hash key maps
- TCP payload coalescing

### Table
Data structures:
- CAM (Content Addressable Memory)
- IP routing tables
- NAT/translation tables

### Match
Tools for:
- Matching header fields, metadata
- Value comparisons, masking
- Metadata and integrity checks

### Action (Header Manipulation)
Applies transformations to packets:
- Add, remove, or modify headers
- Set, unset values in header fields

### Filter
Attaches BPF programs as pre-filters:
- cBPF (classic BPF)
- eBPF (XDP, TC, socket attached)

### HostControl
Interacts with host network configuration:
- Reads/writes routing table, ARP
- Invoke `netlink`, `iproute2`, `system()`

### Lab
Contains automation scripts for testing:
- Docker and KVM-based topology setups
- Buildroot-based or shell scripts creation of minimal OS images for x86-64, ARM32, AArch64

---

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## License
MIT