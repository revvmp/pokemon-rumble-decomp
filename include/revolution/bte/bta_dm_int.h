#ifndef RVL_SDK_BTE_BTA_DM_INT_H
#define RVL_SDK_BTE_BTA_DM_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bta_api.h>
#include <revolution/bte/bta_sys.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/sdp_api.h>

#define BTA_COPY_DEVICE_CLASS(coddst, codsrc)				\
	do														\
	{														\
		((UINT8 *)(coddst))[0] = ((UINT8 *)(codsrc))[0];	\
		((UINT8 *)(coddst))[1] = ((UINT8 *)(codsrc))[1];	\
		((UINT8 *)(coddst))[2] = ((UINT8 *)(codsrc))[2];	\
	} while (FALSE)

#define BTA_ALL_APP_ID 255

#define BTA_DM_NUM_PEER_DEVICE 7
#define BTA_DM_NUM_CONN_SRVS 5
#define BTA_DM_NUM_COMPRESS_SRVS 5
#define BTA_DM_NUM_PM_TIMER 3

#define BTA_SERVICE_ID_TO_SERVICE_MASK(id) (1 << (id))

enum {
    BTA_DM_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_DM),
    BTA_DM_API_DISABLE_EVT,
    BTA_DM_API_SET_NAME_EVT,
    BTA_DM_API_SET_VISIBILITY_EVT,
    BTA_DM_API_SIG_STRENGTH_EVT,
    BTA_DM_ACL_CHANGE_EVT,
    BTA_DM_API_BOND_EVT,
    BTA_DM_API_PIN_REPLY_EVT,
    BTA_DM_API_AUTH_REPLY_EVT,
    BTA_DM_PM_BTM_STATUS_EVT,
    BTA_DM_PM_TIMER_EVT,
    BTA_DM_API_KEEP_ACL_LINKS,
    BTA_DM_API_RESET_HCI,
    BTA_DM_MAX_EVENT
};

enum {
    BTA_DM_API_SEARCH_EVT = BTA_SYS_EVT_START(BTA_ID_DM_SEARCH),
    BTA_DM_API_SEARCH_CANCEL_EVT,
    BTA_DM_API_DISCOVER_EVT,
    BTA_DM_INQUIRY_CMPL_EVT,
    BTA_DM_REMT_NAME_EVT,
    BTA_DM_REMT_NAME_RESP_EVT,
    BTA_DM_SDP_RESULT_EVT,
    BTA_DM_SEARCH_CMPL_EVT,
    BTA_DM_DISCOVERY_RESULT_EVT
};

typedef UINT8 tBTA_DM_CONN_STATE;
enum
{
	BTA_DM_CONNECTED	= 1,
	BTA_DM_UNPAIRING	= 2,
};

typedef UINT8 tBTA_DM_COMPRESS_STATE;
enum
{
	BTA_COMPRESS_STATE_0	= 0,
	BTA_COMPRESS_STATE_1	= 1,
};


typedef struct {
    BT_HDR hdr;
    tBTA_DM_SEC_CBACK* p_sec_cback;
} tBTA_DM_API_ENABLE;

typedef struct {
    BT_HDR hdr;
    char name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH];
} tBTA_DM_API_SET_NAME;

typedef struct {
    BT_HDR hdr;
    tBTA_DM_DISC disc_mode;
    tBTA_DM_CONN conn_mode;
} tBTA_DM_API_SET_VISIBILITY;

typedef struct {
    BT_HDR hdr;
    tBTA_DM_INQ inq_params;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK* p_cback;
} tBTA_DM_API_SEARCH;

typedef struct {
    BT_HDR hdr;
    BD_ADDR bd_addr;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK* p_cback;
} tBTA_DM_API_DISCOVER;

typedef struct {
    BT_HDR hdr;
    BD_ADDR bd_addr;
    UINT8 pin_len;
    UINT8 p_pin[PIN_CODE_LEN];
} tBTA_DM_API_BOND;

typedef struct {
    BT_HDR hdr;
    BD_ADDR bd_addr;
    BOOLEAN accept;
    UINT8 pin_len;
    UINT8 p_pin[PIN_CODE_LEN];
} tBTA_DM_API_PIN_REPLY;

