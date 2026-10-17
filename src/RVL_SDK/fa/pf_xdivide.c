#include <revolution/fa/pf_xdivide.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_xdivide(const char* src, const char* dst, u32 offset) {
    s32 err;
    PF_STR src_str;
    PF_STR dst_str;

    err = PFSTR_InitStr(&src_str, (const s8*)src, PF_CODE_MODE_OEM);
    err |= PFSTR_InitStr(&dst_str, (const s8*)dst, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFFILE_xdivide(&src_str, &dst_str, offset);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
