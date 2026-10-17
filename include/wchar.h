#ifndef MSL_WCHAR_H
#define MSL_WCHAR_H

#include <stddef.h>
#include <stdarg.h>
#include <wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

int vswprintf(wchar_t* dst, size_t count, const wchar_t* format, va_list args);

#ifdef __cplusplus
}
#endif

#endif
