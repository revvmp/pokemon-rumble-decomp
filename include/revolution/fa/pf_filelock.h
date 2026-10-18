#ifndef FA_PF_FILELOCK_H
#define FA_PF_FILELOCK_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

void PF_InitLockFile(void);
s32 PF_LockFile(PF_FILE* p_file, PF_FILE* p_owner);
s32 PF_UnLockFile(PF_FILE* p_file);

#ifdef __cplusplus
}
#endif

#endif
