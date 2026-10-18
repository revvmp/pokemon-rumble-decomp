#ifndef MSL_C_WSTRTOLD_H
#define MSL_C_WSTRTOLD_H

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

double __wcstold(int max_width, wint_t (*read)(void*, wint_t, int),
                 void* read_arg, int* chars_scanned, int* overflow);

#ifdef __cplusplus
}
#endif

#endif
