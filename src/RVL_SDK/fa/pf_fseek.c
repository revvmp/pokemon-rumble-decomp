#include <revolution/fa/pf_fseek.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_fseek(PF_FILE* p_file, s32 offset, int origin) {
    return PFAPI_convertReturnValue(PFFILE_fseek(p_file, offset, origin));
}
