# CORD-FLOW

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C Standard](https://img.shields.io/badge/C-C23-blue.svg)](https://en.wikipedia.org/wiki/C23_%28C_standard_revision%29)
[![Build System](https://img.shields.io/badge/Build-CMake-brightgreen.svg)](https://cmake.org/)
[![Architecture](https://img.shields.io/badge/Arch-x86__64%20%7C%20ARM64%20%7C%20ARM32-orange.svg)](https://github.com/packetcord/cord-flow)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Embedded-lightgrey.svg)](https://github.com/packetcord/cord-flow)

**High-Performance Packet Flow Programming Framework**

A modular component of the PacketCord.io ecosystem, providing comprehensive abstractions for packet processing with receive-match-action-transmit logic. Designed for cross-platform deployment from high-end servers to resource-constrained embedded systems and specialized network hardware.

---

## Components

### FlowPoint
Network ingress/egress abstractions supporting multiple backend technologies:

| Component | Status | Description |
|-----------|--------|-------------|
| `cord_l2_raw_socket_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | Raw L2 Ethernet sockets |
| `cord_l3_raw_socket_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | Raw L3 IP sockets |
| `cord_l3_stack_inject_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | OS IP stack integration |
| `cord_l4_udp_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | UDP client/server endpoints |
| `cord_l4_tcp_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | TCP client/server endpoints |
| `cord_l4_sctp_flow_point` | ![In Progress](https://img.shields.io/badge/Status-In_Progress-orange) | SCTP client/server endpoints |
| `cord_dpdk_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | DPDK ports |
| `cord_xdp_flow_point` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | XDP (AF_XDP) sockets |
| `cord_connectx4_lx_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | ConnectX-4 Lx NIC support |
| `cord_sdr_flow_point` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | Software Defined Radio |

### EventHandler
Efficient event processing:

| Component | Status | Description |
|-----------|--------|-------------|
| `cord_linux_api_event_handler` | ![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen) | epoll-based event handling |
| `cord_dpdk_event_handler` | ![Planned](https://img.shields.io/badge/Status-Planned-red) | DPDK event integration |

### Memory Management
![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen)

Advanced memory allocation optimized for packet processing:

- **Huge Pages**
- **Memory Alignment**
- **Zero-Copy Buffers**
- **PACKET_MMAP**
- **XDP Buffers**

### Protocol Headers
![In Progress](https://img.shields.io/badge/Status-In_Progress-orange)

Comprehensive protocol support with portable definitions.

### Connection Tracking
![In Progress](https://img.shields.io/badge/Status-In_Progress-orange)

Stateful and stateless connection monitoring:

- **Hash-based tracking** - Efficient connection state management
- **TCP payload coalescing** - Reassembly of fragmented streams

### Table Management
![Planned](https://img.shields.io/badge/Status-Planned-red)

Forwarding, NAT-like tables, RIBs and FIBs.

### Match Engine
![Implemented](https://img.shields.io/badge/Status-Implemented-brightgreen)

Flexible packet matching capabilities

### Action Engine
![Planned](https://img.shields.io/badge/Status-Planned-red)

Comprehensive packet manipulation:

- **Header manipulation** - Add, remove, modify protocol headers
- **Field modification** - Set/unset values in header fields
- **Packet transformation** - Complete packet restructuring

### Filter System
![Planned](https://img.shields.io/badge/Status-Planned-red)

Advanced packet filtering with xBPF integration:

- **Classic BPF** - Traditional Berkeley Packet Filter support
- **Extended BPF** - Modern eBPF programs (XDP, TC, socket-attached)
- **Hardware offload** - Filter acceleration where available

---

## Build Instructions

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

---

## Related Projects

CORD-FLOW is part of the **[PacketCord.io](https://github.com/packetcord/packetcord.io)** framework, which comprises three main components:

- **[CORD-FLOW](https://github.com/packetcord/cord-flow)** - High-performance packet flow programming (this repository)
- **[CORD-CRYPTO](https://github.com/packetcord/cord-crypto)** - Cryptographic operations and security primitives
  - **[cord-aes-cipher](https://github.com/packetcord/cord-aes-cipher)** - Hardware-accelerated AES implementation
- **[CORD-CRAFT](https://github.com/packetcord/cord-craft)** - Network packet crafting and generation tools

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## Keywords

`networking` `packet-processing` `dpdk` `xdp` `high-performance` `zero-copy` `embedded` `c` `nfv` `sdn` `protocol-analysis` `network-programming`

---

<div align="center">

**Built with ❤️ by the PacketCord Team**

[![GitHub Stars](https://img.shields.io/github/stars/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)
[![GitHub Forks](https://img.shields.io/github/forks/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)

</div>