typedef struct {
    BT_HDR hdr;
    BD_ADDR bd_addr;
    tBTA_SERVICE_ID service;
    tBTA_AUTH_RESP response;
} tBTA_DM_API_AUTH_REPLY;

typedef struct {
    BT_HDR hdr;
    tBTA_DM_SEARCH result;
} tBTA_DM_REM_NAME;

typedef struct {
    BT_HDR hdr;
    tBTA_DM_SEARCH result;
} tBTA_DM_DISC_RESULT;

typedef struct {
    BT_HDR hdr;
    UINT8 num;
} tBTA_DM_INQUIRY_CMPL;

typedef struct {
    BT_HDR hdr;
    UINT16 sdp_result;
} tBTA_DM_SDP_RESULT;

typedef struct
{
	BT_HDR					hdr;
	tBTA_SIG_STRENGTH_MASK	mask;
	UINT16					period;
	BOOLEAN					start;
} tBTA_API_DM_SIG_STRENGTH;

typedef struct
{
	BT_HDR			hdr;
	tBTM_BL_EVENT	event;
	UINT8			busy_level;
	BOOLEAN			is_new;
	BD_ADDR			bd_addr;
} tBTA_DM_ACL_CHANGE;

typedef union {
    BT_HDR hdr;
    tBTA_DM_API_ENABLE enable;
    tBTA_DM_API_SET_NAME set_name;
    tBTA_DM_API_SET_VISIBILITY set_visibility;
    tBTA_DM_API_SEARCH search;
    tBTA_DM_API_DISCOVER discover;
    tBTA_DM_API_BOND bond;
    tBTA_DM_API_PIN_REPLY pin_reply;
    tBTA_DM_API_AUTH_REPLY auth_reply;
    tBTA_DM_REM_NAME rem_name;
    tBTA_DM_DISC_RESULT disc_result;
    tBTA_DM_INQUIRY_CMPL inq_cmpl;
    tBTA_DM_SDP_RESULT sdp_event;
    tBTA_API_DM_SIG_STRENGTH sig_strength;
    tBTA_DM_ACL_CHANGE acl_change;
} tBTA_DM_MSG;

typedef struct {
    BD_ADDR peer_bdaddr;
    tBTA_DM_CONN_STATE conn_state;
    tBTA_PREF_ROLES pref_role;
    BOOLEAN in_use;
    tBTA_DM_PM_ACTION pm_mode_attempted;
    tBTA_DM_PM_ACTION pm_mode_failed;
} tBTA_DM_PEER_DEVICE;

typedef struct {
    tBTA_DM_PEER_DEVICE peer_device[BTA_DM_NUM_PEER_DEVICE];
    UINT8 count;
} tBTA_DM_ACTIVE_LINK;

typedef struct {
    BD_ADDR peer_bdaddr;
    tBTA_SYS_ID id;
    UINT8 app_id;
    tBTA_SYS_CONN_STATUS state;
} tBTA_DM_SRVCS;

typedef struct {
    UINT8 count;
    tBTA_DM_SRVCS conn_srvc[BTA_DM_NUM_CONN_SRVS];
} tBTA_DM_CONNECTED_SRVCS;

typedef struct {
    BD_ADDR peer_bdaddr;
    tBTA_SYS_ID id;
    UINT8 app_id;
    tBTA_DM_COMPRESS_STATE state;
} tBTA_DM_COMPRESS_SRVCS;

typedef struct {
    TIMER_LIST_ENT timer;
    BD_ADDR peer_bdaddr;
    BOOLEAN in_use;
} tBTA_PM_TIMER;

typedef struct {
    tBTA_DM_ACTIVE_LINK device_list;
    tBTA_DM_SEC_CBACK* p_sec_cback;
    TIMER_LIST_ENT signal_strength_timer;
    tBTA_SIG_STRENGTH_MASK signal_strength_mask;
    UINT16 state;
    UINT16 signal_strength_period;
    BOOLEAN disabling;
    TIMER_LIST_ENT disable_timer;
    UINT8 num_master_only;
    UINT32 wbt_sdp_handle;
    UINT8 wbt_scn;
    UINT8 pm_id;
    tBTA_PM_TIMER pm_timer[BTA_DM_NUM_PM_TIMER];
    BD_ADDR pin_bd_addr;
    DEV_CLASS pin_dev_class;
    BOOLEAN keep_acl_on_shut_down;
} tBTA_DM_CB;

