#include <revolution/fa/pf_code.h>

u16 PF_GET_LE_U16(const u8* buf) {
    return (u16)(buf[1] << 8 | buf[0]);
}

void PF_LE16_TO_U16_STR(u8* str, u32 num) {
    u32 cnt;
    s8 tmp;

    for (cnt = 0; cnt < num; cnt += sizeof(u16)) {
        tmp = ((s8*)str)[cnt];
        str[cnt] = str[cnt + 1];
        str[cnt + 1] = tmp;
    }
}
