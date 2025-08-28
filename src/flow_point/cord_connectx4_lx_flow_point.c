#include <flow_point/cord_connectx4_lx_flow_point.h>
#include <cord_error.h>

static cord_retval_t CordConnectX4LxFlowPoint_rx_(CordConnectX4LxFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] rx() on queue %u\n", self->queue_id);
#endif
    
    *rx_bytes = len;
    return CORD_OK;
}

static cord_retval_t CordConnectX4LxFlowPoint_tx_(CordConnectX4LxFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] tx() on queue %u\n", self->queue_id);
#endif
    
    *tx_bytes = len;
    return CORD_OK;
}

static uint32_t generate_flow_id(CordConnectX4LxFlowPoint const * const self)
{
    static uint32_t counter = 1;
    (void)self;
    return counter++;
}

static CordMellanoxFlowRule* find_flow_rule(CordConnectX4LxFlowPoint * const self, uint32_t flow_id)
{
    for (uint16_t i = 0; i < self->num_flow_rules; i++) {
        if (self->flow_rules[i].flow_id == flow_id) {
            return &self->flow_rules[i];
        }
    }
    return NULL;
}

void CordConnectX4LxFlowPoint_ctor(CordConnectX4LxFlowPoint * const self,
                                   uint8_t id,
                                   const char *pci_address,
                                   uint16_t queue_id)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] ctor()\n");
#endif
    static const CordFlowPointVtbl vtbl = {
        .rx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *rx_bytes))&CordConnectX4LxFlowPoint_rx_,
        .tx = (cord_retval_t (*)(CordFlowPoint const * const self, void *buffer, size_t len, ssize_t *tx_bytes))&CordConnectX4LxFlowPoint_tx_,
    };

    CordFlowPoint_ctor(&self->base, id);
    self->base.vptr = &vtbl;
    
    self->pci_address = malloc(strlen(pci_address) + 1);
    if (self->pci_address) {
        strcpy(self->pci_address, pci_address);
    }
    
    self->queue_id = queue_id;
    self->num_queues = 8;
    self->port_id = 0;
    self->sriov_enabled = false;
    self->num_vfs = 0;
    self->num_flow_rules = 0;
    self->max_flow_rules = 256;
    self->mlx_context = NULL;
    self->mlx_pd = NULL;
    self->mlx_cq = NULL;
    self->mlx_qp = NULL;
    self->rss_enabled = false;
    self->hw_params = NULL;
    
    memset(self->flow_rules, 0, sizeof(self->flow_rules));
    memset(self->rss_key, 0, sizeof(self->rss_key));
    memset(self->rss_indirection_table, 0, sizeof(self->rss_indirection_table));
    
    for (uint16_t i = 0; i < 128; i++) {
        self->rss_indirection_table[i] = i % self->num_queues;
    }
    
    self->base.io_handle = -1;
}

void CordConnectX4LxFlowPoint_dtor(CordConnectX4LxFlowPoint * const self)
{
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] dtor()\n");
#endif
    if (self->pci_address) {
        free(self->pci_address);
    }
    free(self);
}

cord_retval_t CordConnectX4LxFlowPoint_create_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t priority,
                                                        uint32_t table_id,
                                                        uint32_t *flow_id)
{
    if (!self || !flow_id) return CORD_ERR_INVALID;
    if (self->num_flow_rules >= self->max_flow_rules) return CORD_ERR_NO_MEMORY;
    
    uint32_t new_flow_id = generate_flow_id(self);
    CordMellanoxFlowRule *rule = &self->flow_rules[self->num_flow_rules];
    
    memset(rule, 0, sizeof(CordMellanoxFlowRule));
    rule->flow_id = new_flow_id;
    rule->priority = priority;
    rule->table_id = table_id;
    rule->active = false;
    
    self->num_flow_rules++;
    *flow_id = new_flow_id;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Created flow rule %u (priority %u, table %u)\n",
             new_flow_id, priority, table_id);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_destroy_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                         uint32_t flow_id)
{
    if (!self) return CORD_ERR_INVALID;
    
    for (uint16_t i = 0; i < self->num_flow_rules; i++) {
        if (self->flow_rules[i].flow_id == flow_id) {
            memmove(&self->flow_rules[i], &self->flow_rules[i + 1],
                    (self->num_flow_rules - i - 1) * sizeof(CordMellanoxFlowRule));
            self->num_flow_rules--;
            
#ifdef CORD_FLOW_FP_LOG
            CORD_LOG("[CordConnectX4LxFlowPoint] Destroyed flow rule %u\n", flow_id);
#endif
            return CORD_OK;
        }
    }
    
    return CORD_ERR_NOT_FOUND;
}

