#include <revolution/fa/pf_fadjust.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_fadjust(PF_FILE* p_file) {
    return PFAPI_convertReturnValue(PFFILE_fadjust(p_file));
}
