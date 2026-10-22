#ifndef MSL_STDIO_H
#define MSL_STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include "MSL_C/file_struc.h"

#ifdef __cplusplus
extern "C" {
#endif

int vprintf(const char* format, va_list args);
int snprintf(char* dst, size_t count, const char* format, ...);
int vsnprintf(char* dst, size_t count, const char* format, va_list args);
int sprintf(char* buf, const char* format, ...);
#define EOF (-1)

int fclose(FILE* file);
long ftell(FILE* file);
long _ftell(FILE* file);
int fflush(FILE* file);
int fwide(FILE* file, int mode);
int _fseek(FILE* file, fpos_t offset, int mode);
size_t __fwrite(const void* buffer, size_t size, size_t count, FILE* file);

#ifdef __cplusplus
}
#endif

#endif
