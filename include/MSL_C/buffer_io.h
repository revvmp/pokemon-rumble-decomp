#ifndef MSL_C_BUFFER_IO_H
#define MSL_C_BUFFER_IO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void __prep_buffer(FILE* file);
int __flush_buffer(FILE* file, size_t* length);

#ifdef __cplusplus
}
#endif

#endif
