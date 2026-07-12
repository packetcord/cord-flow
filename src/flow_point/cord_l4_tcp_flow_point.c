#include "cord_retval.h"
#include <flow_point/cord_l4_tcp_flow_point.h>
#include <cord_error.h>
#include <linux/filter.h>
#include <sys/socket.h>
#include <poll.h>

static cord_retval_t CordL4TcpFlowPoint_rx_(CordL4TcpFlowPoint * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4TcpFlowPoint] rx()\n");
#endif
    if (self->server_mode) // Server mode
    {
        if (cord_unlikely(self->server_mode_tcp_connection_state != CORD_TCP_CONNECTED))
        {
            socklen_t addr_len = sizeof(self->src_addr_in);
            self->connected_client_sock_fd = accept(self->base.io_handle, (struct sockaddr *)&self->src_addr_in, &addr_len);

            if (self->connected_client_sock_fd >= 0)
            {
                if (fcntl(self->connected_client_sock_fd, F_SETFL, O_NONBLOCK) < 0)
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] rx(): fcntl()");
                    CORD_CLOSE(self->connected_client_sock_fd);
                    self->connected_client_sock_fd = -1;
                    return CORD_ERR;
                }

                self->server_mode_tcp_connection_state = CORD_TCP_CONNECTED;
            }
            else
            {
                if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
                {
                    return CORD_ERR_AGAIN;
                }
                else
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] rx(): Error on accept()");
                    return CORD_ERR;
                }
            }
        }
        else
        {
            *rx_bytes = recv(self->connected_client_sock_fd, buffer, len, 0);

            if (*rx_bytes == 0)
            {
                close(self->connected_client_sock_fd);
                self->connected_client_sock_fd = -1;
                self->server_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;

                return CORD_ERR_AGAIN;
            }

            if (*rx_bytes < 0)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    return CORD_ERR_AGAIN;
                }

                CORD_ERROR("[CordL4TcpFlowPoint] recv()");
                return CORD_ERR;
            }
        }
    }
    else // Client mode
    {
        if (self->client_mode_tcp_connection_state != CORD_TCP_CONNECTED)
        {
            if (self->client_mode_tcp_connection_state == CORD_TCP_DISCONNECTED)
            {
                int ret = connect(self->base.io_handle, (struct sockaddr *)&(self->dst_addr_in), sizeof(self->dst_addr_in));
                if (ret == 0)
                {
                    self->client_mode_tcp_connection_state = CORD_TCP_CONNECTED;
                }
                else if (errno == EINPROGRESS)
                {
                    self->client_mode_tcp_connection_state = CORD_TCP_CONNECTING;
                    return CORD_ERR_AGAIN;
                }
                else
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] rx connect()");
                    self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
                    return CORD_ERR;
                }
            }

            if (self->client_mode_tcp_connection_state == CORD_TCP_CONNECTING)
            {
                struct pollfd pfd = {
                    .fd = self->base.io_handle,
                    .events = POLLOUT
                };

                int ret = poll(&pfd, 1, 0);

                if (ret < 0)
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] rx poll()");
                    return CORD_ERR;
                }
                else if (ret > 0)
                {
                    int err = 0;
                    socklen_t err_len = sizeof(err);
                    if (getsockopt(self->base.io_handle, SOL_SOCKET, SO_ERROR, &err, &err_len) < 0)
                    {
                        CORD_ERROR("[CordL4TcpFlowPoint] rx getsockopt");
                        return CORD_ERR;
                    }
                    else if (err == 0)
                    {
                        self->client_mode_tcp_connection_state = CORD_TCP_CONNECTED;
                    }
                    else
                    {
                        errno = err;
                        CORD_ERROR("[CordL4TcpFlowPoint] rx connect failed via poll");
                        self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
                        return CORD_ERR;
                    }
                }
                else
                {
                    return CORD_ERR_AGAIN; // Connection still in progress
                }
            }
        }

        *rx_bytes = recv(self->base.io_handle, buffer, len, 0);
        if (*rx_bytes < 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                return CORD_ERR_AGAIN;
            }

            CORD_ERROR("[CordL4TcpFlowPoint] recv()");
            return CORD_ERR;
        }
        else if (*rx_bytes == 0) // Handle graceful remote disconnect
        {
            self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
            return CORD_ERR;
        }
    }

    return CORD_OK;
}

