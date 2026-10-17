#include <revolution/types.h>
#include <MetroTRK/Portable/mem_TRK.h>

static inline u8 ReadMemoryByte(const void* address) {
    const u32* word = (const u32*)((u32)address & ~3);
    u32 offset = (const u8*)address - (const u8*)word;
    return *word >> ((3 - offset) * 8);
}

static inline void WriteMemoryByte(void* address, u32 value) {
    u32 offset;
    u32 shift;
    u32* word;
    u32 mask;
    word = (u32*)((u32)address & ~3);
    offset = (u8*)address - (u8*)word;
    shift = (3 - offset) * 8;
    mask = 0xFFu << shift;
    *word = (*word & ~mask) | ((value << shift) & mask);
}

void* TRK_memcpy(void* dest, const void* source, size_t count) {
    u8* write = dest;
    const u8* read = source;
    while (count--) {
        WriteMemoryByte(write++, ReadMemoryByte(read++));
    }
    return dest;
}

void* TRK_memset(void* dest, int value, size_t count) {
    u8* write = dest;
    u8 byte = (u8)value;
    while (count--) {
        u32 offset;
        u32 shift;
        u32 mask;
        u32* word;
        word = (u32*)((u32)write & ~3);
        offset = write - (u8*)word;
        shift = (3 - offset) * 8;
        mask = 0xFFu << shift;
        *word = (*word & ~mask) | (((u32)byte << shift) & mask);
        write++;
    }
    return dest;
}
