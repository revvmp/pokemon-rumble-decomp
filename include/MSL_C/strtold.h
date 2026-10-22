#ifndef MSL_C_STRTOLD_H
#define MSL_C_STRTOLD_H

#ifdef __cplusplus
extern "C" {
#endif

double __strtold(int max_width, int (*read)(void*, int, int), void* read_arg, int* chars_scanned, int* overflow);

#ifdef __cplusplus
}
#endif

#endif
