#include <flow_point/cord_l2_tpacketv3_flow_point.h>
#include <memory/cord_memory.h>
#include <cord_error.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

typedef struct
{
    struct tpacket_block_desc *current_block;
    struct tpacket3_hdr *current_packet;
    unsigned int packets_remaining;
} RxState;

static RxState rx_state = {0};

static cord_retval_t CordL2Tpacketv3FlowPoint_rx_(CordL2Tpacketv3FlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] rx()\n");
#endif

    (void)queue_id;

    while (1)
    {
        if (rx_state.packets_remaining == 0)
        {
            struct tpacket_block_desc *pbd = (struct tpacket_block_desc *)self->ring[self->block_idx].iov_base;

            if (!(pbd->hdr.bh1.block_status & TP_STATUS_USER))
            {
                *rx_bytes = -1;
                return CORD_ERR;
            }

            rx_state.current_block = pbd;
            rx_state.packets_remaining = pbd->hdr.bh1.num_pkts;
            rx_state.current_packet = (struct tpacket3_hdr *)((uint8_t *)pbd + pbd->hdr.bh1.offset_to_first_pkt);
        }

        if (rx_state.packets_remaining > 0)
        {
            struct tpacket3_hdr *hdr = rx_state.current_packet;
            uint8_t *pkt_data = (uint8_t *)hdr + hdr->tp_mac;
            size_t pkt_len = hdr->tp_snaplen;

            if (pkt_len > len)
            {
                CORD_ERROR("[CordL2Tpacketv3FlowPoint] rx : packet too large for buffer");
                pkt_len = len;
            }

            memcpy(buffer, pkt_data, pkt_len);
            *rx_bytes = pkt_len;

            socklen_t addr_len = sizeof(self->anchor_bind_addr);
            struct sockaddr_ll *sll = (struct sockaddr_ll *)&self->anchor_bind_addr;
            sll->sll_pkttype = hdr->tp_status & TP_STATUS_SEND_REQUEST ? PACKET_OUTGOING : PACKET_HOST;

            rx_state.packets_remaining--;

            if (rx_state.packets_remaining > 0)
            {
                rx_state.current_packet = (struct tpacket3_hdr *)((uint8_t *)rx_state.current_packet + rx_state.current_packet->tp_next_offset);
            }
            else
            {
                rx_state.current_block->hdr.bh1.block_status = TP_STATUS_KERNEL;
                CordL2Tpacketv3FlowPoint *mutable_self = (CordL2Tpacketv3FlowPoint *)self;
                mutable_self->block_idx = (mutable_self->block_idx + 1) % mutable_self->req.tp_block_nr;
                rx_state.current_block = NULL;
                rx_state.current_packet = NULL;
            }

            return CORD_OK;
        }
    }
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
    self->block_size = block_size;
    self->frame_size = frame_size;
    self->block_num = block_num;
    self->frame_num = (block_size * block_num) / frame_size;

    assert(self->frame_num == (block_size * block_num) / frame_size);

    self->base.io_handle = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    int version = TPACKET_V3;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_VERSION)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    int qdisc_bypass = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_QDISC_BYPASS, &qdisc_bypass, sizeof(qdisc_bypass)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_QDISC_BYPASS)");
    }

    memset(&self->req, 0, sizeof(self->req));
    self->req.tp_block_size = block_size;
    self->req.tp_frame_size = frame_size;
    self->req.tp_block_nr = block_num;
    self->req.tp_frame_nr = self->frame_num;
    self->req.tp_retire_blk_tov = 1;
    self->req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;

    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_RX_RING, &self->req, sizeof(self->req)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_RX_RING)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    self->map_size = self->req.tp_block_size * self->req.tp_block_nr;
    self->map = mmap(NULL, self->map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_POPULATE, self->base.io_handle, 0);
    if (self->map == MAP_FAILED)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] mmap()");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    self->ring = cord_cache_aligned_malloc(self->req.tp_block_nr * sizeof(struct iovec));
    if (!self->ring)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] cord_cache_aligned_malloc()");
        munmap(self->map, self->map_size);
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < self->req.tp_block_nr; i++)
    {
        self->ring[i].iov_base = self->map + (i * self->req.tp_block_size);
        self->ring[i].iov_len = self->req.tp_block_size;
    }

    self->block_idx = 0;

    struct ifreq anchor_iface_req;
    memset(&anchor_iface_req, 0, sizeof(struct ifreq));
    strncpy(anchor_iface_req.ifr_name, self->anchor_iface_name, IFNAMSIZ);
    if (ioctl(self->base.io_handle, SIOCGIFINDEX, &anchor_iface_req) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] ioctl(SIOCGIFINDEX)");
        cord_cache_aligned_free(self->ring);
        munmap(self->map, self->map_size);
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }
    self->ifindex = anchor_iface_req.ifr_ifindex;

    memset(&self->anchor_bind_addr, 0, sizeof(struct sockaddr_ll));
    self->anchor_bind_addr.sll_family = AF_PACKET;
    self->anchor_bind_addr.sll_protocol = htons(ETH_P_ALL);
    self->anchor_bind_addr.sll_ifindex = anchor_iface_req.ifr_ifindex;

    if (bind(self->base.io_handle, (struct sockaddr *)&self->anchor_bind_addr, sizeof(struct sockaddr_ll)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] bind()");
        cord_cache_aligned_free(self->ring);
        munmap(self->map, self->map_size);
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(self->base.io_handle, SOL_PACKET, PACKET_IGNORE_OUTGOING, &enable, sizeof(enable)) < 0)
    {
        CORD_ERROR("[CordL2Tpacketv3FlowPoint] setsockopt(PACKET_IGNORE_OUTGOING)");
    }

    fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK);

    memset(&rx_state, 0, sizeof(rx_state));
}

void CordL2Tpacketv3FlowPoint_dtor(CordL2Tpacketv3FlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL2Tpacketv3FlowPoint] dtor()\n");
#endif

    if (self->ring)
    {
        cord_cache_aligned_free(self->ring);
        self->ring = NULL;
    }

    if (self->map && self->map != MAP_FAILED)
    {
        munmap(self->map, self->map_size);
        self->map = NULL;
    }

    if (self->base.io_handle >= 0)
    {
        close(self->base.io_handle);
        self->base.io_handle = -1;
    }

    free(self);
}
