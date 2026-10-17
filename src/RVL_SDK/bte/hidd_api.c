#include "RVL_SDK/bte/hidd_int.h"

#include "RVL_SDK/bte/bt_trace.h"

extern void* memset(void* p_dest, int value, unsigned long count);

void HID_DevInit(void) {
    memset(&hd_cb, 0, sizeof(hd_cb));

    hd_cb.l2cap_ctrl_cfg.mtu_present = TRUE;
    hd_cb.l2cap_ctrl_cfg.mtu = 64;

    hd_cb.l2cap_int_cfg.mtu_present = TRUE;
    hd_cb.l2cap_int_cfg.mtu = 64;

    hd_cb.conn.timer_entry.param = (TIMER_PARAM_TYPE)&hidd_proc_repage_timeout;

    hd_cb.trace_level = BT_TRACE_LEVEL_NONE;
}
