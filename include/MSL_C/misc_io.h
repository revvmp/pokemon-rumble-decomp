#ifndef MSL_C_MISC_IO_H
#define MSL_C_MISC_IO_H

#ifdef __cplusplus
extern "C" {
#endif

extern void (*__stdio_exit)(void);

void __stdio_atexit(void);

#ifdef __cplusplus
}
#endif

#endif
