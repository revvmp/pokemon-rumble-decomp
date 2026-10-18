#include <revolution/fa/pf_finfo.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_finfo(PF_FILE* p_file, PF_INFO* p_info) {
    return PFAPI_convertReturnValue(PFFILE_finfo(p_file, p_info));
}
