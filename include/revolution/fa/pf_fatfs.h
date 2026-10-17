#ifndef FA_PF_FATFS_H
#define FA_PF_FATFS_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFATFS_initializeFATFS(u32 config, void* p_parameter);

#ifdef __cplusplus
}
#endif

#endif
