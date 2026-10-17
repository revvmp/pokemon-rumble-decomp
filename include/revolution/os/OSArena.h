#ifndef REVOLUTION_OS_OSARENA_H
#define REVOLUTION_OS_OSARENA_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

void* OSGetMEM1ArenaHi(void);
void* OSGetMEM2ArenaHi(void);
void* OSGetArenaHi(void);
void* OSGetMEM1ArenaLo(void);
void* OSGetMEM2ArenaLo(void);
void* OSGetArenaLo(void);
void OSSetMEM1ArenaHi(void *pNew);
void OSSetMEM2ArenaHi(void *pNew);
void OSSetArenaHi(void *pNew);
void OSSetMEM1ArenaLo(void *pNew);
void OSSetMEM2ArenaLo(void *pNew);
void OSSetArenaLo(void *pNew);
void* OSAllocFromMEM1ArenaLo(u32 size, u32 alignment);
void* OSAllocFromMEM1ArenaHi(u32 size, u32 alignment);

#ifdef __cplusplus
}
#endif

#endif
