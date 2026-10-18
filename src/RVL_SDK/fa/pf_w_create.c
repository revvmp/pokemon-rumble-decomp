#include <revolution/fa/pf_w_create.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

PF_FILE* pf2_w_create(const u16* path, int mode) {
    PF_FILE* p_file;
    PF_STR path_str;
    s32 err;

    (void)mode;
    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFFILE_fopen(&path_str, 0x19, &p_file);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue2NULL(err, p_file);
}
