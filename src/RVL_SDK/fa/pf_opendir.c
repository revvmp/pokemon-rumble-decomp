#include <revolution/fa/pf_opendir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

PF_DIR* pf2_opendir(const char* path) {
    s32 err;
    PF_STR path_str;
    PF_DIR* p_dir;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFDIR_opendir(&path_str, &p_dir);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue2NULL(err, p_dir);
}
