#ifndef RVL_SDK_BTE_BTM_INT_H
#define RVL_SDK_BTE_BTM_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/sdp_api.h>

#define BTM_MAX_SCN PORT_MAX_RFC_PORTS
#define BTM_SEC_MAX_LINK_KEY_CALLBACKS 2
#define BTM_SEC_MAX_RMT_NAME_CALLBACKS 2
#define BTM_DISC_DB_SIZE 4000

typedef UINT8 tBTM_ACL_SWKEY_STATE;
typedef UINT8 tBTM_DEV_STATE;

enum {
    BTM_DEV_STATE_WAIT_RESET_CMPLT,
    BTM_DEV_STATE_WAIT_AFTER_RESET,
    BTM_DEV_STATE_READY
};

typedef UINT8 tBTM_RMT_NAME_ACTIVE_FLAGS;
typedef UINT8 tBTM_INQ_STATE;

typedef char tBTM_LOC_BD_NAME[BTM_MAX_LOC_BD_NAME_LEN + 1];
typedef tBTM_SEC_CBACK tBTM_SEC_CALLBACK;
typedef void tBTM_SCO_IND_CBACK(UINT16 sco_inx);
typedef tSDP_DISCOVERY_DB tBTM_DISCOVERY_DB;

typedef struct {
    tHCI_HANDLE hci_handle;
    UINT16 pkt_types_mask;
    UINT16 restore_pkt_types;
    UINT16 clock_offset;
    BD_ADDR remote_addr;
    DEV_CLASS remote_dc;
    BD_NAME remote_name;
    UINT16 manufacturer;
    UINT16 lmp_subversion;
    UINT16 link_super_tout;
    BD_FEATURES features;
    UINT8 lmp_version;
    BOOLEAN in_use;
    tBTM_ROLE link_role;
    tBTM_ACL_SWKEY_STATE switch_role_state;
} tACL_CONN;

typedef struct {
    tBTM_DEV_STATUS_CB* p_dev_status_cb;
    tBTM_VS_EVT_CB* p_vend_spec_cb;
    tBTM_CMPL_CB* p_stored_link_key_cmpl_cb;
    TIMER_LIST_ENT reset_timer;
    tBTM_CMPL_CB* p_reset_cmpl_cb;
    TIMER_LIST_ENT rln_timer;
    tBTM_CMPL_CB* p_rln_cmpl_cb;
    TIMER_LIST_ENT rlinkp_timer;
    tBTM_CMPL_CB* p_rlinkp_cmpl_cb;
    TIMER_LIST_ENT rssi_timer;
    tBTM_CMPL_CB* p_rssi_cmpl_cb;
    TIMER_LIST_ENT lnk_quality_timer;
    tBTM_CMPL_CB* p_lnk_qual_cmpl_cb;
    TIMER_LIST_ENT qossu_timer;
    tBTM_CMPL_CB* p_qossu_cmpl_cb;
    tBTM_CMPL_CB* p_vsc_cmpl_cb;
    tBTM_CMPL_CB* p_reset_only_cmpl_cb;
    tBTM_ROLE_SWITCH_CMPL switch_role_ref_data;
    tBTM_CMPL_CB* p_switch_role_cb;
    BD_ADDR local_addr;
    tBTM_VERSION_INFO local_version;
    BD_FEATURES local_features;
    DEV_CLASS dev_class;
    UINT16 page_timeout;
    tBTM_DEV_STATE state;
    UINT8 retry_count;
    UINT8 vsc_busy;
} tBTM_DEVCB;

typedef struct {
    UINT32 inq_count;
    BD_ADDR bd_addr;
} tINQ_BDADDR;

typedef struct {
    UINT32 time_of_resp;
    UINT32 inq_count;
    tBTM_INQ_INFO inq_info;
    BOOLEAN in_use;
} tINQ_DB_ENT;

