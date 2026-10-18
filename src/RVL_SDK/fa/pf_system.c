#include <revolution/fa/pf_system.h>
#include <revolution/fa/pf_stub.h>
#include <revolution/fa/pf_str.h>
#include <revolution/os/OSTime.h>

PF_SYSTEM_SET pf_sys_set;

void PFSYS_initializeSYS(void) {
    pfstub_init_stub();
}

s32 PFSYS_GetCurrentContextID(s32* p_context_id) {
    *p_context_id = pf_vol_set.current_context_id;
    return 0;
}

void PFSYS_TimeStamp(PF_SYS_DATE* p_date, PF_SYS_TIME* p_time) {
    OSCalendarTime calendar;
    OSTicksToCalendarTime(OSGetTime(), &calendar);
    p_date->sys_year = calendar.year;
    p_date->sys_month = calendar.mon + 1;
    p_date->sys_day = calendar.mday;
    p_time->sys_hour = calendar.hour;
    p_time->sys_min = calendar.min;
    p_time->sys_sec = calendar.sec;
    p_time->sys_ms = 1;
}
