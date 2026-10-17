#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void* dst, int value, size_t count);
void* memcpy(void* dst, const void* src, size_t count);
int strcmp(const char* lhs, const char* rhs);
char* strcat(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t count);

#ifdef __cplusplus
}
#endif

#endif
