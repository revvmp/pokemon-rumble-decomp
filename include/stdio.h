#ifndef MSL_STDIO_H
#define MSL_STDIO_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int vprintf(const char* format, va_list args);
int snprintf(char* dst, size_t count, const char* format, ...);
int vsnprintf(char* dst, size_t count, const char* format, va_list args);
int sprintf(char* buf, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif
