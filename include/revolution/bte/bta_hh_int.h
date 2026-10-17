#ifndef BTE_BTA_HH_INT_H
#define BTE_BTA_HH_INT_H

#include <revolution/bte/bt_types.h>


#include <revolution/bte/bta_api.h>
#include <revolution/bte/bta_hh_api.h>
#include <revolution/bte/bta_sys.h>
#include <revolution/bte/hiddefs.h>
#include <revolution/bte/sdp_api.h>

#define BTA_HH_MAX_RPT_CHARS		8

#define BTA_HH_FST_TRANS_CB_EVT		BTA_HH_GET_RPT_EVT
#define BTA_HH_FST_BTE_TRANS_EVT	HID_TRANS_GET_REPORT

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT16 tBTA_HH_INT_EVT;
enum
{
	BTA_HH_API_OPEN_EVT = BTA_SYS_EVT_START(BTA_ID_HH),
	BTA_HH_API_CLOSE_EVT,
	BTA_HH_INT_OPEN_EVT,
	BTA_HH_INT_CLOSE_EVT,
	BTA_HH_INT_DATA_EVT,
	BTA_HH_INT_CTRL_DATA,
	BTA_HH_INT_HANDSK_EVT,
	BTA_HH_SDP_CMPL_EVT,
	BTA_HH_API_WRITE_DEV_EVT,
	BTA_HH_API_GET_DSCP_EVT,
	BTA_HH_API_MAINT_DEV_EVT,
	BTA_HH_OPEN_CMPL_EVT,
	BTA_HH_API_ENABLE_EVT,
	BTA_HH_API_DISABLE_EVT,
	BTA_HH_API_GET_ACL_Q_EVT,
	BTA_HH_DISC_CMPL_EVT,
	BTA_HH_INVALID_EVT,
};

typedef UINT8 tBTA_HH_STATE;
enum
{
	BTA_HH_NULL_ST,
	BTA_HH_IDLE_ST,
	BTA_HH_W4_CONN_ST,
	BTA_HH_CONN_ST,
};

typedef struct
{
	BT_HDR			hdr;
	UINT8			sec_mask;
	UINT8			service_name[BTA_SERVICE_NAME_LEN + 1];
	tBTA_HH_CBACK	*p_cback;
} tBTA_HH_API_ENABLE;

typedef struct
{
	BT_HDR				hdr;
	BD_ADDR				bd_addr;
	UINT8				sec_mask;
	tBTA_HH_PROTO_MODE	mode;
} tBTA_HH_API_CONN;

typedef struct
{
	BT_HDR	hdr;
	UINT8	t_type;
	UINT8	param;
	UINT8	rpt_id;
	UINT16	data;
	BT_HDR	*p_data;
} tBTA_HH_CMD_DATA;

typedef struct
{
	BT_HDR	hdr;
	UINT32	data;
	BT_HDR	*p_data;
} tBTA_HH_CBACK_DATA;

typedef struct
{
	BT_HDR				hdr;
	BD_ADDR				bda;
	UINT16				attr_mask;
	UINT16				sub_event;
	UINT8				sub_class;
	UINT8				app_id;
	tHID_DEV_DSCP_INFO	dscp_info;
} tBTA_HH_MAINT_DEV;

typedef union
{
	BT_HDR				hdr;
	tBTA_HH_API_ENABLE	api_enable;
	tBTA_HH_API_CONN	api_conn;
	tBTA_HH_CMD_DATA	api_sndcmd;
	tBTA_HH_CBACK_DATA	hid_cback;
	tBTA_HH_STATUS		status;
	tBTA_HH_MAINT_DEV	api_maintdev;
} tBTA_HH_DATA;

typedef struct
{
	tHID_DEV_DSCP_INFO	dscp_info;
	BD_ADDR				addr;
	UINT16				attr_mask;
	UINT16				w4_evt;
	UINT8				index;
	UINT8				sub_class;
	UINT8				sec_mask;
	UINT8				app_id;
	UINT8				hid_handle;
	BOOLEAN				vp;
	BOOLEAN				in_use;
	BOOLEAN				incoming_conn;
	BOOLEAN				opened;
	tBTA_HH_PROTO_MODE	mode;
	tBTA_HH_STATE		state;
} tBTA_HH_DEV_CB;

typedef struct
{
	BOOLEAN	upper;
	BOOLEAN	ctrl;
	BOOLEAN	alt;
	BOOLEAN	num_lock;
	BOOLEAN	caps_lock;
	UINT8	last_report[BTA_HH_MAX_RPT_CHARS];
} tBTA_HH_KB_CB;

typedef struct
{
	tBTA_HH_KB_CB			kb_cb;
	tBTA_HH_DEV_CB			kdev[BTA_HH_MAX_KNOWN];
	tBTA_HH_DEV_CB			*p_cur;
	UINT8					cb_index[BTA_HH_MAX_KNOWN];
	tBTA_HH_CBACK			*p_cback;
	tSDP_DISCOVERY_DB		*p_disc_db;
	UINT8					trace_level;
	UINT8					cnt_num;
	BOOLEAN					w4_disable;
} tBTA_HH_CB;

extern tBTA_HH_CB bta_hh_cb;
extern tBTA_HH_CFG *p_bta_hh_cfg;

void bta_hh_disc_cmpl(void);
void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event,
                       tBTA_HH_DATA *p_data);
BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class);
void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                               UINT16 attr_mask,
                               tHID_DEV_DSCP_INFO *p_dscp_info, UINT8 sub_class,
                               UINT8 app_id);
void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb);
void bta_hh_trace_dev_db(void);
BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg);
void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                            UINT16 report_len);
void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                           UINT16 report_len);
UINT8 bta_hh_find_cb(BD_ADDR bda);

void bta_hh_api_enable(tBTA_HH_DATA *p_data);
void bta_hh_api_disable(void);
void bta_hh_disc_cmpl(void);

void bta_hh_start_sdp(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_sdp_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_api_disc_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_open_cmpl_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_open_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_data_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_handsk_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_ctrl_dat_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_close_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_get_dscp_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_maint_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
void bta_hh_get_acl_q_info(void);
void bta_hh_write_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);

void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event,
                       tBTA_HH_DATA *p_data);
BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg);

UINT8 bta_hh_find_cb(BD_ADDR bda);
void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb);
void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                               UINT16 attr_mask,
                               tHID_DEV_DSCP_INFO *p_dscp_info, UINT8 sub_class,
                               UINT8 app_id);
BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class);
void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                            UINT16 report_len);
void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_mice_data, UINT8 *p_report,
                           UINT16 report_len);
void bta_hh_trace_dev_db(void);

#ifdef __cplusplus
	}
#endif

#endif
