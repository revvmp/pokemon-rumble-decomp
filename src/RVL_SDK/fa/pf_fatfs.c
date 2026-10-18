#include <revolution/fa/pf_fatfs.h>
#include <revolution/fa/pf_volume.h>

s32 PFFATFS_initializeFATFS(u32 config, void* p_parameter) {
    return PFVOL_InitModule(config, p_parameter);
}
