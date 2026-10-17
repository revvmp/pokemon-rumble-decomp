#ifndef GAME_MEMORY_H
#define GAME_MEMORY_H

#include <revolution/mem.h>

MEMAllocator* GetDefaultAllocator();

namespace GameMemory {

void Initialize() throw();
void* Allocate(u32 size, int alignment = 32) throw();
void Free(void* pMemory) throw();
void* AllocateMEM1(u32 size, int alignment = 32) throw();
void* TryAllocateMEM1(u32 size, int alignment = 32) throw();
void* AllocateMEM2(u32 size, int alignment = 32) throw();
void* TryAllocateMEM2(u32 size, int alignment = 32) throw();

}

extern "C" void* __sys_alloc(unsigned int size);
extern "C" void __sys_free(void* pMemory);

#endif
