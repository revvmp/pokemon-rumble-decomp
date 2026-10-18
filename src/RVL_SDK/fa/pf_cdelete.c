#include <revolution/fa/pf_cdelete.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_str.h>

u32 pf2_cdelete(const char* path, u32 offset, u32 num_clusters) {
    u32 num_processed = 0;
    PF_STR path_str;
    s32 err;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_OEM);
    if (err == 0) {
        PFFILE_cdelete(&path_str, offset, num_clusters, &num_processed);
    } else {
        pf_vol_set.last_error = err;
    }
    return num_processed;
}
