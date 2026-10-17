#ifndef RVL_SDK_BTE_HIDD_API_H
#define RVL_SDK_BTE_HIDD_API_H

#include <revolution/bte/hiddefs.h>

enum {
    HID_DEV_EVT_OPEN,
    HID_DEV_EVT_CLOSE,
    HID_DEV_EVT_RETRYING,
    HID_DEV_EVT_MODE_CHG,
    HID_DEV_EVT_PM_FAILED
};

enum {
    HID_DEV_PM_ACTIVE = 0,
    HID_DEV_PM_SNIFF = 2,
    HID_DEV_PM_PARK = 3,
    HID_DEV_PM_NONE = 0xFF
};

enum {
    HID_DEV_SUBSTATE_ACTIVE,
    HID_DEV_SUBSTATE_IDLE,
    HID_DEV_SUBSTATE_SUSPEND
};

typedef struct {
    UINT8 rep_type;
    UINT8 rep_id;
} tHID_DEV_REP_DATA;

typedef struct pwr_md {
    UINT16 max;
    UINT16 min;
    UINT16 attempt;
    UINT16 timeout;
    UINT8 mode;
} tHID_DEV_PWR_MD;

typedef struct curr_pm_setting {
    UINT8 mode;
    UINT16 interval;
} tHID_DEV_CURR_PM_SETTING;

typedef struct {
    FLOW_SPEC ctrl_ch;
    FLOW_SPEC int_ch;
    FLOW_SPEC hci;
} tQOS_INFO;

typedef union {
    BD_ADDR host_bdaddr;
    BT_HDR* buffer;
    tHID_DEV_REP_DATA get_rep;
    UINT8 pm_err_code;
    UINT16 pm_interval;
} tHID_DEV_REG_INFO_CBACK_DATA;

typedef void tHID_DEV_REG_INFO_CBACK(UINT8 event, UINT32 data, tHID_DEV_REG_INFO_CBACK_DATA* pdata);

void HID_DevInit(void);

#endif
