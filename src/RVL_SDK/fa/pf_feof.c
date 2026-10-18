#include <revolution/fa/pf_feof.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_feof(PF_FILE* p_file) {
    return PFAPI_convertReturnValue4feof(PFFILE_feof(p_file));
}
