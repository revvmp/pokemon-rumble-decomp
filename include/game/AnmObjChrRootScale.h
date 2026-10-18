#ifndef GAME_ANMOBJCHRROOTSCALE_H
#define GAME_ANMOBJCHRROOTSCALE_H

#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

class AnmObjChrRootScale : public AnmObjChr {
public:
    static AnmObjChrRootScale* Construct(MEMAllocator* pAllocator, u32* pSize, ResMdl mdl);

    void SetScale(const math::VEC3& rScale);
};

}
}

void DestroyG3dObj(nw4r::g3d::G3dObj* pObj);

#endif