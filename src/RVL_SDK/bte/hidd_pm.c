#include "RVL_SDK/bte/hidd_int.h"

#include "RVL_SDK/bte/btm_api.h"
#include "RVL_SDK/bte/btu.h"

extern void* memcpy(void* p_dest, const void* p_src, unsigned long count);

#define HIDD_PM_IDLE_TIMEOUT 60

UINT8 hidd_pm_set_now(tHID_DEV_PWR_MD* p_req_mode) {
    tHID_DEV_PWR_MD act_pm = {0};
    tBTM_STATUS st = BTM_SUCCESS;

    if (hd_cb.pm_ctrl_busy ||
        (p_req_mode->mode == hd_cb.curr_pm.mode &&
         (p_req_mode->mode == HID_DEV_PM_ACTIVE ||
          (hd_cb.curr_pm.interval >= p_req_mode->min && hd_cb.curr_pm.interval <= p_req_mode->max)))) {
        hd_cb.final_pm.mode = HID_DEV_PM_NONE;
        return TRUE;
    }

    switch (p_req_mode->mode) {
    case HID_DEV_PM_ACTIVE:
        if (hd_cb.curr_pm.mode == HID_DEV_PM_SNIFF) {
            st = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD*)&act_pm);
            hd_cb.pm_ctrl_busy = TRUE;
        } else if (hd_cb.curr_pm.mode == HID_DEV_PM_PARK) {
            st = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD*)&act_pm);
            hd_cb.pm_ctrl_busy = TRUE;
        }
        break;

    case HID_DEV_PM_SNIFF:
        if (hd_cb.curr_pm.mode != HID_DEV_PM_ACTIVE) {
            hidd_pm_set_now(&act_pm);
        } else {
            st = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD*)p_req_mode);
            hd_cb.pm_ctrl_busy = TRUE;
        }
        break;

    case HID_DEV_PM_PARK:
        if (hd_cb.curr_pm.mode != HID_DEV_PM_ACTIVE) {
            hidd_pm_set_now(&act_pm);
        } else {
            st = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD*)p_req_mode);
            hd_cb.pm_ctrl_busy = TRUE;
        }
        break;
    }

    if (st == BTM_SUCCESS || st == BTM_CMD_STARTED)
        return TRUE;

    st += 55;

    if (hd_cb.callback)
        (*hd_cb.callback)(HID_DEV_EVT_PM_FAILED, hd_cb.conn_substate, (tHID_DEV_REG_INFO_CBACK_DATA*)&st);

    return FALSE;
}

UINT8 hidd_pm_set_power_mode(tHID_DEV_PWR_MD* p_req_mode) {
    memcpy(&hd_cb.final_pm, p_req_mode, sizeof(hd_cb.final_pm));
    return hidd_pm_set_now(p_req_mode);
}

void hidd_pm_proc_mode_change(UINT8 hci_status, UINT8 mode, UINT16 interval) {
    if (!hd_cb.reg_flag)
        return;

    hd_cb.pm_ctrl_busy = FALSE;

    if (hci_status != 0) {
        if (hd_cb.callback)
            (*hd_cb.callback)(HID_DEV_EVT_PM_FAILED, hd_cb.conn_substate, (tHID_DEV_REG_INFO_CBACK_DATA*)&hci_status);
        return;
    }

    hd_cb.curr_pm.mode = mode;
    hd_cb.curr_pm.interval = interval;

    if (hd_cb.final_pm.mode != HID_DEV_PM_NONE) {
        if (hd_cb.final_pm.mode != hd_cb.curr_pm.mode ||
            (hd_cb.final_pm.mode != HID_DEV_PM_ACTIVE &&
             (hd_cb.curr_pm.interval < hd_cb.final_pm.min || hd_cb.curr_pm.interval > hd_cb.final_pm.max))) {
            hidd_pm_set_now(&hd_cb.final_pm);
        } else {
            hd_cb.final_pm.mode = HID_DEV_PM_NONE;
        }
    } else if (hd_cb.curr_pm.mode == HID_DEV_PM_ACTIVE) {
        hidd_pm_start();
    }

    if (hd_cb.callback)
        (*hd_cb.callback)(HID_DEV_EVT_MODE_CHG, mode, (tHID_DEV_REG_INFO_CBACK_DATA*)&interval);
}

void hidd_pm_inact_timeout(void) {
    hidd_pm_set_power_mode(&hd_cb.pm_params[HID_DEV_SUBSTATE_IDLE]);
    hd_cb.conn_substate = HID_DEV_SUBSTATE_IDLE;
}

UINT8 hidd_pm_start(void) {
    hidd_pm_set_power_mode(&hd_cb.pm_params[HID_DEV_SUBSTATE_ACTIVE]);
    hd_cb.conn_substate = HID_DEV_SUBSTATE_ACTIVE;

    hd_cb.idle_tle.param = (TIMER_PARAM_TYPE)&hidd_pm_inact_timeout;
    btu_start_timer(&hd_cb.idle_tle, BTU_TTYPE_USER_FUNC, HIDD_PM_IDLE_TIMEOUT);

    return 0;
}
