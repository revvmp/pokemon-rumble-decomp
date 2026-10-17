#ifndef MSL_C_STRING_H
#define MSL_C_STRING_H

#ifndef __size_t_defined
#define __size_t_defined
typedef unsigned long size_t;
#endif

void* memcpy(void* dst, const void* src, size_t n);
void* memset(void* dst, int val, size_t n);
char* strncpy(char* dst, const char* src, size_t n);

#endif