cord_retval_t CordConnectX4LxFlowPoint_add_match_field(CordConnectX4LxFlowPoint * const self,
                                                       uint32_t flow_id,
                                                       CordMellanoxMatchField field,
                                                       uint64_t value,
                                                       uint64_t mask)
{
    if (!self) return CORD_ERR_INVALID;
    
    CordMellanoxFlowRule *rule = find_flow_rule(self, flow_id);
    if (!rule) return CORD_ERR_NOT_FOUND;
    if (rule->num_matches >= 16) return CORD_ERR_NO_MEMORY;
    
    CordMellanoxMatch *match = &rule->matches[rule->num_matches];
    match->field = field;
    match->value = value;
    match->mask = mask;
    match->enabled = true;
    
    rule->num_matches++;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Added match field %d to flow %u (value=0x%lx, mask=0x%lx)\n",
             field, flow_id, value, mask);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_add_action(CordConnectX4LxFlowPoint * const self,
                                                  uint32_t flow_id,
                                                  CordMellanoxFlowAction action,
                                                  uint32_t param)
{
    if (!self) return CORD_ERR_INVALID;
    
    CordMellanoxFlowRule *rule = find_flow_rule(self, flow_id);
    if (!rule) return CORD_ERR_NOT_FOUND;
    if (rule->num_actions >= 8) return CORD_ERR_NO_MEMORY;
    
    CordMellanoxAction *act = &rule->actions[rule->num_actions];
    act->action = action;
    act->enabled = true;
    
    switch (action) {
        case CORD_MLX_ACTION_REDIRECT_TO_QUEUE:
            act->queue_id = param;
            break;
        case CORD_MLX_ACTION_REDIRECT_TO_TABLE:
            act->table_id = param;
            break;
        case CORD_MLX_ACTION_MARK:
            act->mark_value = param;
            break;
        default:
            break;
    }
    
    rule->num_actions++;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Added action %d to flow %u (param=%u)\n",
             action, flow_id, param);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_commit_flow_rule(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id)
{
    if (!self) return CORD_ERR_INVALID;
    
    CordMellanoxFlowRule *rule = find_flow_rule(self, flow_id);
    if (!rule) return CORD_ERR_NOT_FOUND;
    
    rule->active = true;
    rule->packet_count = 0;
    rule->byte_count = 0;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Committed flow rule %u to hardware\n", flow_id);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_configure_rss(CordConnectX4LxFlowPoint * const self,
                                                     const uint8_t *rss_key,
                                                     size_t key_len,
                                                     const uint16_t *indirection_table,
                                                     size_t table_size)
{
    if (!self) return CORD_ERR_INVALID;
    if (key_len > sizeof(self->rss_key)) return CORD_ERR_INVALID;
    if (table_size > sizeof(self->rss_indirection_table) / sizeof(uint16_t)) return CORD_ERR_INVALID;
    
    if (rss_key && key_len > 0) {
        memcpy(self->rss_key, rss_key, key_len);
        memset(self->rss_key + key_len, 0, sizeof(self->rss_key) - key_len);
    }
    
    if (indirection_table && table_size > 0) {
        memcpy(self->rss_indirection_table, indirection_table, table_size * sizeof(uint16_t));
    }
    
    self->rss_enabled = true;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] RSS configured (key_len=%zu, table_size=%zu)\n",
             key_len, table_size);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_enable_sriov(CordConnectX4LxFlowPoint * const self,
                                                    uint8_t num_vfs)
{
    if (!self) return CORD_ERR_INVALID;
    if (num_vfs > 64) return CORD_ERR_INVALID;
    
    self->sriov_enabled = (num_vfs > 0);
    self->num_vfs = num_vfs;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] SR-IOV %s (%u VFs)\n",
             self->sriov_enabled ? "enabled" : "disabled", num_vfs);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_set_queue_count(CordConnectX4LxFlowPoint * const self,
                                                       uint16_t num_queues)
{
    if (!self) return CORD_ERR_INVALID;
    if (num_queues == 0 || num_queues > 1024) return CORD_ERR_INVALID;
    
    self->num_queues = num_queues;
    
    for (uint16_t i = 0; i < 128; i++) {
        self->rss_indirection_table[i] = i % num_queues;
    }
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Queue count set to %u\n", num_queues);
#endif
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_query_flow_stats(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id,
                                                        uint64_t *packet_count,
                                                        uint64_t *byte_count)
{
    if (!self || !packet_count || !byte_count) return CORD_ERR_INVALID;
    
    CordMellanoxFlowRule *rule = find_flow_rule(self, flow_id);
    if (!rule) return CORD_ERR_NOT_FOUND;
    if (!rule->active) return CORD_ERR_INVALID;
    
    *packet_count = rule->packet_count;
    *byte_count = rule->byte_count;
    
    return CORD_OK;
}

cord_retval_t CordConnectX4LxFlowPoint_reset_flow_stats(CordConnectX4LxFlowPoint * const self,
                                                        uint32_t flow_id)
{
    if (!self) return CORD_ERR_INVALID;
    
    CordMellanoxFlowRule *rule = find_flow_rule(self, flow_id);
    if (!rule) return CORD_ERR_NOT_FOUND;
    
    rule->packet_count = 0;
    rule->byte_count = 0;
    
#ifdef CORD_FLOW_FP_LOG
    CORD_LOG("[CordConnectX4LxFlowPoint] Reset stats for flow rule %u\n", flow_id);
#endif
    
    return CORD_OK;
}

uint16_t CordConnectX4LxFlowPoint_get_queue_id(const CordConnectX4LxFlowPoint * const self)
{
    return self ? self->queue_id : 0;
}

uint16_t CordConnectX4LxFlowPoint_get_num_queues(const CordConnectX4LxFlowPoint * const self)
{
    return self ? self->num_queues : 0;
}

uint16_t CordConnectX4LxFlowPoint_get_num_flow_rules(const CordConnectX4LxFlowPoint * const self)
{
    return self ? self->num_flow_rules : 0;
}

const char* CordConnectX4LxFlowPoint_get_pci_address(const CordConnectX4LxFlowPoint * const self)
{
    return self ? self->pci_address : NULL;
}