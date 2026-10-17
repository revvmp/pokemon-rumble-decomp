#ifndef NW4R_SND_SOUND_MEMORY_ALLOCATABLE_H
#define NW4R_SND_SOUND_MEMORY_ALLOCATABLE_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

class SoundMemoryAllocatable {
public:
    virtual ~SoundMemoryAllocatable() {}
    virtual void* Alloc(u32 size) = 0;
};

}
}

#endif
