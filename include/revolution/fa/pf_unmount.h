#ifndef FA_PF_UNMOUNT_H
#define FA_PF_UNMOUNT_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_unmount(char drive, u32 mode);

#ifdef __cplusplus
}
#endif

#endif
