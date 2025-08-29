# 🌐 CORD-FLOW

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C Standard](https://img.shields.io/badge/C-C23-blue.svg)](https://en.wikipedia.org/wiki/C23_%28C_standard_revision%29)
[![Build System](https://img.shields.io/badge/Build-CMake-brightgreen.svg)](https://cmake.org/)
[![Architecture](https://img.shields.io/badge/Arch-x86__64%20%7C%20ARM64%20%7C%20ARM32-orange.svg)](https://github.com/packetcord/cord-flow)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Embedded-lightgrey.svg)](https://github.com/packetcord/cord-flow)

**High-Performance Packet Flow Programming Framework**

A modular component of the PacketCord ecosystem, providing comprehensive abstractions for packet processing with receive-match-action-transmit logic. Designed for cross-platform deployment from high-end servers to resource-constrained embedded systems and specialized network hardware.

---

## 🚀 Key Features

- **🏎️ Zero-Copy Architecture** - Optimized memory management with huge page support
- **⚡ Hardware Acceleration** - Native support for DPDK, XDP, and AES-NI instructions
- **🔧 Modular Design** - Pluggable components for different network stacks and hardware
- **📡 Multi-Protocol Support** - Comprehensive L2-L7 protocol header definitions
- **🎯 High Performance** - Cache-aligned buffers, prefetching, and branch prediction
- **🔌 Platform Agnostic** - Works on servers, embedded systems, and network hardware

## 📋 Components

### 🌊 FlowPoint
Network ingress/egress abstractions supporting multiple backend technologies:

| Component | Status | Description | Use Case |
|-----------|--------|-------------|----------|
| `cord_l2_raw_socket_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | Raw Ethernet sockets | Layer 2 packet manipulation |
| `cord_l3_stack_inject_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | Host stack injection | Network stack integration |
| `cord_l4_udp_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | UDP endpoints | UDP transport layer |
| `cord_l4_tcp_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | TCP endpoints | TCP transport layer |
| `cord_l4_sctp_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | SCTP endpoints | SCTP transport layer |
| `cord_l3_raw_socket_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | Raw IP sockets | Layer 3 packet processing |
| `cord_dpdk_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | DPDK RX/TX ports and queues | High-performance userspace networking |
| `cord_xdp_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | XDP hookpoints (AF_XDP/native) | Kernel bypass packet processing |
| `cord_connectx4_lx_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | ConnectX-4 Lx NIC support | Hardware-specific networking |
| `cord_sdr_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | Software Defined Radio | RF packet processing |
| `cord_xwdm_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | XWDM optical networking | Optical transport networking |
| `cord_custom_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | User-defined drivers | Custom hardware support |

### ⚡ EventHandler
Efficient event processing with multiple polling strategies:

| Component | Status | Description |
|-----------|--------|-------------|
| `cord_linux_api_event_handler` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | epoll-based event handling |
| `cord_dpdk_event_handler` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | Poll-mode or interrupt-based DPDK integration |
| `cord_custom_event_handler` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | User-defined event processing logic |

### 🧠 Memory Management
![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen)

Advanced memory allocation optimized for packet processing:

- **Huge Pages** - 2MB/1GB page allocation for performance
- **Cache Alignment** - 64-byte aligned buffers to prevent false sharing
- **Zero-Copy Buffers** - Efficient packet buffer management
- **PACKET_MMAP** - Ring buffer support (TPACKET_V3, PACKET_FANOUT)

### 📦 Protocol Headers
![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen)

Comprehensive protocol support with portable definitions:

- **L2 Protocols** - Ethernet, VLAN, ARP, STP
- **L3 Protocols** - IPv4, IPv6, ICMP, IGMP
- **L4 Protocols** - TCP, UDP, SCTP
- **Tunneling** - VXLAN, GRE, MPLS
- **VoIP** - SIP, RTP, RTCP
- **3GPP** - GTP, SCTP, Diameter
- **Routing** - OSPF, BGP, IS-IS

### 🔍 Connection Tracking
![In Progress](https://img.shields.io/badge/Status-In_Progress-orange)

Stateful and stateless connection monitoring:

- **Hash-based tracking** - Efficient connection state management
- **TCP payload coalescing** - Reassembly of fragmented streams
- **Statistical analysis** - Connection pattern detection

### 📊 Table Management
![Planned](https://img.shields.io/badge/Status-Planned-red)

High-performance data structures:

- **CAM Tables** - Content Addressable Memory for fast lookups
- **IP Routing Tables** - Optimized routing decision engines
- **NAT Tables** - Network Address Translation support

### 🎯 Match Engine
![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen)

Flexible packet matching capabilities:

- **Header field matching** - Multi-layer protocol field extraction
- **Value comparisons** - Efficient comparison operations
- **Metadata processing** - Packet metadata and integrity checks

### 🔄 Action Engine
![Planned](https://img.shields.io/badge/Status-Planned-red)

Comprehensive packet manipulation:

- **Header manipulation** - Add, remove, modify protocol headers
- **Field modification** - Set/unset values in header fields
- **Packet transformation** - Complete packet restructuring

### 🛡️ Filter System
![Planned](https://img.shields.io/badge/Status-Planned-red)

Advanced packet filtering with BPF integration:

- **Classic BPF** - Traditional Berkeley Packet Filter support
- **Extended BPF** - Modern eBPF programs (XDP, TC, socket-attached)
- **Hardware offload** - Filter acceleration where available

### 🖥️ Host Control
![Planned](https://img.shields.io/badge/Status-Planned-red)

System integration and configuration:

- **Routing table management** - Read/write system routing tables
- **ARP manipulation** - Address Resolution Protocol control
- **Network configuration** - Integration with netlink, iproute2

---

## 🛠️ Build Instructions

### Prerequisites
- **CMake** 3.16 or higher
- **GCC** with C23 support
- **Linux** development headers

### Standard Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Installation
```bash
sudo make install
```

### Cross-Compilation
For embedded targets, specify the toolchain:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake ..
```

---

## 🔧 Usage Example

```c
#include <cord_flow/flow_point/cord_flow_point.h>
#include <cord_flow/protocol_headers/cord_protocol_headers.h>
#include <cord_flow/memory/cord_memory.h>

// Create a high-performance packet buffer
uint8_t *packet_buffer = cord_huge_pages_malloc(2048);

// Initialize a flow point for packet reception
CordFlowPoint *flow_point = CORD_CREATE_FLOW_POINT(0);

// Receive packets with zero-copy semantics
ssize_t bytes_received;
cord_retval_t result = CORD_FLOW_POINT_RX(flow_point, packet_buffer, 2048, &bytes_received);

// Parse protocol headers
cord_eth_hdr_t *eth_hdr = (cord_eth_hdr_t*)packet_buffer;
if (cord_ntohs(eth_hdr->ether_type) == CORD_ETHERTYPE_IP) {
    cord_ipv4_hdr_t *ip_hdr = (cord_ipv4_hdr_t*)(packet_buffer + sizeof(cord_eth_hdr_t));
    // Process IPv4 packet...
}

// Cleanup
CORD_DESTROY_FLOW_POINT(flow_point);
cord_huge_pages_free(packet_buffer);
```

---

## 📁 Repository Structure

```
cord-flow/
├── CMakeLists.txt             # Build configuration
├── LICENSE                    # MIT license  
├── README.md                  # This file
├── include/cord_flow/         # Public C header files
│   ├── action/
│   ├── conntrack/
│   ├── event_handler/
│   ├── filter/
│   ├── flow_point/
│   ├── host_control/
│   ├── match/
│   ├── memory/
│   ├── protocol_headers/
│   ├── table/
│   ├── cord_error.h
│   ├── cord_retval.h
│   └── cord_type.h
└── src/                       # C source implementations
    ├── action/
    ├── conntrack/
    ├── event_handler/
    ├── filter/
    ├── flow_point/
    ├── host_control/
    ├── match/
    ├── memory/
    ├── protocol_headers/
    └── table/
```

---

## 🔗 Related Projects

CORD-FLOW is part of the broader **PacketCord** ecosystem:

- **[cord-aes-cipher](https://github.com/packetcord/cord-aes-cipher)** - Hardware-accelerated AES implementation
- **[PacketCord Core](https://github.com/packetcord/core)** - Main framework orchestration
- **[PacketCord Labs](https://github.com/packetcord/labs)** - Testing and automation tools

---

## 🤝 Contributing

We welcome contributions! Please:

1. 🍴 Fork the repository
2. 🌟 Create a feature branch
3. 🧪 Add tests for new functionality
4. 📝 Update documentation
5. 🔄 Submit a pull request

For major changes, please open an issue first to discuss your proposed changes.

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## 🏷️ Keywords

`networking` `packet-processing` `dpdk` `xdp` `high-performance` `zero-copy` `embedded` `c` `nfv` `sdn` `protocol-analysis` `network-programming`

---

<div align="center">

**Built with ❤️ by the PacketCord Team**

[![GitHub Stars](https://img.shields.io/github/stars/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)
[![GitHub Forks](https://img.shields.io/github/forks/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)

</div>