typedef struct {
    tBTA_DM_SEARCH_CBACK* p_search_cback;
    tBTM_INQ_INFO* p_btm_inq_info;
    tBTA_SERVICE_MASK services;
    tBTA_SERVICE_MASK services_to_search;
    tBTA_SERVICE_MASK services_found;
    tSDP_DISCOVERY_DB* p_sdp_db;
    UINT16 state;
    BD_ADDR peer_bdaddr;
    char peer_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH];
    TIMER_LIST_ENT search_timer;
    TIMER_LIST_ENT rnr_timer;
    UINT8 service_index;
    tBTA_DM_MSG* p_search_queue;
    BOOLEAN wait_disc;
    BOOLEAN sdp_results;
} tBTA_DM_SEARCH_CB;

typedef struct {
    DEV_CLASS dev_class;
    UINT16 policy_settings;
    UINT16 page_timeout;
    UINT16 link_timeout;
} tBTA_DM_CFG;

typedef struct
{
	UINT8	id;
	UINT8	app_id;
	UINT8	cfg;
} tBTA_DM_RM;

typedef struct
{
	UINT8	id;
	UINT8	app_id;
	UINT8	mask;
} tBTA_DM_COMPRESS;

extern const UINT16 bta_service_id_to_uuid_lkup_tbl[];
extern const UINT32 bta_service_id_to_btm_srv_id_lkup_tbl[];
extern tBTA_DM_CONNECTED_SRVCS bta_dm_conn_srvcs;
extern tBTA_DM_CB bta_dm_cb;
extern tBTA_DM_SEARCH_CB bta_dm_search_cb;
extern const tBTA_DM_CFG bta_dm_cfg;

void bta_dm_enable(tBTA_DM_MSG* p_data);
void bta_dm_disable(tBTA_DM_MSG* p_data);
void bta_dm_set_dev_name(tBTA_DM_MSG* p_data);
void bta_dm_set_visibility(tBTA_DM_MSG* p_data);
void bta_dm_bond(tBTA_DM_MSG* p_data);
void bta_dm_pin_reply(tBTA_DM_MSG* p_data);
void bta_dm_auth_reply(tBTA_DM_MSG* p_data);
void bta_dm_search_start(tBTA_DM_MSG* p_data);
void bta_dm_search_cancel(tBTA_DM_MSG* p_data);
void bta_dm_discover(tBTA_DM_MSG* p_data);
void bta_dm_inq_cmpl(tBTA_DM_MSG* p_data);
void bta_dm_rmt_name(tBTA_DM_MSG* p_data);
void bta_dm_disc_rmt_name(tBTA_DM_MSG* p_data);
void bta_dm_sdp_result(tBTA_DM_MSG* p_data);
void bta_dm_search_cmpl(tBTA_DM_MSG* p_data);
void bta_dm_disc_result(tBTA_DM_MSG* p_data);
void bta_dm_search_result(tBTA_DM_MSG* p_data);
void bta_dm_free_sdp_db(tBTA_DM_MSG* p_data);
void bta_dm_queue_search(tBTA_DM_MSG* p_data);
void bta_dm_queue_disc(tBTA_DM_MSG* p_data);
void bta_dm_search_clear_queue(tBTA_DM_MSG* p_data);
void bta_dm_search_cancel_cmpl(tBTA_DM_MSG* p_data);
void bta_dm_search_cancel_transac_cmpl(tBTA_DM_MSG* p_data);
void bta_dm_search_cancel_notify(tBTA_DM_MSG* p_data);

void bta_dm_init_pm(void);
void bta_dm_disable_pm(void);

extern tBTA_DM_RM *p_bta_dm_rm_cfg;
extern tBTA_DM_COMPRESS *p_bta_dm_compress_cfg;
#endif
