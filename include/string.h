#ifndef MSL_STRING_H
#define MSL_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long size_t;

void* memcpy(void* dst, const void* src, size_t n);
void* memset(void* dst, int val, size_t n);

#ifdef __cplusplus
}
#endif

#endif
