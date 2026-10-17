#ifndef NW4R_EF_MEMORY_MANAGER_H
#define NW4R_EF_MEMORY_MANAGER_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

class Effect;
class Emitter;
class Particle;
class ParticleManager;

class MemoryManagerBase {
public:
    MemoryManagerBase() {}
    virtual ~MemoryManagerBase() {}

    virtual void GarbageCollection() = 0;

    virtual Effect* AllocEffect() = 0;
    virtual void FreeEffect(void* pObject) = 0;
    virtual u32 GetNumAllocEffect() const = 0;
    virtual u32 GetNumActiveEffect() const = 0;
    virtual u32 GetNumFreeEffect() const = 0;

    virtual Emitter* AllocEmitter() = 0;
    virtual void FreeEmitter(void* pObject) = 0;
    virtual u32 GetNumAllocEmitter() const = 0;
    virtual u32 GetNumActiveEmitter() const = 0;
    virtual u32 GetNumFreeEmitter() const = 0;

    virtual ParticleManager* AllocParticleManager() = 0;
    virtual void FreeParticleManager(void* pObject) = 0;
    virtual u32 GetNumAllocParticleManager() const = 0;
    virtual u32 GetNumActiveParticleManager() const = 0;
    virtual u32 GetNumFreeParticleManager() const = 0;

    virtual Particle* AllocParticle() = 0;
    virtual void FreeParticle(void* pObject) = 0;
    virtual u32 GetNumAllocParticle() const = 0;
    virtual u32 GetNumActiveParticle() const = 0;
    virtual u32 GetNumFreeParticle() const = 0;

    virtual void* AllocHeap(u32 size) = 0;
    virtual void FreeHeap(void* pPtr) = 0;
};

}
}

#endif
