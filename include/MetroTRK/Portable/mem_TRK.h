#ifndef METROTRK_PORTABLE_MEM_TRK_H
#define METROTRK_PORTABLE_MEM_TRK_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* TRK_memcpy(void* dest, const void* source, size_t count);
void* TRK_memset(void* dest, int value, size_t count);

#ifdef __cplusplus
}
#endif

#endif
