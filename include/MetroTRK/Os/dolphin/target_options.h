#ifndef OS_DOLPHIN_TARGET_OPTIONS_H
#define OS_DOLPHIN_TARGET_OPTIONS_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void SetUseSerialIO(u8 enable);
u8 GetUseSerialIO(void);

#ifdef __cplusplus
}
#endif

#endif
