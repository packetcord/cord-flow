#ifndef CORD_CONNECTX4_LX_FLOW_POINT_H
#define CORD_CONNECTX4_LX_FLOW_POINT_H

#include <flow_point/cord_flow_point.h>

#define CORD_CREATE_CONNECTX4_LX_FLOW_POINT CORD_CREATE_CONNECTX4_LX_FLOW_POINT_ON_HEAP
#define CORD_DESTROY_CONNECTX4_LX_FLOW_POINT CORD_DESTROY_CONNECTX4_LX_FLOW_POINT_ON_HEAP

#define CORD_CREATE_CONNECTX4_LX_FLOW_POINT_ON_HEAP(id, pci_addr, queue_id) \
    (CordFlowPoint *) NEW_ON_HEAP(CordConnectX4LxFlowPoint, id, pci_addr, queue_id)

#define CORD_CREATE_CONNECTX4_LX_FLOW_POINT_ON_STACK(id, pci_addr, queue_id)\
    (CordFlowPoint *) &NEW_ON_STACK(CordConnectX4LxFlowPoint, id, pci_addr, queue_id)

#define CORD_DESTROY_CONNECTX4_LX_FLOW_POINT_ON_HEAP(name) \
    do {                                                   \
        DESTROY_ON_HEAP(CordConnectX4LxFlowPoint, name);   \
    } while(0)

#define CORD_DESTROY_CONNECTX4_LX_FLOW_POINT_ON_STACK(name)\
    do {                                                   \
        DESTROY_ON_STACK(CordConnectX4LxFlowPoint, name);  \
    } while(0)

typedef enum
{
    CORD_MLX_ACTION_DROP,
    CORD_MLX_ACTION_FORWARD,
    CORD_MLX_ACTION_REDIRECT_TO_TABLE,
    CORD_MLX_ACTION_REDIRECT_TO_QUEUE,
    CORD_MLX_ACTION_MIRROR,
    CORD_MLX_ACTION_MARK,
    CORD_MLX_ACTION_MODIFY_HEADER,
    CORD_MLX_ACTION_ENCAP,
    CORD_MLX_ACTION_DECAP
} CordMellanoxFlowAction;

typedef enum
{
    CORD_MLX_MATCH_ETH_SRC,
    CORD_MLX_MATCH_ETH_DST,
    CORD_MLX_MATCH_ETH_TYPE,
    CORD_MLX_MATCH_VLAN_ID,
    CORD_MLX_MATCH_IPV4_SRC,
    CORD_MLX_MATCH_IPV4_DST,
    CORD_MLX_MATCH_IPV6_SRC,
    CORD_MLX_MATCH_IPV6_DST,
    CORD_MLX_MATCH_IP_PROTOCOL,
    CORD_MLX_MATCH_TCP_SRC_PORT,
    CORD_MLX_MATCH_TCP_DST_PORT,
    CORD_MLX_MATCH_UDP_SRC_PORT,
    CORD_MLX_MATCH_UDP_DST_PORT,
    CORD_MLX_MATCH_TUNNEL_ID,
    CORD_MLX_MATCH_GTP_TEID
} CordMellanoxMatchField;

typedef struct
{
    CordMellanoxMatchField field;
    uint64_t value;
    uint64_t mask;
    bool enabled;
} CordMellanoxMatch;

typedef struct
{
    CordMellanoxFlowAction action;
    uint32_t queue_id;
    uint32_t table_id;
    uint32_t mark_value;
    bool enabled;
} CordMellanoxAction;

typedef struct
{
    uint32_t flow_id;
    uint32_t priority;
    uint32_t table_id;
    CordMellanoxMatch matches[16];
    uint8_t num_matches;
    CordMellanoxAction actions[8];
    uint8_t num_actions;
    uint64_t packet_count;
    uint64_t byte_count;
    bool active;
} CordMellanoxFlowRule;

typedef struct CordConnectX4LxFlowPoint
{
    CordFlowPoint base;
    char *pci_address;
    uint16_t queue_id;
    uint16_t num_queues;
    uint32_t port_id;
    bool sriov_enabled;
    uint8_t num_vfs;
    CordMellanoxFlowRule flow_rules[256];
    uint16_t num_flow_rules;
    uint32_t max_flow_rules;
    void *mlx_context;
    void *mlx_pd;
    void *mlx_cq;
    void *mlx_qp;
    bool rss_enabled;
    uint8_t rss_key[40];
    uint16_t rss_indirection_table[128];
    void *hw_params;
} CordConnectX4LxFlowPoint;

void CordConnectX4LxFlowPoint_ctor(CordConnectX4LxFlowPoint * const self,
                                   uint8_t id,
                                   const char *pci_address,
                                   uint16_t queue_id);

void CordConnectX4LxFlowPoint_dtor(CordConnectX4LxFlowPoint * const self);

cord_retval_t CordConnectX4LxFlowPoint_create_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t priority,
                                                        uint32_t table_id,
                                                        uint32_t *flow_id);

cord_retval_t CordConnectX4LxFlowPoint_destroy_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                         uint32_t flow_id);

cord_retval_t CordConnectX4LxFlowPoint_add_match_field(CordConnectX4LxFlowPoint * const self,
                                                       uint32_t flow_id,
                                                       CordMellanoxMatchField field,
                                                       uint64_t value,
                                                       uint64_t mask);

cord_retval_t CordConnectX4LxFlowPoint_add_action(CordConnectX4LxFlowPoint * const self,
                                                  uint32_t flow_id,
                                                  CordMellanoxFlowAction action,
                                                  uint32_t param);

cord_retval_t CordConnectX4LxFlowPoint_commit_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id);

cord_retval_t CordConnectX4LxFlowPoint_configure_rss(CordConnectX4LxFlowPoint * const self,
                                                     const uint8_t *rss_key,
                                                     size_t key_len,
                                                     const uint16_t *indirection_table,
                                                     size_t table_size);

cord_retval_t CordConnectX4LxFlowPoint_enable_sriov(CordConnectX4LxFlowPoint * const self,
                                                    uint8_t num_vfs);

cord_retval_t CordConnectX4LxFlowPoint_set_queue_count(CordConnectX4LxFlowPoint * const self,
                                                       uint16_t num_queues);

cord_retval_t CordConnectX4LxFlowPoint_query_flow_stats(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id,
                                                        uint64_t *packet_count,
                                                        uint64_t *byte_count);

cord_retval_t CordConnectX4LxFlowPoint_reset_flow_stats(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id);

uint16_t CordConnectX4LxFlowPoint_get_queue_id(const CordConnectX4LxFlowPoint * const self);
uint16_t CordConnectX4LxFlowPoint_get_num_queues(const CordConnectX4LxFlowPoint * const self);
uint16_t CordConnectX4LxFlowPoint_get_num_flow_rules(const CordConnectX4LxFlowPoint * const self);
const char* CordConnectX4LxFlowPoint_get_pci_address(const CordConnectX4LxFlowPoint * const self);

#endif // CORD_CONNECTX4_LX_FLOW_POINT_H