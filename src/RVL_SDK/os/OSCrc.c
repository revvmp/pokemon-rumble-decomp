#include "revolution/types.h"

u32 OSCalcCRC32(const u8* array, s32 length) {
    static const u32 crc32_table[] = {
        0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
        0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C,
    };
    u32 crc = 0xFFFFFFFF;
    s32 i;

    for (i = 0; i != length; i++) {
        u32 val = *array;
        crc = (crc / 16) ^ crc32_table[(crc ^ val) & 0xF];
        crc = (crc / 16) ^ crc32_table[(crc ^ (val / 16)) & 0xF];
        array++;
    }

    return ~crc;
}