typedef struct {
    tBTM_CMPL_CB* p_remname_cmpl_cb;
    TIMER_LIST_ENT rmt_name_timer_ent;
    tBTM_DISC_MODE discoverable_mode;
    tBTM_CONN_MODE connectable_mode;
    UINT16 page_scan_window;
    UINT16 page_scan_period;
    UINT16 inq_scan_window;
    UINT16 inq_scan_period;
    tBTM_INQ_SCAN_TYPE inq_scan_type;
    tBTM_INQ_SCAN_TYPE page_scan_type;
    BD_ADDR remname_bda;
    tBTM_RMT_NAME_ACTIVE_FLAGS remname_active;
    tBTM_CMPL_CB* p_inq_cmpl_cb;
    tBTM_INQ_RESULTS_CB* p_inq_results_cb;
    tBTM_CMPL_CB* p_inqfilter_cmpl_cb;
    tBTM_INQ_DB_CHANGE_CB* p_inq_change_cb;
    UINT32 inq_counter;
    TIMER_LIST_ENT inq_timer_ent;
    tINQ_BDADDR* p_bd_db;
    UINT16 num_bd_entries;
    UINT16 max_bd_entries;
    tINQ_DB_ENT inq_db[BTM_INQ_DB_SIZE];
    tBTM_INQ_PARMS inqparms;
    tBTM_INQUIRY_CMPL inq_cmpl_info;
    UINT16 per_min_delay;
    UINT16 per_max_delay;
    BOOLEAN inqfilt_active;
    UINT8 pending_filt_complete_event;
    tBTM_INQ_FILT_COND_TYPE inqfilt_type;
    tBTM_INQ_STATE state;
    tBTM_INQ_ACTIVE_STATUS inq_active;
} tBTM_INQUIRY_VAR_ST;

typedef struct {
    tBTM_ESCO_CBACK* p_esco_cback;
    tBTM_ESCO_PARAMS setup;
    tBTM_ESCO_DATA data;
    tHCI_STATUS hci_status;
} tBTM_ESCO_INFO;

typedef struct {
    tBTM_SCO_CB* p_conn_cb;
    tBTM_SCO_CB* p_disc_cb;
    UINT16 state;
    tHCI_HANDLE hci_handle;
    BOOLEAN is_orig;
    BOOLEAN rem_bd_known;
    tBTM_ESCO_INFO esco;
} tSCO_CONN;

typedef struct {
    tBTM_SCO_IND_CBACK* app_sco_ind_cb;
    tSCO_CONN sco_db[BTM_MAX_SCO_LINKS];
    BD_ADDR xfer_addr;
    UINT16 sco_disc_reason;
    tBTM_ESCO_PARAMS def_esco_parms;
    BOOLEAN esco_supported;
    tBTM_SCO_TYPE desired_sco_mode;
    tBTM_SCO_TYPE xfer_sco_type;
} tSCO_CB;

typedef struct {
    UINT32 mx_proto_id;
    UINT32 orig_mx_chan_id;
    UINT32 term_mx_chan_id;
    UINT16 psm;
    UINT8 security_flags;
    UINT8 service_id;
    UINT8 orig_service_name[BTM_SEC_SERVICE_NAME_LEN + 1];
    UINT8 term_service_name[BTM_SEC_SERVICE_NAME_LEN + 1];
} tBTM_SEC_SERV_REC;

typedef struct {
    tBTM_SEC_SERV_REC* p_cur_service;
    tBTM_SEC_CALLBACK* p_callback;
    void* p_ref_data;
    UINT32 timestamp;
    UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT16 hci_handle;
    UINT16 clock_offset;
    BD_ADDR bd_addr;
    DEV_CLASS dev_class;
    LINK_KEY link_key;
    tBTM_BD_NAME sec_bd_name;
    UINT8 sec_flags;
    BD_FEATURES features;
    UINT8 sec_state;
    BOOLEAN is_originator;
    UINT8 security_required;
    BOOLEAN link_key_not_sent;
    UINT8 link_key_type;
    BOOLEAN link_key_changed;
    BOOLEAN sec_acl_connected;
} tBTM_SEC_DEV_REC;

typedef struct {
    tBTM_LOC_BD_NAME bd_name;
    BOOLEAN pin_type;
    UINT8 pin_code_len;
    PIN_CODE pin_code;
    BOOLEAN connectable;
    UINT8 def_inq_scan_mode;
} tBTM_CFG;

typedef struct {
    tBTM_PM_PWR_MD req_mode[BTM_MAX_PM_RECORDS];
    tBTM_PM_PWR_MD set_mode;
    UINT16 interval;
    UINT8 state;
    UINT8 chg_ind;
} tBTM_PM_MCB;

