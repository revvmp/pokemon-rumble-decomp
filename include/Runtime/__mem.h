#ifndef RUNTIME_MEM_H
#define RUNTIME_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void __fill_mem(void* dst, int value, unsigned long count);

#ifdef __cplusplus
}
#endif

#endif
