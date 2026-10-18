#ifndef FA_PF_SYSTEM_H
#define FA_PF_SYSTEM_H

#include <revolution/types.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSMessage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_STUB_CONTROL {
    u32 initialized;
    OSThread* p_thread;
    OSMessageQueue* p_mailbox;
    u8 stack[0x2000];
    OSThread thread;
    OSMessageQueue mailbox;
    OSMessage messages[20];
} PF_STUB_CONTROL;

typedef struct PF_SYSTEM_SET {
    PF_STUB_CONTROL stub;
    u32 flock_count;
} PF_SYSTEM_SET;

extern PF_SYSTEM_SET pf_sys_set;

s32 PFSYS_GetCurrentContextID(s32* p_context_id);
void PFSYS_initializeSYS(void);

typedef struct PF_SYS_DATE {
    u32 sys_year;
    u32 sys_month;
    u32 sys_day;
} PF_SYS_DATE;

typedef struct PF_SYS_TIME {
    u32 sys_hour;
    u32 sys_min;
    u32 sys_sec;
    u32 sys_ms;
} PF_SYS_TIME;

void PFSYS_TimeStamp(PF_SYS_DATE* p_date, PF_SYS_TIME* p_time);

#ifdef __cplusplus
}
#endif

#endif
