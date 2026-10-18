#include <revolution/fa/pf_w_opendir.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

PF_DIR* pf2_w_opendir(const u16* path) {
    s32 err;
    PF_STR path_str;
    PF_DIR* p_dir;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFDIR_opendir(&path_str, &p_dir);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue2NULL(err, p_dir);
}
