#include <revolution/fa/pf_code.h>

const u8 pf_valid_fn_char[96] = {
    1, 3, 0, 3, 3, 3, 3, 3, 3, 3, 0, 1, 1, 3, 3, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 1, 0, 1, 0, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 0, 1, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 0,
};

s32 PFCODE_Combine_Width(s16 oem_width, s16 uni_width) {
    return (oem_width << 16) + uni_width;
}

void PFCODE_Divide_Width(s32 width, s16* oem_width, s16* uni_width) {
    *oem_width = (u32)width >> 16;
    *uni_width = width;
}
