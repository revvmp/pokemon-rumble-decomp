#include <revolution/fa/pf_chmod.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_str.h>

int pf2_chmod(const char* path, int attr) {
    s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFDIR_chmod(&path_str, attr);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
