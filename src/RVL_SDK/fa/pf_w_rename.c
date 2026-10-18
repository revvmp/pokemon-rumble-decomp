#include <revolution/fa/pf_w_rename.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

int pf2_w_rename(const u16* src, const u16* dst) {
    s32 err;
    PF_STR src_str;
    PF_STR dst_str;

    err = PFSTR_InitStr(&src_str, (const s8*)src, PF_CODE_MODE_UNICODE);
    err |= PFSTR_InitStr(&dst_str, (const s8*)dst, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFDIR_rename(&src_str, &dst_str);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
