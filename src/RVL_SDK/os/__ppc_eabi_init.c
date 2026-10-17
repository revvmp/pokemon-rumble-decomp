#include <revolution/types.h>
#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>
#include "Runtime/__ppc_eabi_init.h"

typedef void (*funcPtr)(void);

__declspec(section ".ctors") extern funcPtr _ctors[];
__declspec(section ".dtors") extern funcPtr _dtors[];

extern void __OSFPRInit(void);
extern void __OSPSInit(void);

static void __init_cpp(void);

#pragma section code_type ".init"

asm void __init_hardware(void) {
    nofralloc
    mfmsr r0
    ori r0, r0, 0x2000
    mtmsr r0
    mflr r31
    bl __OSPSInit
    bl __OSFPRInit
    bl __OSCacheInit
    mtlr r31
    blr
}

asm void __flush_cache(register void* address, register unsigned int size) {
    nofralloc
    lis r5, 0xFFFF
    ori r5, r5, 0xFFF1
    and r5, r5, address
    subf r3, r5, r3
    add size, size, r3

flush_block:
    dcbst 0, r5
    sync
    icbi 0, r5
    addic r5, r5, 8
    subic. size, size, 8
    bge flush_block
    isync
    blr
}

#pragma section code_type ".text"

asm void __init_user(void) {
    fralloc
    bl __init_cpp
    frfree
    blr
}

static void __init_cpp(void) {
    funcPtr* ctor;

    for (ctor = _ctors; *ctor; ctor++) {
        (*ctor)();
    }
}

static void __fini_cpp(void) {
    funcPtr* dtor;

    for (dtor = _dtors; *dtor; dtor++) {
        (*dtor)();
    }
}

__declspec(weak) void exit(int status) {
    __fini_cpp();
    PPCHalt();
}
