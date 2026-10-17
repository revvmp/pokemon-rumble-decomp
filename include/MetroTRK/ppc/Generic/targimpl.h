#ifndef PPC_GENERIC_TARGIMPL_H
#define PPC_GENERIC_TARGIMPL_H

#include <stddef.h>

#include <MetroTRK/Portable/nubevent.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKTargetAccessMemory(void* buffer, u32 address, size_t* length, MemoryAccessOptions accessMode, BOOL read);
DSError TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
u32 TRKTargetGetPC(void);
DSError TRKTargetSingleStep(u32 count, BOOL stepOver);
DSError TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver);
DSError TRKTargetStop(void);
DSError TRKTargetAddStopInfo(TRKBuffer* buffer);
void TRKTargetAddExceptionInfo(TRKBuffer* buffer);

DSError TRKTargetInterrupt(TRKEvent* event);
DSError TRKTargetSupportRequest(void);
BOOL TRKTargetStopped(void);

#ifdef __cplusplus
}
#endif

#endif
