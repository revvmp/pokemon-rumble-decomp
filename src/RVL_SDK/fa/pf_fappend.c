#include <revolution/fa/pf_fappend.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

u32 pf2_fappend(PF_FILE* p_file, u32 count) {
    u32 appended;
    PFFILE_fappend(p_file, count, &appended);
    return appended;
}
