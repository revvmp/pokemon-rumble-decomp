#include <revolution/bte/hidd_int.h>

#include <revolution/bte/bt_trace.h>

tHID_STATUS hidd_conn_initiate(void) {
    tHID_CONN* p_hcon = &hd_cb.conn;

    HIDD_TRACE_EVENT0("HID - Originate started");

    p_hcon->ctrl_cid = 0;
    p_hcon->intr_cid = 0;
    p_hcon->conn_flags = HID_CONN_FLAGS_IS_ORIG;

    if ((p_hcon->ctrl_cid = L2CA_ConnectReq(HID_PSM_CONTROL, hd_cb.host_addr)) == 0) {
        HIDD_TRACE_WARNING0("HID - Originate failed");
        return HID_ERR_L2CAP_FAILED;
    }

    p_hcon->conn_state = HID_CONN_STATE_CONNECTING_CTRL;
    return HID_SUCCESS;
}
