#ifndef RVL_SDK_BTE_HIDH_INT_H
#define RVL_SDK_BTE_HIDH_INT_H

#include "RVL_SDK/bte/hid_conn.h"
#include "RVL_SDK/bte/hidh_api.h"
#include "RVL_SDK/bte/l2c_api.h"

#define HID_HOST_MAX_DEVICES 16
#define HID_HOST_MAX_CONN_RETRY 0

enum {
    HID_DEV_NO_CONN,
    HID_DEV_CONNECTED
};

typedef struct per_device_ctb {
    BOOLEAN in_use;
    BD_ADDR addr;
    UINT16 attr_mask;
    UINT8 state;
    UINT8 conn_substate;
    UINT8 conn_tries;
    tHID_CONN conn;
} tHID_HOST_DEV_CTB;

typedef struct host_ctb {
    tHID_HOST_DEV_CTB devices[HID_HOST_MAX_DEVICES];
    tHID_HOST_DEV_CALLBACK* callback;
    tL2CAP_CFG_INFO l2cap_cfg;
    BOOLEAN sdp_busy;
    tHID_HOST_SDP_CALLBACK* sdp_cback;
    tSDP_DISCOVERY_DB* p_sdp_db;
    tHID_DEV_SDP_INFO sdp_rec;
    BOOLEAN reg_flag;
    UINT8 trace_level;
} tHID_HOST_CTB;

extern tHID_HOST_CTB hh_cb;

tHID_STATUS hidh_conn_reg(void);
void hidh_conn_dereg(void);
tHID_STATUS hidh_conn_initiate(UINT8 dhandle);
tHID_STATUS hidh_conn_snd_data(UINT8 dhandle, UINT8 trans_type, UINT8 param, UINT16 data, UINT8 rpt_id, BT_HDR* buf);
tHID_STATUS hidh_conn_disconnect(UINT8 dhandle);

#endif
