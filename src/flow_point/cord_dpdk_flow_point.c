#ifdef ENABLE_DPDK_DATAPLANE

#include <flow_point/cord_dpdk_flow_point.h>
#include <cord_error.h>
#include <rte_eal.h>
#include <rte_errno.h>
#include <string.h>
#include <stdbool.h>

static cord_retval_t CordDpdkFlowPoint_rx_(CordDpdkFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] rx()\n");
#endif
    //
    // Implement the DPDK rx() logic
    //
    return CORD_OK;
}

static cord_retval_t CordDpdkFlowPoint_tx_(CordDpdkFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] tx()\n");
#endif
    //
    // Implement the DPDK tx() logic
    //

    return CORD_OK;
}

void CordDpdkFlowPoint_ctor(CordDpdkFlowPoint * const self,
    uint8_t id,
    uint16_t port_id,
    uint16_t queue_count,
    uint16_t queue_size,
    uint8_t cpu_mask,
    struct rte_mempool *mbuf_pool)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] ctor()\n");
#endif

    static const CordFlowPointVtbl vtbl_base = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordDpdkFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordDpdkFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl_base;
    self->port_id = port_id;
    self->queue_count = queue_count;
    self->queue_size = queue_size;
    self->cpu_mask = cpu_mask;
    self->mbuf_pool = mbuf_pool;

    struct rte_eth_conf port_conf;
	struct rte_eth_dev_info dev_info;
	struct rte_eth_txconf txconf;
    int retval;

	if (!rte_eth_dev_is_valid_port(self->port_id))
		CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_dev_is_valid_port()");

	memset(&port_conf, 0, sizeof(struct rte_eth_conf));

	retval = rte_eth_dev_info_get(self->port_id, &dev_info);
	if (retval != 0)
		CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_dev_info_get()");

	if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE)
		port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;

	retval = rte_eth_dev_configure(self->port_id, self->queue_count, self->queue_count, &port_conf);
	if (retval != 0)
        CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_dev_configure()");

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(self->port_id, &(self->queue_size), &(self->queue_size));
	if (retval != 0)
        CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_dev_adjust_nb_rx_tx_desc()");

    txconf = dev_info.default_txconf;
	txconf.offloads = port_conf.txmode.offloads;

	for (uint16_t q = 0; q < self->queue_count; q++) {
		// RX queues setup
        retval = rte_eth_rx_queue_setup(self->port_id, q, self->queue_size, rte_eth_dev_socket_id(self->port_id), NULL, self->mbuf_pool);
		if (retval < 0)
            CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_rx_queue_setup()");;
        
        // TX queues setup
        retval = rte_eth_tx_queue_setup(self->port_id, q, self->queue_size, rte_eth_dev_socket_id(self->port_id), &txconf);
        if (retval < 0)
            CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_tx_queue_setup()");;
	}

	retval = rte_eth_dev_start(self->port_id);
	if (retval < 0)
        CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_dev_start()");;

	struct rte_ether_addr addr;
	retval = rte_eth_macaddr_get(self->port_id, &addr);
	if (retval != 0)
        CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_macaddr_get()");;

	CORD_LOG("[CordDpdkFlowPoint] ctor(): Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			self->port_id, RTE_ETHER_ADDR_BYTES(&addr));

	retval = rte_eth_promiscuous_enable(self->port_id);
	if (retval != 0)
        CORD_ERROR("[CordDpdkFlowPoint] ctor(): rte_eth_promiscuous_enable()");;
}

void CordDpdkFlowPoint_dtor(CordDpdkFlowPoint * const self)
{
#ifdef CORD_FLOW_POINT_LOG
    CORD_LOG("[CordDpdkFlowPoint] dtor()\n");
#endif
    free(self);
}

#endif
