#ifndef RVL_SDK_BTE_HCIMSGS_H
#define RVL_SDK_BTE_HCIMSGS_H

#include <revolution/bte/bt_types.h>
#include <revolution/bte/btu.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/hcidefs.h>

#define HCI_CMD_POOL_ID GKI_POOL_ID_2
#define HCI_GET_CMD_BUF(paramlen) ((BT_HDR*)GKI_getpoolbuf(HCI_CMD_POOL_ID))

#define HCIC_PARAM_SIZE_INQUIRY 5
#define HCIC_PARAM_SIZE_INQ_CANCEL 0
#define HCIC_PARAM_SIZE_PER_INQ_MODE 9
#define HCIC_PARAM_SIZE_CREATE_CONN 13
#define HCIC_PARAM_SIZE_DISCONNECT 3
#define HCIC_PARAM_SIZE_ADD_SCO_CONN 4
#define HCIC_PARAM_SIZE_ACCEPT_CONN 7
#define HCIC_PARAM_SIZE_REJECT_CONN 7
#define HCIC_PARAM_SIZE_LINK_KEY_REQ_REPLY 22
#define HCIC_PARAM_SIZE_LINK_KEY_NEG_REPLY 6
#define HCIC_PARAM_SIZE_PIN_CODE_REQ_REPLY 23
#define HCIC_PARAM_SIZE_PIN_CODE_NEG_REPLY 6
#define HCIC_PARAM_SIZE_CHANGE_CONN_TYPE 4
#define HCIC_PARAM_SIZE_CMD_HANDLE 2
#define HCIC_PARAM_SIZE_SET_CONN_ENCRYPT 3
#define HCIC_PARAM_SIZE_RMT_NAME_REQ 10
#define HCIC_PARAM_SIZE_RMT_NAME_REQ_CANCEL 6
#define HCIC_PARAM_SIZE_SETUP_ESCO 17
#define HCIC_PARAM_SIZE_ACCEPT_ESCO 21
#define HCIC_PARAM_SIZE_REJECT_ESCO 7
#define HCIC_PARAM_SIZE_HOLD_MODE 6
#define HCIC_PARAM_SIZE_SNIFF_MODE 10
#define HCIC_PARAM_SIZE_PARK_MODE 6
#define HCIC_PARAM_SIZE_SWITCH_ROLE 7
#define HCIC_PARAM_SIZE_WRITE_POLICY_SET 4
#define HCIC_PARAM_SIZE_RESET 0
#define HCIC_PARAM_SIZE_READ_STORED_KEY 7
#define HCIC_PARAM_SIZE_DELETE_STORED_KEY 7
#define HCIC_PARAM_SIZE_CHANGE_NAME BD_NAME_LEN
#define HCIC_PARAM_SIZE_READ_CMD 0
#define HCIC_PARAM_SIZE_WRITE_PARAM1 1
#define HCIC_PARAM_SIZE_WRITE_PARAM2 2
#define HCIC_PARAM_SIZE_WRITE_PARAM3 3
#define HCIC_PARAM_SIZE_SET_AFH_CHANNELS 10
#define HCIC_PARAM_SIZE_WRITE_PAGESCAN_CFG 4
#define HCIC_PARAM_SIZE_WRITE_INQSCAN_CFG 4
#define HCIC_PARAM_SIZE_WRITE_AUTO_FLUSH_TOUT 4
#define HCIC_PARAM_SIZE_SET_HOST_BUF_SIZE 7
#define HCIC_PARAM_SIZE_WRITE_LINK_SUPER_TOUT 4
#define HCI_MAX_NUM_OF_LINK_KEYS_PER_CMD 11

