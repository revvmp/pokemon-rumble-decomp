#include <revolution/fa/pf_init_prfile2.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_fatfs.h>
#include <revolution/fa/pf_system.h>

s32 pf2_init_prfile2(s32 config, void* parameter) {
    s32 err = PFFATFS_initializeFATFS(config, parameter);
    if (err == 0) {
        PFSYS_initializeSYS();
    }
    return PFAPI_convertReturnValue(err);
}
