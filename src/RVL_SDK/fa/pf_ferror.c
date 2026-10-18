#include <revolution/fa/pf_ferror.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_ferror(PF_FILE* p_file) {
    return PFAPI_convertError(PFFILE_ferror(p_file));
}
