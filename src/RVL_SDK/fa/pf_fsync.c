#include <revolution/fa/pf_fsync.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_fsync(PF_FILE* p_file) {
    return PFAPI_convertReturnValue(PFFILE_fsync(p_file));
}
