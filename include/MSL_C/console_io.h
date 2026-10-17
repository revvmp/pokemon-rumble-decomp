#ifndef MSL_C_CONSOLE_IO_H
#define MSL_C_CONSOLE_IO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int __read_console(__file_handle handle, unsigned char* buffer, size_t* count, __ref_con refCon);
int __write_console(__file_handle handle, unsigned char* buffer, size_t* count, __ref_con refCon);
int __close_console(__file_handle handle);

#ifdef __cplusplus
}
#endif

#endif
