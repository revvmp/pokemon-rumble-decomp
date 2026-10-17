#include <revolution/bte/hidd_int.h>

#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btu.h>

tHID_DEV_CTB hd_cb;

void hidd_proc_repage_timeout(void) {
    HIDD_TRACE_DEBUG0("hidd_proc_repage_timeout");

    hd_cb.conn_tries++;

    if (hidd_conn_initiate() != HID_SUCCESS) {
        if (hd_cb.conn_tries > HIDD_MAX_CONN_RETRY) {
            hd_cb.dev_state = 0;
            (*hd_cb.callback)(HID_DEV_EVT_CLOSE, 0, NULL);
        } else {
            btu_start_timer(&hd_cb.conn.timer_entry, BTU_TTYPE_USER_FUNC, 1);
        }
    } else {
        (*hd_cb.callback)(HID_DEV_EVT_RETRYING, hd_cb.conn_tries, NULL);
    }
}
