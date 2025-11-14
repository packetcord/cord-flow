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

struct cord_tpacketv3_ring_t
{
    int fd;
    struct iovec *ring;
    uint8_t *map;
    size_t map_size;
    struct tpacket_req3 req;
    unsigned int block_idx;
    char iface[IFNAMSIZ];

    struct tpacket_block_desc *current_block;
    struct tpacket3_hdr *current_packet;
    unsigned int packets_remaining;
};

static cord_retval_t CordL2Tpacketv3FlowPoint_rx_(CordL2Tpacketv3FlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] rx()\n");
#endif

    (void)queue_id;

    cord_tpacketv3_ring_t *ring = self->rx_ring;

    if (ring->packets_remaining == 0)
    {
        struct tpacket_block_desc *pbd = (struct tpacket_block_desc *)ring->ring[ring->block_idx].iov_base;

        if (!(pbd->hdr.bh1.block_status & TP_STATUS_USER))
        {
            *rx_bytes = -1;
            return CORD_ERR;
        }

        ring->current_block = pbd;
        ring->packets_remaining = pbd->hdr.bh1.num_pkts;
        ring->current_packet = (struct tpacket3_hdr *)((uint8_t *)pbd + pbd->hdr.bh1.offset_to_first_pkt);
    }

    struct tpacket3_hdr *hdr = ring->current_packet;
    uint8_t *pkt_data = (uint8_t *)hdr + hdr->tp_mac;
    size_t pkt_len = hdr->tp_snaplen;

    if (pkt_len > len)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] rx : packet too large for buffer");
        pkt_len = len;
    }

    memcpy(buffer, pkt_data, pkt_len);
    *rx_bytes = pkt_len;

    struct sockaddr_ll *sll = (struct sockaddr_ll *)&self->anchor_bind_addr;
    sll->sll_pkttype = hdr->tp_status & TP_STATUS_SEND_REQUEST ? PACKET_OUTGOING : PACKET_HOST;

    ring->packets_remaining--;

    if (ring->packets_remaining > 0)
    {
        ring->current_packet = (struct tpacket3_hdr *)((uint8_t *)ring->current_packet + ring->current_packet->tp_next_offset);
    }
    else
    {
        ring->current_block->hdr.bh1.block_status = TP_STATUS_KERNEL;
        ring->block_idx = (ring->block_idx + 1) % ring->req.tp_block_nr;
        ring->current_block = NULL;
        ring->current_packet = NULL;
    }

    return CORD_OK;
}

static cord_retval_t CordL2Tpacketv3FlowPoint_tx_(CordL2Tpacketv3FlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] tx()\n");
#endif

    (void)queue_id;

    socklen_t addr_len = sizeof(self->anchor_bind_addr);
    *tx_bytes = sendto(self->base.io_handle, buffer, len, MSG_DONTWAIT, (struct sockaddr *)&self->anchor_bind_addr, addr_len);

    if (*tx_bytes < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            CORD_ERROR("[CordL2Tpacketv3FlowPoint] tx : sendto()");
            return CORD_ERR;
        }
        return CORD_ERR;
    }

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
                                    uint32_t block_size,
                                    uint32_t frame_size,
                                    uint32_t block_num)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] ctor()\n");
#endif

    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes))&CordL2Tpacketv3FlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes))&CordL2Tpacketv3FlowPoint_tx_,
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

    self->rx_ring = cord_tpacketv3_ring_alloc(anchor_iface_name, block_size, frame_size, block_num, true);
    if (!self->rx_ring)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] cord_tpacketv3_ring_alloc()");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->base.io_handle = self->rx_ring->fd;
    self->ifindex = if_nametoindex(anchor_iface_name);

    memset(&self->anchor_bind_addr, 0, sizeof(struct sockaddr_ll));
    self->anchor_bind_addr.sll_family = AF_PACKET;
    self->anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    self->anchor_bind_addr.sll_ifindex = self->ifindex;
    self->anchor_bind_addr.sll_halen = ETH_ALEN;

    fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK);
}

void CordL2Tpacketv3FlowPoint_dtor(CordL2Tpacketv3FlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] dtor()\n");
#endif

    cord_tpacketv3_ring_free(self->rx_ring);
    self->rx_ring = NULL;

    free(self);
}
