#ifndef MSL_C_MEM_FUNCS_H
#define MSL_C_MEM_FUNCS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void __copy_longs_aligned(void* dst, const void* src, size_t count);
void __copy_longs_rev_aligned(void* dst, const void* src, size_t count);
void __copy_longs_unaligned(void* dst, const void* src, size_t count);
void __copy_longs_rev_unaligned(void* dst, const void* src, size_t count);

#ifdef __cplusplus
}
#endif

#endif
