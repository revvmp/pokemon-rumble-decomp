#include <revolution/bte/bta_sys_int.h>

const tBTA_SYS_CFG bta_sys_cfg = {TASK_MBOX_2_EVT_MASK, TASK_MBOX_2, 1, 0};
tBTA_SYS_CFG* p_bta_sys_cfg = (tBTA_SYS_CFG*)&bta_sys_cfg;