typedef struct {
    tBTM_PM_STATUS_CBACK* cback;
    UINT8 mask;
} tBTM_PM_RCB;

typedef struct {
    TIMER_LIST_ENT disc_timer_ent;
    UINT16 num_uuid_filters;
    tBT_UUID uuid_filters[SDP_MAX_UUID_FILTERS];
    UINT16 num_attr_filters;
    UINT16 attr_filters[SDP_MAX_ATTR_FILTERS];
    tBTM_INQ_INFO* p_cur_ii;
    tBTM_CMPL_CB* p_disc_cmpl_cb;
    UINT8 btm_db_area[BTM_DISC_DB_SIZE];
    UINT8 disc_active;
} tBTM_DISCOVERY_VAR_ST;

typedef struct {
    tBTM_CFG cfg;
    tACL_CONN acl_db[MAX_L2CAP_LINKS];
    UINT8 btm_scn[BTM_MAX_SCN];
    UINT16 btm_def_link_policy;
    UINT16 btm_def_link_super_tout;
    tBTM_ACL_DB_CHANGE_CB* p_acl_changed_cb;
    tBTM_PM_MCB pm_mode_db[MAX_L2CAP_LINKS];
    tBTM_PM_RCB pm_reg_db[BTM_MAX_PM_RECORDS];
    UINT8 pm_pend_link;
    UINT8 pm_pend_id;
    tBTM_DEVCB devcb;
    UINT16 btm_acl_pkt_types_supported;
    UINT16 btm_sco_pkt_types_supported;
    tBTM_DISCOVERY_VAR_ST btm_disc_vars;
    tBTM_DISCOVERY_DB* p_btm_ddb;
    tBTM_INQUIRY_VAR_ST btm_inq_vars;
    tBTM_FILTER_CB* p_inq_filter_cb;
    tSCO_CB sco_cb;
    tBTM_APPL_INFO api;
    tBTM_LINK_KEY_CALLBACK* p_link_key_callback[BTM_SEC_MAX_LINK_KEY_CALLBACKS];
    tBTM_RMT_NAME_CALLBACK* p_rmt_name_callback[BTM_SEC_MAX_RMT_NAME_CALLBACKS];
    tBTM_FILTER_CB* p_conn_filter_cb;
    tBTM_SEC_DEV_REC* p_collided_dev_rec;
    TIMER_LIST_ENT sec_pin_code_req_tle;
    BD_ADDR sec_pin_code_req_bd_addr;
    TIMER_LIST_ENT sec_collision_tle;
    UINT32 dev_rec_count;
    UINT8 security_mode;
    BOOLEAN pairing_disabled;
    BOOLEAN connect_only_paired;
    BOOLEAN security_mode_changed;
    BOOLEAN pin_type_changed;
    BOOLEAN bond_created_connection;
    UINT8 disconnect_after_bond_reason;
    UINT8 pin_code_len;
    PIN_CODE pin_code;
    tBTM_SEC_SERV_REC sec_serv_rec[BTM_SEC_MAX_SERVICE_RECORDS];
    tBTM_SEC_DEV_REC sec_dev_rec[BTM_SEC_MAX_DEVICE_RECORDS];
    tBTM_MKEY_CALLBACK* mkey_cback;
    BD_ADDR connecting_bda;
    DEV_CLASS connecting_dc;
    tBTM_AFH_CHANNEL first_disabled_channel;
    tBTM_AFH_CHANNEL last_disabled_channel;
    UINT8 acl_disc_reason;
    tBT_TRACE_LEVEL trace_level;
} tBTM_CB;

extern tBTM_CB btm_cb;

tBTM_STATUS btm_remove_acl(BD_ADDR bd_addr);
UINT32* btm_read_trusted_mask(BD_ADDR bd_addr);
void btm_init(void);
void btm_discovery_db_init(void);
void btm_sec_mx_access_request(BD_ADDR bd_addr, UINT16 psm, BOOLEAN is_originator, UINT32 mx_proto_id, UINT32 mx_chan_id,
                               tBTM_SEC_CALLBACK* p_callback, void* p_ref_data);
