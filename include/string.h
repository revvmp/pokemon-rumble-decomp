#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
#define _MSL_CANT_THROW __attribute__((nothrow))
#else
#define _MSL_CANT_THROW
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void* dst, int value, size_t count) _MSL_CANT_THROW;
void* memcpy(void* dst, const void* src, size_t count) _MSL_CANT_THROW;
int memcmp(const void* lhs, const void* rhs, size_t count) _MSL_CANT_THROW;
void* memmove(void* dst, const void* src, size_t count) _MSL_CANT_THROW;
int strcmp(const char* lhs, const char* rhs) _MSL_CANT_THROW;
int strncmp(const char* lhs, const char* rhs, size_t count) _MSL_CANT_THROW;
char* strcat(char* dst, const char* src) _MSL_CANT_THROW;
char* strncat(char* dst, const char* src, size_t count) _MSL_CANT_THROW;
char* strcpy(char* dst, const char* src) _MSL_CANT_THROW;
char* strncpy(char* dst, const char* src, size_t count) _MSL_CANT_THROW;
size_t strlen(const char* str) _MSL_CANT_THROW;
char* strchr(const char* str, int c) _MSL_CANT_THROW;
char* strstr(const char* str, const char* pat) _MSL_CANT_THROW;
void* __memrchr(const void* src, int value, size_t count) _MSL_CANT_THROW;

#ifdef __cplusplus
}
#endif

#endif
