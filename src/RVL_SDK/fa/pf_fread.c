#include <revolution/fa/pf_fread.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

u32 pf2_fread(void* buffer, u32 size, u32 count, PF_FILE* p_file) {
    u32 completed;
    PFFILE_fread(buffer, size, count, p_file, &completed);
    return completed;
}
