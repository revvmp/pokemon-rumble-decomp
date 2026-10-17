#include <string.h>

#include <revolution/bte/bt_trace.h>
#include <revolution/bte/port_api.h>
#include <revolution/bte/rfc_int.h>

void RFCOMM_Init(void) {
    memset(&rfc_cb, 0, sizeof(rfc_cb));

    rfc_cb.rfc.last_mux = MAX_BD_CONNECTIONS;

    rfc_cb.trace_level = BT_TRACE_LEVEL_DEBUG;

    rfcomm_l2cap_if_init();
}
