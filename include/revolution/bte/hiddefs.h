#ifndef RVL_SDK_BTE_HIDDEFS_H
#define RVL_SDK_BTE_HIDDEFS_H

#include <revolution/bte/sdp_api.h>

#define HID_MAX_SVC_NAME_LEN 32
#define HID_MAX_SVC_DESCR_LEN 32
#define HID_MAX_PROV_NAME_LEN 32

typedef UINT8 tHID_STATUS;

enum {
    HID_SUCCESS,
    HID_ERR_NOT_REGISTERED,
    HID_ERR_ALREADY_REGISTERED,
    HID_ERR_NO_RESOURCES,
    HID_ERR_NO_CONNECTION,
    HID_ERR_INVALID_PARAM,
    HID_ERR_UNSUPPORTED,
    HID_ERR_UNKNOWN_COMMAND,
    HID_ERR_CONGESTED,
    HID_ERR_CONN_IN_PROCESS,
    HID_ERR_ALREADY_CONN,
    HID_ERR_DISCONNECTING,
    HID_ERR_SET_CONNABLE_FAIL,
    HID_ERR_HOST_UNKNOWN,
    HID_ERR_L2CAP_FAILED,
    HID_ERR_AUTH_FAILED,
    HID_ERR_SDP_BUSY,
    HID_ERR_INVALID = 0xFF
};

typedef struct desc_info {
    UINT16 dl_len;
    UINT8* dsc_list;
} tHID_DEV_DSCP_INFO;

typedef struct sdp_info {
    char svc_name[HID_MAX_SVC_NAME_LEN];
    char svc_descr[HID_MAX_SVC_DESCR_LEN];
    char prov_name[HID_MAX_PROV_NAME_LEN];
    UINT16 rel_num;
    UINT16 hpars_ver;
    UINT8 sub_class;
    UINT8 ctry_code;
    UINT16 sup_timeout;
    tHID_DEV_DSCP_INFO dscp_info;
    tSDP_DISC_REC* p_sdp_layer_rec;
} tHID_DEV_SDP_INFO;

#endif
