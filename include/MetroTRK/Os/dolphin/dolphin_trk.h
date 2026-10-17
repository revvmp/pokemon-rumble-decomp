#ifndef OS_DOLPHIN_DOLPHIN_TRK_H
#define OS_DOLPHIN_DOLPHIN_TRK_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitMetroTRK(void);
void InitMetroTRK_BBA(void);
void EnableMetroTRKInterrupts(void);
u32 TRKTargetTranslate(u32 address);
DSError TRKInitializeTarget(void);
void __TRK_copy_vectors(void);
void __TRK_reset(void);

extern u32 _db_stack_addr;
extern u8 gTRKInterruptVectorTable[];

#ifdef __cplusplus
}
#endif

#endif
