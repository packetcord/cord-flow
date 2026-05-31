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

- DPDK FlowPoint
- L2 Custom FlowPoint
- L2 Raw Socket FlowPoint
- L2 TPACKETv3 FlowPoint
- L3 Raw Socket FlowPoint
- L3 Stack Inject FlowPoint
- L4 UDP FlowPoint
- L4 TCP FlowPoint
- L4 SCTP FlowPoint
- XDP FlowPoint

### EventHandler
The CORD-FLOW library relies on the Linux API epoll() event notification mechanism and the DPDK RTE_ETH_FOREACH_DEV (port) loop to handle the input packets entering a flow point. In addition to this, there is also a skeleton for implementing a custom event handler.

### xBPF implementation

| Feature / Technology | Section Type | Libraries | Status |
| :--- | :--- | :--- | :--- |
| **cBPF** | *N/A* | *N/A* | Implemented |
| **eBPF over L2 (raw)/L3/L4 socket** | `SEC("socket")` | libbpf | Implemented |
| **eBPF over AF_XDP socket via XSK_MAP** | `SEC("xdp_sock")` | libxdp, libbpf | Planned |
| **eBPF over hardware interface via custom FlowPoint** *(instead of `ip link`)* | `SEC("xdp")` | libxdp, libbpf | Planned |
| **eBPF over DPDK device/interface/port** | *N/A* | libbpf | Planned |
---

## Build Instructions

### Prerequisites
- **CMake** 3.16 or higher
- **GCC** with C23 support
- **Linux** development headers

### Building CORD-FLOW
```bash
git clone https://github.com/packetcord/cord-flow.git
cd cord-flow
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

<div align="center">

[![GitHub Stars](https://img.shields.io/github/stars/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)
[![GitHub Forks](https://img.shields.io/github/forks/packetcord/cord-flow?style=social)](https://github.com/packetcord/cord-flow)

</div>