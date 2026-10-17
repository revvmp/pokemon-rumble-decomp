#ifndef RVL_SDK_BTE_HIDD_INT_H
#define RVL_SDK_BTE_HIDD_INT_H

#include <revolution/bte/hid_conn.h>
#include <revolution/bte/hidd_api.h>
#include <revolution/bte/l2c_api.h>

#define HIDD_MAX_CONN_RETRY 15

typedef struct hid_control_block {
    BD_ADDR host_addr;
    BOOLEAN host_known;
    UINT8 virtual_cable;
    UINT8 dev_state;
    UINT8 conn_tries;
    UINT8 sec_mask;
    UINT16 get_rep_buf_sz;
    tHID_CONN conn;
    TIMER_LIST_ENT idle_tle;
    tHID_DEV_PWR_MD pm_params[3];
    tHID_DEV_CURR_PM_SETTING curr_pm;
    BOOLEAN pm_ctrl_busy;
    UINT8 conn_substate;
    tHID_DEV_PWR_MD final_pm;
    BOOLEAN use_qos_flg;
    BOOLEAN unplug_on;
    tQOS_INFO qos_info;
    tHID_DEV_REG_INFO_CBACK* callback;
    tL2CAP_CFG_INFO l2cap_ctrl_cfg;
    tL2CAP_CFG_INFO l2cap_int_cfg;
    BOOLEAN reg_flag;
    UINT8 trace_level;
} tHID_DEV_CTB;

extern tHID_DEV_CTB hd_cb;

tHID_STATUS hidd_conn_initiate(void);
void hidd_proc_repage_timeout(void);
UINT8 hidd_pm_set_now(tHID_DEV_PWR_MD* p_req_mode);
UINT8 hidd_pm_set_power_mode(tHID_DEV_PWR_MD* p_req_mode);
void hidd_pm_proc_mode_change(UINT8 hci_status, UINT8 mode, UINT16 interval);
void hidd_pm_inact_timeout(void);
UINT8 hidd_pm_start(void);

#endif