BOOLEAN btsnd_hcic_inquiry(LAP const inq_lap, UINT8 duration, UINT8 response_cnt);
BOOLEAN btsnd_hcic_inq_cancel(void);
BOOLEAN btsnd_hcic_per_inq_mode(UINT16 max_period, UINT16 min_period, LAP const inq_lap, UINT8 duration, UINT8 response_cnt);
BOOLEAN btsnd_hcic_create_conn(BD_ADDR dest, UINT16 packet_types, UINT8 page_scan_rep_mode, UINT8 page_scan_mode, UINT16 clock_offset, UINT8 allow_switch);
BOOLEAN btsnd_hcic_disconnect(UINT16 handle, UINT8 reason);
BOOLEAN btsnd_hcic_add_SCO_conn(UINT16 handle, UINT16 packet_types);
void btsnd_hcic_accept_conn(void* buffer, BD_ADDR dest, UINT8 role);
void btsnd_hcic_reject_conn(void* buffer, BD_ADDR dest, UINT8 reason);
BOOLEAN btsnd_hcic_link_key_req_reply(BD_ADDR bd_addr, LINK_KEY link_key);
BOOLEAN btsnd_hcic_link_key_neg_reply(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_pin_code_req_reply(BD_ADDR bd_addr, UINT8 pin_code_len, PIN_CODE pin_code);
BOOLEAN btsnd_hcic_pin_code_neg_reply(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_change_conn_type(UINT16 handle, UINT16 packet_types);
BOOLEAN btsnd_hcic_auth_request(UINT16 handle);
BOOLEAN btsnd_hcic_set_conn_encrypt(UINT16 handle, BOOLEAN enable);
BOOLEAN btsnd_hcic_rmt_name_req(BD_ADDR bd_addr, UINT8 page_scan_rep_mode, UINT8 page_scan_mode, UINT16 clock_offset);
BOOLEAN btsnd_hcic_rmt_name_req_cancel(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_rmt_features_req(UINT16 handle);
BOOLEAN btsnd_hcic_rmt_ver_req(UINT16 handle);
BOOLEAN btsnd_hcic_read_rmt_clk_offset(UINT16 handle);
BOOLEAN btsnd_hcic_setup_esco_conn(UINT16 handle, UINT32 tx_bw, UINT32 rx_bw, UINT16 max_latency, UINT16 voice, UINT8 retrans_effort, UINT16 packet_types);
void btsnd_hcic_accept_esco_conn(void* buffer, BD_ADDR bd_addr, UINT32 tx_bw, UINT32 rx_bw, UINT16 max_latency, UINT16 content_fmt, UINT8 retrans_effort, UINT16 packet_types);
void btsnd_hcic_reject_esco_conn(void* buffer, BD_ADDR bd_addr, UINT8 reason);
BOOLEAN btsnd_hcic_hold_mode(void* buffer, UINT16 handle, UINT16 max_hold_period, UINT16 min_hold_period);
BOOLEAN btsnd_hcic_sniff_mode(void* buffer, UINT16 handle, UINT16 max_sniff_period, UINT16 min_sniff_period, UINT16 sniff_attempt, UINT16 sniff_timeout);
BOOLEAN btsnd_hcic_exit_sniff_mode(void* buffer, UINT16 handle);
BOOLEAN btsnd_hcic_park_mode(void* buffer, UINT16 handle, UINT16 beacon_max_interval, UINT16 beacon_min_interval);
BOOLEAN btsnd_hcic_exit_park_mode(void* buffer, UINT16 handle);
BOOLEAN btsnd_hcic_switch_role(BD_ADDR bd_addr, UINT8 role);
BOOLEAN btsnd_hcic_write_policy_set(UINT16 handle, UINT16 settings);
BOOLEAN btsnd_hcic_reset(void);
void btsnd_hcic_set_event_filter(void* buffer, UINT8 filt_type, UINT8 filt_cond_type, UINT8* filt_cond, UINT8 filt_cond_len);
BOOLEAN btsnd_hcic_write_pin_type(UINT8 type);
void btsnd_hcic_read_stored_key(void* buffer, BD_ADDR bd_addr, BOOLEAN read_all_flag);
void btsnd_hcic_write_stored_key(void* buffer, UINT8 num_keys, BD_ADDR* bd_addr, LINK_KEY* link_key);
BOOLEAN btsnd_hcic_delete_stored_key(BD_ADDR bd_addr, BOOLEAN delete_all_flag);
void btsnd_hcic_change_name(void* buffer, BD_NAME name);
void btsnd_hcic_write_page_tout(void* buffer, UINT16 timeout);
void btsnd_hcic_write_scan_enable(void* buffer, UINT8 flag);
void btsnd_hcic_write_pagescan_cfg(void* buffer, UINT16 interval, UINT16 window);
void btsnd_hcic_write_inqscan_cfg(void* buffer, UINT16 interval, UINT16 window);
BOOLEAN btsnd_hcic_write_auth_enable(UINT8 flag);
BOOLEAN btsnd_hcic_write_encr_mode(UINT8 mode);
void btsnd_hcic_write_dev_class(void* buffer, DEV_CLASS dev_class);
void btsnd_hcic_write_auto_flush_tout(void* buffer, UINT16 handle, UINT16 tout);
BOOLEAN btsnd_hcic_set_host_buf_size(UINT16 acl_len, UINT8 sco_len, UINT16 acl_num, UINT16 sco_num);
BOOLEAN btsnd_hcic_write_link_super_tout(UINT16 handle, UINT16 timeout);
void btsnd_hcic_write_cur_iac_lap(void* buffer, UINT8 num_cur_iac, LAP* const iac_lap);
BOOLEAN btsnd_hcic_read_local_ver(void);
BOOLEAN btsnd_hcic_read_local_features(void);
void btsnd_hcic_read_buffer_size(void* buffer);
BOOLEAN btsnd_hcic_read_bd_addr(void);
BOOLEAN btsnd_hcic_get_link_quality(UINT16 handle);
BOOLEAN btsnd_hcic_read_rssi(UINT16 handle);
BOOLEAN btsnd_hcic_set_afh_channels(UINT8 first, UINT8 last);
void btsnd_hcic_write_inqscan_type(void* buffer, UINT8 type);
void btsnd_hcic_write_inquiry_mode(void* buffer, UINT8 mode);
void btsnd_hcic_write_pagescan_type(void* buffer, UINT8 type);
void btsnd_hcic_vendor_spec_cmd(void* buffer, UINT16 opcode, UINT8 len, UINT8* p_data);

#endif
