#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/btm_int.h>
#include <string.h>

tBTM_CB btm_cb;

void btm_init(void) {
    memset(&btm_cb, 0, sizeof btm_cb);

    btm_cb.trace_level = BT_TRACE_LEVEL_NONE;

    btm_inq_db_init();
    btm_acl_init();
    btm_sec_init(BTM_SEC_MODE_SERVICE);
    btm_sco_init();
    btm_dev_init();
}
