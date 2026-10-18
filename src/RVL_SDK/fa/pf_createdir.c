#include <revolution/fa/pf_createdir.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_str.h>

int pf2_createdir(const char* path, s32 mode, PF_DTA* p_dta) {
    s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFDIR_createdir(&path_str, mode, p_dta);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
