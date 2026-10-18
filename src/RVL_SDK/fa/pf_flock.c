#include <revolution/fa/pf_flock.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_flock(PF_FILE* p_file, u32 mode) {
    return PFAPI_convertReturnValue(PFFILE_flock(p_file, mode | 8));
}
