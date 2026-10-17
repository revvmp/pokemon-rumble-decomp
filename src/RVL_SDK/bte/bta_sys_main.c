#include <revolution/bte/bta_sys_int.h>
#include <revolution/bte/bta_dm_int.h>
#include <revolution/bte/bt_trace.h>

void bta_dm_immediate_disable(void);

#ifndef BTA_SYS_TIMER_PERIOD
#define BTA_SYS_TIMER_PERIOD 1000
#endif

tBTA_SYS_CB bta_sys_cb;

UINT8 appl_trace_level;

void bta_sys_init(void) {
    memset(&bta_sys_cb, 0, sizeof bta_sys_cb);

    ptim_init(&bta_sys_cb.ptim_cb, BTA_SYS_TIMER_PERIOD, p_bta_sys_cfg->timer);

    bta_sys_cb.task_id = GKI_get_taskid();

    appl_trace_level = p_bta_sys_cfg->trace_level;
}

void bta_sys_event(BT_HDR* p_msg) {
    UINT8 id;
    BOOLEAN freebuf = TRUE;

    if (appl_trace_level >= BT_TRACE_LEVEL_EVENT)
        LogMsg_1(TRACE_ORG_APPL | TRACE_TYPE_EVENT, "BTA got event 0x%x", p_msg->event);

    id = (UINT8)(p_msg->event >> 8);

    if (bta_sys_cb.events_disabled) {
        if (p_msg->event == BTA_DM_API_DISABLE_EVT)
            bta_dm_immediate_disable();

        GKI_freebuf(p_msg);
        return;
    }

    if (id < BTA_ID_MAX && bta_sys_cb.reg[id]) {
        freebuf = (*bta_sys_cb.reg[id]->evt_hdlr)(p_msg);
    } else if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) {
        LogMsg_1(TRACE_ORG_APPL | TRACE_TYPE_WARNING, "BTA got unregistered event id %d", id);
    }

    if (freebuf)
        GKI_freebuf(p_msg);
}

void bta_sys_timer_update(void) {
    if (!bta_sys_cb.timers_disabled)
        ptim_timer_update(&bta_sys_cb.ptim_cb);
}

void bta_sys_register(UINT8 id, tBTA_SYS_REG const* p_reg) {
    bta_sys_cb.reg[id] = (tBTA_SYS_REG*)p_reg;
}

void bta_sys_sendmsg(void* p_msg) {
    GKI_send_msg(bta_sys_cb.task_id, p_bta_sys_cfg->mbox, p_msg);
}

void bta_sys_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, INT32 timeout) {
    ptim_start_timer(&bta_sys_cb.ptim_cb, p_tle, type, timeout);
}

void bta_sys_stop_timer(TIMER_LIST_ENT* p_tle) {
    ptim_stop_timer(&bta_sys_cb.ptim_cb, p_tle);
}

void bta_sys_disable(void) {
    int i;

    for (i = 0; i < BTA_ID_MAX; ++i) {
        if (bta_sys_cb.reg[i] && bta_sys_cb.reg[i]->disable)
            (*bta_sys_cb.reg[i]->disable)();
    }
}

void bta_sys_disable_timers(void) {
    bta_sys_cb.timers_disabled = TRUE;
}

void bta_sys_set_trace_level(UINT8 level) {
    appl_trace_level = level;
}
