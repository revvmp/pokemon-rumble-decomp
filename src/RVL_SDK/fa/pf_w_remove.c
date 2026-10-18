#include <revolution/fa/pf_w_remove.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

int pf2_w_remove(const u16* path) {
    s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFFILE_remove(&path_str);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
