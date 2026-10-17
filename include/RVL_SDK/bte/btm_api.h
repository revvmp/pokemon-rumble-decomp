#ifndef RVL_SDK_BTE_BTM_API_H
#define RVL_SDK_BTE_BTM_API_H

#include "RVL_SDK/bte/bt_types.h"

typedef UINT8 tBTM_STATUS;

enum {
    BTM_SUCCESS = 0,
    BTM_CMD_STARTED = 1
};

#define BTM_PM_SET_ONLY_ID 0x80

typedef struct {
    UINT16 max;
    UINT16 min;
    UINT16 attempt;
    UINT16 timeout;
    UINT8 mode;
} tBTM_PM_PWR_MD;

enum {
    BTM_SEC_NONE = 0
};

enum {
    BTM_SEC_PROTO_HID = 6
};

enum {
    BTM_SEC_SERVICE_HID_SEC_CTRL = 32,
    BTM_SEC_SERVICE_HID_NOSEC_CTRL = 33,
    BTM_SEC_SERVICE_HID_INTR = 34
};

tBTM_STATUS BTM_SetPowerMode(UINT8 pm_id, BD_ADDR remote_bda, tBTM_PM_PWR_MD* p_mode);
BOOLEAN BTM_SetSecurityLevel(BOOLEAN is_originator, char* p_name, UINT8 service_id, UINT8 sec_level, UINT16 psm,
                             UINT32 mx_proto_id, UINT32 mx_chan_id);

#endif
