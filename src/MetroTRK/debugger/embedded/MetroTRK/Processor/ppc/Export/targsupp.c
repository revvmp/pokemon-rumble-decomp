#include <MetroTRK/ppc/Export/targsupp.h>

#pragma force_active on

asm u32 TRKAccessFile(MessageCommandID command, u32 handle, size_t* count, u8* buffer) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKOpenFile(u32 command, u32 handle, u32* count, u8* buffer) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKCloseFile(u32 command, u32 handle) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKPositionFile(u32 command, u32 handle, u32* position, u8* buffer) {
    nofralloc
    twi 31, r0, 0
    blr
}

#pragma force_active off
