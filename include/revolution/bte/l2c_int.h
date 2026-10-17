#ifndef RVL_SDK_BTE_L2C_INT_H
#define RVL_SDK_BTE_L2C_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/l2c_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define L2CAP_MIN_MTU 48

#define L2CAP_LINK_CONNECT_TOUT 60
#define L2CAP_LINK_ROLE_SWITCH_TOUT 10
#define L2CAP_ECHO_RSP_TOUT 30

#define L2CAP_SEND_CMD_OFFSET 0

typedef enum {
    CST_CLOSED = 0,
    CST_ORIG_W4_SEC_COMP = 1,
    CST_TERM_W4_SEC_COMP = 2,
    CST_W4_L2CAP_CONNECT_RSP = 3,
    CST_W4_L2CA_CONNECT_RSP = 4,
    CST_CONFIG = 5,
    CST_OPEN = 6,
    CST_W4_L2CAP_DISCONNECT_RSP = 7,
    CST_W4_L2CA_DISCONNECT_RSP = 8
} tL2C_CHNL_STATE;

typedef enum {
    LST_DISCONNECTED = 0,
    LST_CONNECT_HOLDING = 1,
    LST_CONNECTING_WAIT_SWITCH = 2,
    LST_CONNECTING = 3,
    LST_CONNECTED = 4,
    LST_DISCONNECTING = 5
} tL2C_LINK_STATE;

enum {
    L2CEVT_LP_DISCONNECT_IND = 3,
    L2CEVT_L2CAP_CONNECT_REQ = 10,
    L2CEVT_L2CAP_CONNECT_RSP = 11,
    L2CEVT_L2CAP_CONNECT_RSP_PND = 12,
    L2CEVT_L2CAP_CONNECT_RSP_NEG = 13,
    L2CEVT_L2CAP_CONFIG_REQ = 14,
    L2CEVT_L2CAP_CONFIG_RSP = 15,
    L2CEVT_L2CAP_CONFIG_RSP_NEG = 16,
    L2CEVT_L2CAP_DISCONNECT_REQ = 17,
    L2CEVT_L2CAP_DISCONNECT_RSP = 18,
    L2CEVT_L2CAP_DATA = 19,
    L2CEVT_L2CA_CONNECT_REQ = 20,
    L2CEVT_L2CA_CONNECT_RSP = 21,
    L2CEVT_L2CA_CONNECT_RSP_NEG = 22,
    L2CEVT_L2CA_CONFIG_REQ = 23,
    L2CEVT_L2CA_CONFIG_RSP = 24,
    L2CEVT_L2CA_CONFIG_RSP_NEG = 25,
    L2CEVT_L2CA_DISCONNECT_REQ = 26,
    L2CEVT_L2CA_DISCONNECT_RSP = 27,
    L2CEVT_L2CA_DATA_WRITE = 29,
    L2CEVT_TIMEOUT = 30
};

typedef struct {
    BD_ADDR bd_addr;
    UINT8 status;
    UINT16 psm;
    UINT16 l2cap_result;
    UINT16 l2cap_status;
    UINT16 remote_cid;
} tL2C_CONN_INFO;

typedef struct {
    BOOLEAN in_use;
    UINT16 psm;
    tL2CAP_APPL_INFO api;
} tL2C_RCB;

typedef struct t_l2c_ccb {
    BOOLEAN in_use;
    tL2C_CHNL_STATE chnl_state;
    struct t_l2c_ccb* p_next_ccb;
    struct t_l2c_ccb* p_prev_ccb;
    struct t_l2c_linkcb* p_lcb;
    UINT16 local_cid;
    UINT16 remote_cid;
    TIMER_LIST_ENT timer_entry;
    tL2C_RCB* p_rcb;
    UINT8 config_done;
    UINT8 local_id;
    UINT8 remote_id;
    BOOLEAN rtry_flag;
    UINT16 in_mtu;
    UINT16 out_mtu;
    UINT16 our_flush_to;
    FLOW_SPEC in_flow;
    FLOW_SPEC out_flow;
    BUFFER_Q xmit_hold_q;
} tL2C_CCB;

typedef struct t_l2c_linkcb {
    BOOLEAN in_use;
    tL2C_LINK_STATE link_state;
    tL2C_CCB* p_first_ccb;
    tL2C_CCB* p_last_ccb;
    TIMER_LIST_ENT timer_entry;
    UINT16 handle;
    BD_ADDR remote_bd_addr;
    UINT8 link_role;
    UINT8 id;
    UINT16 link_flush_tout;
    UINT16 link_xmit_quota;
    UINT16 link_xmit_window;
    UINT16 sent_not_acked;
    UINT16 link_cong_start;
    UINT16 link_cong_end;
    UINT16 link_cong_discard;
    BOOLEAN partial_segment_being_sent;
    BOOLEAN cong_sent;
    BUFFER_Q link_xmit_data_q;
    BT_HDR* p_hcit_rcv_acl;
    tL2CA_ECHO_RSP_CB* p_echo_rsp_cb;
    UINT16 idle_timeout;
    UINT8 acl_priority;
} tL2C_LCB;

