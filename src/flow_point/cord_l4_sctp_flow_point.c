#include <flow_point/cord_l4_sctp_flow_point.h>
#include <cord_error.h>
#include <linux/filter.h>

static cord_retval_t CordL4SctpFlowPoint_rx_(CordL4SctpFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4SctpFlowPoint] rx()\n");
#endif
    //
    //  Implement the rx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4SctpFlowPoint_tx_(CordL4SctpFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4SctpFlowPoint] tx()\n");
#endif
    //
    // Implement the tx() logic
    //

    return CORD_OK;
}

static cord_retval_t CordL4SctpFlowPoint_attach_xBPF_(CordL4SctpFlowPoint const * const self, void *filter, void *params)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4SctpFlowPoint] attach_xBPF()\n");
#endif
    if (filter != NULL)
    {
        if (params != NULL)
        {
            cord_filter_type_t filter_type = *((cord_filter_type_t *)params);
            if (filter_type == CBPF_FILTER)
            {
                // The filter variable holds a pointer to (struct sock_fprog)
                struct sock_fprog * const filter_fprog = (struct sock_fprog *)filter;

                if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_ATTACH_FILTER, filter_fprog, sizeof(struct sock_fprog)) < 0)
                {
	                CORD_ERROR("[CordL4SctpFlowPoint] setsockopt attach cBPF filter");
	                CORD_CLOSE(self->base.io_handle);
	                CORD_EXIT(EXIT_FAILURE);
                }
            }
            else if (filter_type == EBPF_FILTER)
            {
                // The filter variable holds the file descriptor (int) to the eBPF program
                int ebpf_filter_program_fd = *((int *)filter);

                if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_ATTACH_BPF, &ebpf_filter_program_fd, sizeof(ebpf_filter_program_fd)) < 0)
                {
                    CORD_ERROR("[CordL4SctpFlowPoint] setsockopt attach eBPF filter");
	                CORD_CLOSE(self->base.io_handle);
	                CORD_EXIT(EXIT_FAILURE);
                }
            }
            else 
            {
                return CORD_ERR;
            }
        }
    }

    return CORD_OK;
}

void CordL4SctpFlowPoint_ctor(CordL4SctpFlowPoint * const self,
                              uint8_t id,
                              in_addr_t ipv4_src_addr,
                              in_addr_t ipv4_dst_addr,
                              uint16_t src_port,
                              uint16_t dst_port)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4SctpFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes))&CordL4SctpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes))&CordL4SctpFlowPoint_tx_,
        .attach_xBPF = (cord_retval_t (*)(CordFlowPoint const * const self, void *filter, void *params))&CordL4SctpFlowPoint_attach_xBPF_,
        .cleanup = (void     (*)(CordFlowPoint const * const))&CordL4SctpFlowPoint_dtor,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->ipv4_src_addr = ipv4_src_addr;
    self->ipv4_dst_addr = ipv4_dst_addr;

    inet_pton(AF_INET6, "::1", &(self->ipv6_src_addr));
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));

    self->src_port = src_port;
    self->dst_port = dst_port;
}

void CordL4SctpFlowPoint_dtor(CordL4SctpFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4SctpFlowPoint] dtor()\n");
#endif
    close(self->base.io_handle);
    free(self);
}
