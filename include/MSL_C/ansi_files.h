#ifndef MSL_C_ANSI_FILES_H
#define MSL_C_ANSI_FILES_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define console_buff_mode _IOLBF
#define console_buff_size 256

typedef unsigned char console_buff[console_buff_size];

void __close_all(void);
unsigned int __flush_all(void);

#ifdef __cplusplus
}
#endif

#endif
