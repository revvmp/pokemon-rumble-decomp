#ifndef FA_PF_SYSTEM_H
#define FA_PF_SYSTEM_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

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
