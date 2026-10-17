#include <revolution/fa/pf_fclose.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_fclose(PF_FILE* p_file) {
    return PFAPI_convertReturnValue(PFFILE_fclose(p_file));
}
