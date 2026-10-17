#include <revolution/bte/gap_int.h>

void GAP_Init(void) {
    memset(&gap_cb, 0, sizeof(gap_cb));

    gap_cb.btm_cback[0] = &gap_btm_cback0;
    gap_cb.btm_cback[1] = &gap_btm_cback1;

    gap_cb.trace_level = BT_TRACE_LEVEL_DEBUG;

    gap_conn_init();
}
