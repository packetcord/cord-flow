#include <flow_point/cord_l2_tpacketv3_flow_point.h>
#include <memory/cord_memory.h>
#include <cord_error.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <arpa/inet.h>

static cord_retval_t CordL2Tpacketv3FlowPoint_rx_(CordL2Tpacketv3FlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_packets)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] rx()\n");
#endif

    (void)queue_id;

    struct cord_tpacketv3_ring *ring = *(struct cord_tpacketv3_ring **)buffer;
    CordL2Tpacketv3FlowPoint *mutable_self = (CordL2Tpacketv3FlowPoint *)self;

    ssize_t count = 0;
    mutable_self->rx_start_block_idx = ring->block_idx;
    mutable_self->rx_num_blocks_processed = 0;

    unsigned int current_block_idx = ring->block_idx;

    while (count < (ssize_t)len)
    {
        struct tpacket_block_desc *pbd = (struct tpacket_block_desc *)ring->iov_ring[current_block_idx].iov_base;

        if (!(pbd->hdr.bh1.block_status & TP_STATUS_USER))
            break;

        unsigned int num_pkts = pbd->hdr.bh1.num_pkts;
        struct tpacket3_hdr *hdr = (struct tpacket3_hdr *)((uint8_t *)pbd + pbd->hdr.bh1.offset_to_first_pkt);

        for (unsigned int i = 0; i < num_pkts; i++)
        {
            count++;
            if (count >= (ssize_t)len)
                break;

            hdr = (struct tpacket3_hdr *)((uint8_t *)hdr + hdr->tp_next_offset);
        }

        mutable_self->rx_num_blocks_processed++;
        current_block_idx = (current_block_idx + 1) % ring->req.tp_block_nr;
    }

    mutable_self->rx_packet_count = count;
    *rx_packets = count;

    return CORD_OK;
}

static cord_retval_t CordL2Tpacketv3FlowPoint_tx_(CordL2Tpacketv3FlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_packets)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] tx()\n");
#endif

    (void)queue_id;

    struct cord_tpacketv3_ring *ring = *(struct cord_tpacketv3_ring **)buffer;

    ssize_t sent_count = 0;
    ssize_t packets_to_send = (ssize_t)len;

    while (sent_count < packets_to_send)
    {
        struct tpacket_block_desc *pbd = (struct tpacket_block_desc *)ring->iov_ring[ring->block_idx].iov_base;

        if (!(pbd->hdr.bh1.block_status & TP_STATUS_USER))
            break;

        unsigned int num_pkts = pbd->hdr.bh1.num_pkts;
        struct tpacket3_hdr *hdr = (struct tpacket3_hdr *)((uint8_t *)pbd + pbd->hdr.bh1.offset_to_first_pkt);

        for (unsigned int i = 0; i < num_pkts; i++)
        {
            uint8_t *pkt_data = (uint8_t *)hdr + hdr->tp_mac;
            uint32_t pkt_len = hdr->tp_snaplen;

            ssize_t ret = sendto(self->base.io_handle, pkt_data, pkt_len, MSG_DONTWAIT,
                                 (struct sockaddr *)&self->anchor_bind_addr, sizeof(self->anchor_bind_addr));
            if (ret > 0)
                sent_count++;

            if (sent_count >= packets_to_send)
                break;

            hdr = (struct tpacket3_hdr *)((uint8_t *)hdr + hdr->tp_next_offset);
        }

        pbd->hdr.bh1.block_status = TP_STATUS_KERNEL;
        ring->block_idx = (ring->block_idx + 1) % ring->req.tp_block_nr;
    }

    *tx_packets = sent_count;

    return CORD_OK;
}

static cord_retval_t CordL2Tpacketv3FlowPoint_attach_xBPF_filter_(CordL2Tpacketv3FlowPoint const * const self, void *filter)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] attach_xBPF_filter()\n");
#endif

    (void)self;
    (void)filter;

    return CORD_OK;
}

static cord_retval_t CordL2Tpacketv3FlowPoint_attach_xBPF_program_(CordL2Tpacketv3FlowPoint const * const self, void *program)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] attach_xBPF_program()\n");
#endif

    (void)self;
    (void)program;

    return CORD_OK;
}

void CordL2Tpacketv3FlowPoint_ctor(CordL2Tpacketv3FlowPoint * const self,
                                    uint8_t id,
                                    const char *anchor_iface_name,
                                    struct cord_tpacketv3_ring **rx_ring)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] ctor()\n");
#endif

    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_packets))&CordL2Tpacketv3FlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_packets))&CordL2Tpacketv3FlowPoint_tx_,
        .cleanup = (void (*)(CordFlowPoint const * const))&CordL2Tpacketv3FlowPoint_dtor,
    };

    static const CordL2Tpacketv3FlowPointVtbl vtbl_deriv = {
        .attach_xBPF_filter = (cord_retval_t (*)(CordFlowPoint const * const self, void *filter))&CordL2Tpacketv3FlowPoint_attach_xBPF_filter_,
        .attach_xBPF_program = (cord_retval_t (*)(CordFlowPoint const * const self, void *program))&CordL2Tpacketv3FlowPoint_attach_xBPF_program_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->vptr = &vtbl_deriv;
    self->anchor_iface_name = anchor_iface_name;
    self->rx_ring = rx_ring;
    self->rx_start_block_idx = 0;
    self->rx_num_blocks_processed = 0;
    self->rx_packet_count = 0;

    self->base.io_handle = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    int version = TPACKET_V3;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(TPACKET_V3)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    int qdisc_bypass = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_QDISC_BYPASS, &qdisc_bypass, sizeof(qdisc_bypass)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_QDISC_BYPASS)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_BINDTODEVICE, self->anchor_iface_name, strlen(self->anchor_iface_name)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(SO_BINDTODEVICE)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    struct ifreq anchor_iface_req;
    memset(&anchor_iface_req, 0, sizeof(struct ifreq));
    strncpy(anchor_iface_req.ifr_name, self->anchor_iface_name, IFNAMSIZ);
    if (ioctl(self->base.io_handle, SIOCGIFINDEX, &anchor_iface_req) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] ioctl(SIOCGIFINDEX)");
    }
    self->ifindex = anchor_iface_req.ifr_ifindex;

    memset(&(self->anchor_bind_addr), 0, sizeof(struct sockaddr_ll));
    self->anchor_bind_addr.sll_family = AF_PACKET;
    self->anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    self->anchor_bind_addr.sll_ifindex = anchor_iface_req.ifr_ifindex;
    if (bind(self->base.io_handle, (struct sockaddr *)&(self->anchor_bind_addr), sizeof(struct sockaddr_ll)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] bind()");
    }

    int ignore_outgoing = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_IGNORE_OUTGOING, &ignore_outgoing, sizeof(ignore_outgoing)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_IGNORE_OUTGOING)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }
    
    (*rx_ring)->fd = self->base.io_handle;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_RX_RING, &((*rx_ring)->req), sizeof((*rx_ring)->req)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_RX_RING)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    cord_tpacketv3_ring_init(self->rx_ring);

    fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK);
}

void CordL2Tpacketv3FlowPoint_dtor(CordL2Tpacketv3FlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] dtor()\n");
#endif

    cord_tpacketv3_ring_free(self->rx_ring);

    free(self);
}