static cord_retval_t CordL4TcpFlowPoint_tx_(CordL4TcpFlowPoint * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4TcpFlowPoint] tx()\n");
#endif
    if (self->server_mode) // Server mode
    {
        if (self->connected_client_sock_fd < 0) // Client not connected to our server
        {
            return CORD_ERR;
        }

        *tx_bytes = send(self->connected_client_sock_fd, buffer, len, 0);
        if (*tx_bytes < 0)
        {
            CORD_ERROR("[CordL4TcpFlowPoint] send()");
        }
    }
    else // Client mode
    {
        switch(self->client_mode_tcp_connection_state)
        {
            case CORD_TCP_DISCONNECTED:
            {
                int ret = connect(self->base.io_handle, (struct sockaddr *)&(self->dst_addr_in), sizeof(self->dst_addr_in));
                if (ret == 0)
                {
                    self->client_mode_tcp_connection_state = CORD_TCP_CONNECTED;
                }
                if (ret < 0)
                {
                    if (errno == EINPROGRESS)
                    {
                        self->client_mode_tcp_connection_state = CORD_TCP_CONNECTING;
                    }
                    else
                    {
                        CORD_ERROR("[CordL4TcpFlowPoint] connect()");
                        self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
                    }
                }
                break;
            }

            case CORD_TCP_CONNECTING:
            {
                struct pollfd pfd = {
                    .fd = self->base.io_handle,
                    .events = POLLOUT
                };

                int ret = poll(&pfd, 1, 0);

                if (ret < 0)
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] poll()");
                }
                else if (ret > 0)
                {
                    int err = 0;
                    socklen_t err_len = sizeof(err);

                    if (getsockopt(self->base.io_handle, SOL_SOCKET, SO_ERROR, &err, &err_len) < 0)
                    {
                        CORD_ERROR("[CordL4TcpFlowPoint] getsockopt(SO_ERROR)");
                    }
                    else if (err == 0)
                    {
                        self->client_mode_tcp_connection_state = CORD_TCP_CONNECTED;
                    }
                    else
                    {
                        errno = err;
                        CORD_ERROR("[CordL4TcpFlowPoint] connect()");
                        self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
                    }
                }
                break;
            }

            case CORD_TCP_CONNECTED:
            {
                *tx_bytes = send(self->base.io_handle, buffer, len, 0);
                if (*tx_bytes < 0)
                {
                    CORD_ERROR("[CordL4TcpFlowPoint] send()");
                    self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
                }
                break;
            }
        }
    }

    return CORD_OK;
}

static cord_retval_t CordL4TcpFlowPoint_attach_xBPF_(CordL4TcpFlowPoint const * const self, void *filter, void *params)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4TcpFlowPoint] attach_xBPF()\n");
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
	                CORD_ERROR("[CordL4TcpFlowPoint] setsockopt attach cBPF filter");
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
                    CORD_ERROR("[CordL4TcpFlowPoint] setsockopt attach eBPF filter");
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

