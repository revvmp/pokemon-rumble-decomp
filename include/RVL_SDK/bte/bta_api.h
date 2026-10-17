#ifndef RVL_SDK_BTE_BTA_API_H
#define RVL_SDK_BTE_BTA_API_H

#include <RVL_SDK/bte/bt_types.h>

typedef UINT8 tBTA_STATUS;

enum {
    BTA_SUCCESS = 0,
    BTA_FAILURE = 1
};

typedef UINT8 tBTA_SERVICE_ID;

enum {
    BTA_MAX_SERVICE_ID = 23
};

typedef UINT32 tBTA_SERVICE_MASK;
typedef UINT8 tBTA_DM_DISC;
typedef UINT8 tBTA_DM_CONN;
typedef UINT8 tBTA_DM_INQ_MODE;
typedef UINT8 tBTA_DM_INQ_FILT;

typedef UINT8 tBTA_AUTH_RESP;

enum {
    BTA_DM_AUTH_PERM = 0,
    BTA_DM_AUTH_TEMP = 1,
    BTA_DM_NOT_AUTH = 2
};

typedef UINT8 tBTA_PREF_ROLES;
typedef INT8 tBTA_DM_RSSI_VALUE;
typedef UINT8 tBTA_DM_LINK_QUALITY_VALUE;
typedef UINT8 tBTA_SIG_STRENGTH_MASK;

typedef UINT8 tBTA_DM_SEC_EVT;

enum {
    BTA_DM_ENABLE_EVT = 0,
    BTA_DM_DISABLE_EVT = 1,
    BTA_DM_PIN_REQ_EVT = 2,
    BTA_DM_AUTH_CMPL_EVT = 3,
    BTA_DM_AUTHORIZE_EVT = 4,
    BTA_DM_LINK_UP_EVT = 5,
    BTA_DM_LINK_DOWN_EVT = 6,
    BTA_DM_SIG_STRENGTH_EVT = 7
};

typedef UINT8 tBTA_DM_SEARCH_EVT;

enum {
    BTA_DM_INQ_RES_EVT = 0,
    BTA_DM_INQ_CMPL_EVT = 1,
    BTA_DM_DISC_RES_EVT = 2,
    BTA_DM_DISC_CMPL_EVT = 3,
    BTA_DM_SEARCH_CANCEL_CMPL_EVT = 4
};

typedef UINT8 tBTA_DM_PM_ACTION;

typedef struct {
    DEV_CLASS dev_class;
    DEV_CLASS dev_class_mask;
} tBTA_DM_COD_COND;

typedef union {
    BD_ADDR bd_addr;
    tBTA_DM_COD_COND dev_class_cond;
} tBTA_DM_INQ_COND;

typedef struct {
    tBTA_DM_INQ_MODE mode;
    UINT8 duration;
    UINT8 max_resps;
    tBTA_DM_INQ_FILT filter_type;
    tBTA_DM_INQ_COND filter_cond;
} tBTA_DM_INQ;

typedef struct {
    BD_ADDR bd_addr;
} tBTA_DM_ENABLE;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    DEV_CLASS dev_class;
} tBTA_DM_PIN_REQ;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    BOOLEAN key_present;
    LINK_KEY key;
    BOOLEAN success;
} tBTA_DM_AUTH_CMPL;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    tBTA_SERVICE_ID service;
} tBTA_DM_AUTHORIZE;

typedef struct {
    BD_ADDR bd_addr;
} tBTA_DM_LINK_UP;

typedef struct {
    BD_ADDR bd_addr;
    UINT8 status;
} tBTA_DM_LINK_DOWN;

typedef struct {
    BD_ADDR bd_addr;
    tBTA_SIG_STRENGTH_MASK mask;
    tBTA_DM_RSSI_VALUE rssi_value;
    tBTA_DM_LINK_QUALITY_VALUE link_quality_value;
} tBTA_DM_SIG_STRENGTH;

typedef union {
    tBTA_DM_ENABLE enable;
    tBTA_DM_PIN_REQ pin_req;
    tBTA_DM_AUTH_CMPL auth_cmpl;
    tBTA_DM_AUTHORIZE authorize;
    tBTA_DM_LINK_UP link_up;
    tBTA_DM_LINK_DOWN link_down;
    tBTA_DM_SIG_STRENGTH sig_strength;
} tBTA_DM_SEC;

typedef void tBTA_DM_SEC_CBACK(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* p_data);

typedef struct {
    BD_ADDR bd_addr;
    DEV_CLASS dev_class;
    INT8 rssi;
    BOOLEAN remt_name_not_required;
} tBTA_DM_INQ_RES;

typedef struct {
    UINT8 num_resps;
} tBTA_DM_INQ_CMPL;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    tBTA_SERVICE_MASK services;
    tBTA_STATUS result;
} tBTA_DM_DISC_RES;

typedef union {
    tBTA_DM_INQ_RES inq_res;
    tBTA_DM_INQ_CMPL inq_cmpl;
    tBTA_DM_DISC_RES disc_res;
} tBTA_DM_SEARCH;

typedef void tBTA_DM_SEARCH_CBACK(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* p_data);

#endif