void btm_sec_abort_access_req(BD_ADDR bd_addr);
UINT16 btm_get_max_packet_size(BD_ADDR addr);
BOOLEAN btm_sec_is_bonding(UINT16 handle);
void btm_remove_sco_links(BD_ADDR bda);
void btm_acl_removed(BD_ADDR bda);
tBTM_STATUS btm_sec_disconnect(UINT16 handle, UINT8 reason);
BOOLEAN btm_is_sco_active_by_bdaddr(BD_ADDR remote_bda);

void btm_acl_encrypt_change(UINT16 handle, UINT8 status, UINT8 encr_enable);
void btm_read_remote_version_complete(UINT8* p);
void btm_read_remote_features_complete(UINT8* p);
void btm_read_link_policy_complete(UINT8* p);
void btm_process_clk_off_comp_evt(UINT16 hci_handle, UINT16 clock_offset);
void btm_acl_role_changed(UINT8 hci_status, BD_ADDR bd_addr, UINT8 new_role);
void btm_qos_setup_complete(UINT8 status, UINT16 handle, FLOW_SPEC* p_flow);
void btm_read_rssi_complete(UINT8* p);
void btm_read_link_quality_complete(UINT8* p);
void btm_reset_complete(void);
void btm_read_hci_buf_size_complete(UINT8* p, UINT16 evt_len);
void btm_read_local_version_complete(UINT8* p, UINT16 evt_len);
void btm_read_local_features_complete(UINT8* p, UINT16 evt_len);
void btm_read_local_name_complete(UINT8* p, UINT16 evt_len);
void btm_read_local_addr_complete(UINT8* p, UINT16 evt_len);
void btm_vsc_complete(UINT8* p, UINT16 opcode, UINT16 evt_len);
void btm_vendor_specific_evt(UINT8* p, UINT8 evt_len);
void btm_read_stored_link_key_complete(UINT8* p);
void btm_write_stored_link_key_complete(UINT8* p);
void btm_delete_stored_link_key_complete(UINT8* p);
void btm_return_link_keys_evt(tBTM_RETURN_LINK_KEYS_EVT* result);
void btm_report_device_status(tBTM_DEV_STATUS status);
void btm_event_filter_complete(UINT8* p);
void btm_process_inq_results(UINT8* p, UINT8 inq_res_mode);
void btm_process_inq_complete(UINT8 status);
void btm_process_remote_name(BD_ADDR bda, BD_NAME bdn, UINT16 evt_len, UINT8 hci_status);
void btm_pm_proc_cmd_status(UINT8 status);
void btm_pm_proc_mode_change(UINT8 hci_status, UINT16 hci_handle, UINT8 mode, UINT16 interval);
void btm_sco_chk_pend_unpark(UINT8 hci_status, UINT16 hci_handle, UINT8 mode);
void btm_sco_conn_req(BD_ADDR bda, DEV_CLASS dev_class, UINT8 link_type);
void btm_sco_connected(UINT8 hci_status, BD_ADDR bda, UINT16 hci_handle, tBTM_ESCO_DATA* p_esco_data);
void btm_sco_removed(UINT16 hci_handle, UINT8 reason);
void btm_esco_proc_conn_chg(UINT8 status, UINT16 handle, UINT8 tx_interval, UINT8 retrans_window, UINT16 rx_pkt_len,
                            UINT16 tx_pkt_len);
BOOLEAN btm_is_sco_active(UINT16 handle);
void btm_sec_conn_req(BD_ADDR bda, DEV_CLASS dc);
void btm_sec_rmt_name_request_complete(BD_ADDR_PTR p_bd_addr, UINT8* p_bd_name, UINT8 status);
void btm_sec_auth_complete(UINT16 handle, UINT8 status);
void btm_sec_mkey_comp_event(UINT16 handle, UINT8 status, UINT8 key_flg);
void btm_sec_encrypt_change(UINT16 handle, UINT8 status, UINT8 encr_enable);
void btm_sec_connected(BD_ADDR bda, UINT16 handle, UINT8 status, UINT8 enc_mode);
void btm_sec_disconnected(UINT16 handle, UINT8 reason);
void btm_sec_link_key_notification(BD_ADDR_PTR p_bda, UINT8* p_link_key, UINT8 key_type);
void btm_sec_link_key_request(BD_ADDR_PTR p_bda);
void btm_sec_pin_code_request(BD_ADDR_PTR p_bda);
void btm_sec_update_clock_offset(UINT16 handle, UINT16 clock_offset);

#endif