void CordL4TcpFlowPoint_ctor(CordL4TcpFlowPoint * const self,
                             uint8_t id,
                             in_addr_t ipv4_src_addr,
                             in_addr_t ipv4_dst_addr,
                             uint16_t src_port,
                             uint16_t dst_port,
                             bool server_mode)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4TcpFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *rx_bytes))&CordL4TcpFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint * const self, uint16_t queue_id, void *buffer, size_t len, ssize_t *tx_bytes))&CordL4TcpFlowPoint_tx_,
        .attach_xBPF = (cord_retval_t (*)(CordFlowPoint const * const self, void *filter, void *params))&CordL4TcpFlowPoint_attach_xBPF_,
        .cleanup = (void     (*)(CordFlowPoint const * const))&CordL4TcpFlowPoint_dtor,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    self->ipv4_src_addr = ipv4_src_addr;
    self->ipv4_dst_addr = ipv4_dst_addr;

    inet_pton(AF_INET6, "::1", &(self->ipv6_src_addr));
    inet_pton(AF_INET6, "::1", &(self->ipv6_dst_addr));

    self->src_port = src_port;
    self->dst_port = dst_port;

    self->server_mode = server_mode;
    self->connected_client_sock_fd = -1;

    self->base.io_handle = socket(AF_INET, SOCK_STREAM, 0);
    if (self->base.io_handle < 0)
    {
        CORD_ERROR("[CordL4TcpFlowPoint] socket()");
        CORD_EXIT(EXIT_FAILURE);
    }

    self->src_addr_in.sin_family = AF_INET;
    self->src_addr_in.sin_addr.s_addr = self->ipv4_src_addr;
    self->src_addr_in.sin_port = htons(self->src_port);

    self->dst_addr_in.sin_family = AF_INET;
    self->dst_addr_in.sin_addr.s_addr = self->ipv4_dst_addr;
    self->dst_addr_in.sin_port = htons(self->dst_port);

    self->client_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;
    self->server_mode_tcp_connection_state = CORD_TCP_DISCONNECTED;

    int reuse = 1;
    if (setsockopt(self->base.io_handle, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        CORD_ERROR("[CordL4TcpFlowPoint] setsockopt(SO_REUSEADDR)");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    if (self->server_mode) // Server mode
    {
        if (self->src_port == 0)
        {
            CORD_ERROR("[CordL4TcpFlowPoint] Server mode requires a port");
            CORD_CLOSE(self->base.io_handle);
            CORD_EXIT(EXIT_FAILURE);
        }

        if (bind(self->base.io_handle, (struct sockaddr *)&self->src_addr_in, sizeof(self->src_addr_in)) < 0)
        {
            CORD_ERROR("[CordL4TcpFlowPoint] bind()");
            CORD_CLOSE(self->base.io_handle);
            CORD_EXIT(EXIT_FAILURE);
        }

        CORD_LOG("[CordL4TcpFlowPoint] Successfully bound to port %d\n", self->src_port);
    }
    else // Client mode
    {
        if ((self->ipv4_src_addr != 0) || (self->src_port != 0)) // Either source IPv4 or port specified for explicit client bind
        {
            if (bind(self->base.io_handle, (struct sockaddr *)&self->src_addr_in, sizeof(self->src_addr_in)) < 0)
            {
                CORD_ERROR("[CordL4TcpFlowPoint] bind()");
                CORD_CLOSE(self->base.io_handle);
                CORD_EXIT(EXIT_FAILURE);
            }

            CORD_LOG("[CordL4TcpFlowPoint] Successfully bound to port %d\n", self->src_port);
        }
    }

    if (fcntl(self->base.io_handle, F_SETFL, O_NONBLOCK) < 0)
    {
        CORD_ERROR("[CordL4TcpFlowPoint] fcntl()");
        CORD_CLOSE(self->base.io_handle);
        CORD_EXIT(EXIT_FAILURE);
    }

    if (self->server_mode) // Server mode
    {
        const int max_pending_connections = 1;
        if (listen(self->base.io_handle, max_pending_connections) < 0)
        {
            CORD_ERROR("[CordL4TcpFlowPoint] listen()");
            CORD_CLOSE(self->base.io_handle);
            CORD_EXIT(EXIT_FAILURE);
        }
    }
    else
    {
        // Client mode
    }
}

void CordL4TcpFlowPoint_dtor(CordL4TcpFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordL4TcpFlowPoint] dtor()\n");
#endif
    if (self->connected_client_sock_fd > 0)
        CORD_CLOSE(self->connected_client_sock_fd);
    
    CORD_CLOSE(self->base.io_handle);
    free(self);
}
