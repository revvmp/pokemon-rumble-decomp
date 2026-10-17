#ifndef BTE_BTA_HH_API_H
#define BTE_BTA_HH_API_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/hidh_int.h>
#include <revolution/bte/bt_types.h>


#include <revolution/bte/bta_api.h>
#include <revolution/bte/hiddefs.h>

#define BTA_HH_MAX_KNOWN		HID_HOST_MAX_DEVICES

#define BTA_HH_INVALID_HANDLE	0xff

#ifdef __cplusplus
	extern "C" {
#endif

typedef UINT8 tBTA_HH_STATUS;
enum
{
	BTA_HH_OK				= 0,
	BTA_HH_HS_TRANS_NOT_SPT	= 3,
	BTA_HH_HS_ERROR			= 5,
	BTA_HH_ERR				= 6,
	BTA_HH_ERR_SDP			= 7,
	BTA_HH_ERR_PROTO		= 8,
	BTA_HH_ERR_DB_FULL		= 9,
	BTA_HH_ERR_NO_RES		= 11,
	BTA_HH_ERR_AUTH_FAILED	= 12,
	BTA_HH_ERR_HDL			= 13,
	BTA_HH_ERR_16			= 16,
	BTA_HH_ERR_TOD_UNSPT	= 255,
};

typedef UINT16 tBTA_HH_EVT;
enum
{
	BTA_HH_ENABLE_EVT		= 0,
	BTA_HH_DISABLE_EVT		= 1,
	BTA_HH_OPEN_EVT			= 2,
	BTA_HH_CLOSE_EVT		= 3,
	BTA_HH_GET_RPT_EVT		= 4,
	BTA_HH_SET_RPT_EVT		= 5,
	BTA_HH_GET_PROTO_EVT	= 6,
	BTA_HH_SET_PROTO_EVT	= 7,
	BTA_HH_GET_IDLE_EVT		= 8,
	BTA_HH_SET_IDLE_EVT		= 9,
	BTA_HH_GET_DSCP_EVT		= 10,
	BTA_HH_ADD_DEV_EVT		= 11,
	BTA_HH_RMV_DEV_EVT		= 12,
	BTA_HH_VC_UNPLUG_EVT	= 13,
	BTA_HH_GET_ACL_Q_EVT	= 15,
};

typedef UINT8 tBTA_HH_PROTO_MODE;
enum
{
	BTA_HH_PROTO_RPT_MODE	= 0,
	BTA_HH_PROTO_BOOT_MODE	= 1,

	BTA_HH_PROTO_UNKNOWN	= 255
};

typedef UINT8 tBTA_HH_BOOT_RPT_ID;
enum
{
	BTA_HH_KEYBD_RPT_ID = 1,
	BTA_HH_MOUSE_RPT_ID,
};

typedef UINT8 tBTA_HH_DEVT;
enum
{
	BTA_HH_DEVT_UNKNOWN	= 0,
};

typedef UINT16 tBTA_HH_ATTR_MASK;

typedef UINT8 tBTA_HH_RPT_TYPE;
enum
{
	BTA_HH_RPTT_OUTPUT	= 2,
};

typedef UINT8 tBTA_HH_TRANS_CTRL_TYPE;
enum
{
	BTA_HH_CTRL_SUSPEND					= 3,
	BTA_HH_CTRL_EXIT_SUSPEND			= 4,
	BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG	= 5,
};

typedef struct
{
	tBTA_HH_DEVT	tod;
	UINT8			app_id;
} tBTA_HH_SPT_TOD;

typedef struct
{
	UINT8			max_devt_spt;
	tBTA_HH_SPT_TOD	*p_devt_list;
	UINT16			sdp_db_size;
} tBTA_HH_CFG;

typedef struct
{
	BD_ADDR			bda;
	tBTA_HH_STATUS	status;
	UINT8			handle;
} tBTA_HH_CONN;

typedef tBTA_HH_CONN tBTA_HH_DEV_INFO;

typedef struct
{
	tBTA_HH_STATUS	status;
	UINT8			handle;
} tBTA_HH_CBDATA;

typedef struct
{
	UINT8	this_char;
	BOOLEAN	caps_lock;
	BOOLEAN	upper;
	BOOLEAN	ctrl;
	BOOLEAN	alt;
	BOOLEAN	num_lock;
} tBTA_HH_KEYBD_RPT;

typedef struct
{
	UINT8	mouse_button;
	INT8	delta_x;
	INT8	delta_y;
} tBTA_HH_MICE_RPT;

typedef struct
{
	tBTA_HH_BOOT_RPT_ID	dev_type;
	union
	{
		tBTA_HH_KEYBD_RPT	keybd_rpt;
		tBTA_HH_MICE_RPT	mice_rpt;
	} data_rpt;
} tBTA_HH_BOOT_RPT;

typedef struct
{
	tBTA_HH_STATUS	status;
	UINT8			handle;
	union
	{
		tBTA_HH_PROTO_MODE	proto_mode;
		BT_HDR				rpt_data;
		UINT8				idle_rate;
	} rsp_data;
} tBTA_HH_HSDATA;

typedef struct
{
	UINT8	handle;
	UINT16	num_queued;
	UINT16	num_not_acked;
} tBTA_HH_QUEUE_INFO;

typedef struct
{
	UINT16				num_avail_buffs;
	UINT16				num_buffs;
	UINT16				num_links;
	tBTA_HH_QUEUE_INFO	queue_info[4];
} tBTA_HH_ACL_QUEUE_INFO;

typedef union
{
	tBTA_HH_DEV_INFO		dev_info;
	tBTA_HH_CONN			conn;
	tBTA_HH_CBDATA			dev_status;
	tBTA_HH_STATUS			status;
	tHID_DEV_DSCP_INFO		dscp_info;
	tBTA_HH_HSDATA			hs_data;
	tBTA_HH_ACL_QUEUE_INFO	acl_queue_info;
} tBTA_HH;

typedef void tBTA_HH_CBACK(tBTA_HH_EVT event, tBTA_HH *p_data);

void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK *p_cback);
void BTA_HhDisable(void);
void BTA_HhClose(UINT8 dev_handle);
void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask);

void BTA_HhSetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, BT_HDR *p_data);
void BTA_HhGetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, UINT8 rpt_id,
                     UINT16 buf_size);
void BTA_HhSetProtoMode(UINT8 dev_handle, tBTA_HH_PROTO_MODE p_type);
void BTA_HhGetProtoMode(UINT8 dev_handle);
void BTA_HhSetIdle(UINT8 dev_handle, UINT16 idle_rate);
void BTA_HhGetIdle(UINT8 dev_handle);
void BTA_HhSendCtrl(UINT8 dev_handle, tBTA_HH_TRANS_CTRL_TYPE c_type);
void BTA_HhSendData(UINT8 dev_handle, BT_HDR *p_data);
void BTA_HhGetDscpInfo(UINT8 dev_handle);
void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class,
                  UINT8 app_id, tHID_DEV_DSCP_INFO dscp_info);
void BTA_HhRemoveDev(UINT8 dev_handle);
void BTA_HhGetAclQueueInfo(void);
void BTA_HhParseBootRpt(tBTA_HH_BOOT_RPT *p_data, UINT8 *p_report,
                        UINT16 report_len);

#ifdef __cplusplus
	}
#endif

#endif
