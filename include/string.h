#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void* dst, int value, size_t count);
void* memcpy(void* dst, const void* src, size_t count);
int memcmp(const void* lhs, const void* rhs, size_t count);
void* memmove(void* dst, const void* src, size_t count);
int strcmp(const char* lhs, const char* rhs);
int strncmp(const char* lhs, const char* rhs, size_t count);
char* strcat(char* dst, const char* src);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t count);
void* __memrchr(const void* src, int value, size_t count);

#ifdef __cplusplus
}
#endif

#endif
