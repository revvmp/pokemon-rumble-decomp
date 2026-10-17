#ifndef RVL_SDK_BTE_HID_CONN_H
#define RVL_SDK_BTE_HID_CONN_H

#include "RVL_SDK/bte/gki.h"

enum {
    HID_CONN_STATE_UNUSED,
    HID_CONN_STATE_CONNECTING_CTRL,
    HID_CONN_STATE_CONNECTING_INTR,
    HID_CONN_STATE_CONFIG,
    HID_CONN_STATE_CONNECTED,
    HID_CONN_STATE_DISCONNECTING,
    HID_CONN_STATE_SECURITY
};

enum {
    HID_CONN_FLAGS_IS_ORIG = 1 << 0,
    HID_CONN_FLAGS_THEIR_CTRL_CFG_DONE = 1 << 1,
    HID_CONN_FLAGS_OUR_CTRL_CFG_DONE = 1 << 2,
    HID_CONN_FLAGS_THEIR_INTR_CFG_DONE = 1 << 3,
    HID_CONN_FLAGS_OUR_INTR_CFG_DONE = 1 << 4,
    HID_CONN_FLAGS_CONGESTED = 1 << 5,
    HID_CONN_FLAGS_INACTIVE = 1 << 6
};

enum {
    HID_SEC_CHN = 1,
    HID_NOSEC_CHN = 2
};

typedef struct hid_conn {
    UINT8 conn_state;
    UINT8 conn_flags;
    UINT8 ctrl_id;
    UINT16 ctrl_cid;
    UINT16 intr_cid;
    UINT16 rem_mtu_size;
    UINT16 disc_reason;
    TIMER_LIST_ENT timer_entry;
} tHID_CONN;

#endif
