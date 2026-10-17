#ifndef PPC_GENERIC_FLUSH_CACHE_H
#define PPC_GENERIC_FLUSH_CACHE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void TRK_flush_cache(u32 address, int length);

#ifdef __cplusplus
}
#endif

#endif
