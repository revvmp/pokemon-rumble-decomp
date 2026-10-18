#include <revolution/fa/pf_cut.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_str.h>

int pf2_cut(const char* path, u32 offset) {
    u32 num_deleted;
    PF_STR path_str;
    s32 err;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        PFFILE_cdelete(&path_str, offset, 1, &num_deleted);
        if (num_deleted != 1) {
            err = -1;
        }
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
