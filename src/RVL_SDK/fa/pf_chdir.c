#include <revolution/fa/pf_chdir.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>

int pf2_chdir(const char* path) {
    s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFDIR_chdir(&path_str);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
