#include <game/Memory.h>

#include <revolution/mem.h>
#include <revolution/os.h>
#include <revolution/os/OSArena.h>
#include <revolution/types.h>

namespace GameMemory {
namespace {
MEMHeapHandle sHeapMEM1;
MEMHeapHandle sHeapMEM2;
}
namespace {
void* AllocateCallback(MEMAllocator*, u32 size) throw() {
    return Allocate(size);
}
void FreeCallback(MEMAllocator*, void* pMemory) throw() {
    Free(pMemory);
}
MEMAllocatorFunc sAllocatorFunctions = { AllocateCallback, FreeCallback };
MEMAllocator sAllocator = { &sAllocatorFunctions, nullptr, 0, 0 };
}
void Initialize() throw() {
    if (OSGetMEM1ArenaHi() == reinterpret_cast<void*>(0x81800000)) {
        OSSetMEM1ArenaHi(static_cast<u8*>(OSGetMEM1ArenaHi()) - 32);
    }
    u8* pLow = static_cast<u8*>(OSGetMEM1ArenaLo());
    u32 size = static_cast<u8*>(OSGetMEM1ArenaHi()) - pLow;
    void* pMemory = OSAllocFromMEM1ArenaHi(size, 32);
    sHeapMEM1 = MEMCreateExpHeapEx(pMemory, size,
        MEM_HEAP_OPT_0_CLEAR | MEM_HEAP_OPT_DEBUG_FILL | MEM_HEAP_OPT_THREAD_SAFE);
    u32 high = reinterpret_cast<u32>(OSGetMEM2ArenaHi()) & ~63;
    u32 low = (reinterpret_cast<u32>(OSGetMEM2ArenaLo()) + 63) & ~63;
    sHeapMEM2 = MEMCreateExpHeapEx(reinterpret_cast<void*>(low), high - low,
        MEM_HEAP_OPT_0_CLEAR | MEM_HEAP_OPT_DEBUG_FILL | MEM_HEAP_OPT_THREAD_SAFE);
    OSSetMEM2ArenaHi(nullptr);
    OSSetMEM2ArenaLo(nullptr);
}
void* Allocate(u32 size, int alignment) throw() {
    void* pMemory = MEMAllocFromExpHeapEx(sHeapMEM1, size, alignment);
    if (pMemory != nullptr) {
        return pMemory;
    }
    return MEMAllocFromExpHeapEx(sHeapMEM2, size, alignment);
}
void Free(void* pMemory) throw() {
    if (pMemory != nullptr) {
        MEMFreeToExpHeap(MEMFindContainHeap(pMemory), pMemory);
    }
}
void* AllocateMEM1(u32 size, int alignment) throw() {
    return MEMAllocFromExpHeapEx(sHeapMEM1, size, alignment);
}
void* TryAllocateMEM1(u32 size, int alignment) throw() {
    void* pMemory = MEMAllocFromExpHeapEx(sHeapMEM1, size, alignment);
    if (pMemory != nullptr) {
        return pMemory;
    }
    return nullptr;
}
void* AllocateMEM2(u32 size, int alignment) throw() {
    return MEMAllocFromExpHeapEx(sHeapMEM2, size, alignment);
}
void* TryAllocateMEM2(u32 size, int alignment) throw() {
    void* pMemory = MEMAllocFromExpHeapEx(sHeapMEM2, size, alignment);
    if (pMemory != nullptr) {
        return pMemory;
    }
    return nullptr;
}
}
MEMAllocator* GetDefaultAllocator() {
    return &GameMemory::sAllocator;
}
extern "C" void* __sys_alloc(unsigned int size) {
    return GameMemory::Allocate(size, 32);
}
extern "C" void __sys_free(void* pMemory) {
    GameMemory::Free(pMemory);
}
