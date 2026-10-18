#include <revolution/fa/pf_rmdir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_rmdir(const char* path) {
    s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFDIR_rmdir(&path_str);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
