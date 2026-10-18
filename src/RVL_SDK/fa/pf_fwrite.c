#include <revolution/fa/pf_fwrite.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

u32 pf2_fwrite(void* buffer, u32 size, u32 count, PF_FILE* p_file) {
    u32 completed;
    PFFILE_fwrite(buffer, size, count, p_file, &completed);
    return completed;
}