typedef struct {
    UINT8 l2cap_trace_level;
    UINT8 desire_role;
    UINT16 num_lm_acl_bufs;
    UINT16 controller_xmit_window;
    tL2C_LCB lcb_pool[MAX_L2CAP_LINKS];
    tL2C_CCB ccb_pool[MAX_L2CAP_CHANNELS];
    tL2C_RCB rcb_pool[MAX_L2CAP_CLIENTS];
    tL2C_CCB* p_free_ccb_first;
    tL2C_CCB* p_free_ccb_last;
    UINT16 num_links_active;
    UINT16 idle_timeout;
    tL2C_LCB* p_cur_hcit_lcb;
    BUFFER_Q rcv_hold_q;
    TIMER_LIST_ENT rcv_hold_tle;
    UINT8 compress_pool_id;
} tL2C_CB;

extern tL2C_CB l2cb;

void l2c_csm_execute(tL2C_CCB* p_ccb, UINT16 event, void* p_data);

BOOLEAN l2c_link_hci_conn_comp(UINT8 status, UINT16 handle, BD_ADDR p_bda);
BOOLEAN l2c_link_hci_disc_comp(UINT16 handle, UINT8 reason);
BOOLEAN l2c_link_hci_qos_violation(UINT16 handle);
void l2c_link_timeout(tL2C_LCB* p_lcb);
void l2c_link_check_send_pkts(tL2C_LCB* p_lcb, tL2C_CCB* p_ccb, BT_HDR* p_buf);
void l2c_link_adjust_allocation(void);
void l2c_link_process_num_completed_pkts(UINT8* p);
void l2c_link_role_changed(BD_ADDR bd_addr, UINT8 new_role);
void l2c_link_role_change_failed(void);
void l2c_pin_code_request(BD_ADDR bd_addr);

void l2c_init(void);
void l2c_rcv_acl_data(BT_HDR* p_msg);
void l2c_process_compress_setup_timeout(TIMER_LIST_ENT* p_tle);
void l2c_process_timeout(TIMER_LIST_ENT* p_tle);
void l2c_process_held_packets(BOOLEAN timed_out);

tL2C_LCB* l2cu_allocate_lcb(BD_ADDR p_bd_addr);
void l2cu_release_lcb(tL2C_LCB* p_lcb);
tL2C_LCB* l2cu_find_lcb_by_bd_addr(BD_ADDR p_bd_addr);
tL2C_LCB* l2cu_find_lcb_by_handle(UINT16 handle);
UINT8 l2cu_get_conn_role(tL2C_LCB* p_this_lcb);
BT_HDR* l2cu_build_header(UINT16 hci_handle, UINT16 len, UINT8 cmd, UINT8 id);
void l2cu_send_peer_cmd_reject(tL2C_LCB* p_lcb, UINT16 reason, UINT8 rem_id, UINT16 p1, UINT16 p2);
void l2cu_send_peer_connect_req(tL2C_CCB* p_ccb);
void l2cu_send_peer_connect_rsp(tL2C_CCB* p_ccb, UINT16 result, UINT16 status);
void l2cu_reject_connection(tL2C_LCB* p_lcb, UINT16 remote_cid, UINT8 rem_id, UINT16 result);
void l2cu_send_peer_config_req(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_send_peer_config_rsp(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_send_peer_config_rej(tL2C_CCB* p_ccb, UINT8* p_data, UINT16 data_len, UINT16 rej_len);
void l2cu_send_peer_disc_req(tL2C_CCB* p_ccb);
void l2cu_send_peer_disc_rsp(tL2C_LCB* p_lcb, UINT8 remote_id, UINT16 local_cid, UINT16 remote_cid);
void l2cu_send_peer_echo_req(tL2C_LCB* p_lcb, UINT8* p_data, UINT16 data_len);
void l2cu_send_peer_echo_rsp(tL2C_LCB* p_lcb, UINT8 id, UINT8* p_data, UINT16 data_len);
void l2cu_send_peer_info_req(tL2C_LCB* p_lcb, UINT16 info_type);
void l2cu_send_peer_info_rsp(tL2C_LCB* p_lcb, UINT8 remote_id, UINT16 info_type);
tL2C_CCB* l2cu_allocate_ccb(tL2C_LCB* p_lcb);
void l2cu_release_ccb(tL2C_CCB* p_ccb);
tL2C_CCB* l2cu_find_ccb_by_cid(tL2C_LCB* p_lcb, UINT16 local_cid);
tL2C_CCB* l2cu_find_ccb_by_remote_cid(tL2C_LCB* p_lcb, UINT16 remote_cid);
tL2C_RCB* l2cu_allocate_rcb(UINT16 psm);
void l2cu_release_rcb(tL2C_RCB* p_rcb);
tL2C_RCB* l2cu_find_rcb_by_psm(UINT16 psm);
BOOLEAN l2cu_process_peer_cfg_req(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_process_peer_cfg_rsp(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_process_our_cfg_req(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_process_our_cfg_rsp(tL2C_CCB* p_ccb, tL2CAP_CFG_INFO* p_cfg);
void l2cu_device_reset(void);
BOOLEAN l2cu_create_conn(tL2C_LCB* p_lcb);
BOOLEAN l2cu_create_conn_after_switch(tL2C_LCB* p_lcb);
tL2C_LCB* l2cu_find_lcb_by_state(tL2C_LINK_STATE state);
BOOLEAN l2cu_lcb_disconnecting(void);

#ifdef __cplusplus
}
#endif

#endif
