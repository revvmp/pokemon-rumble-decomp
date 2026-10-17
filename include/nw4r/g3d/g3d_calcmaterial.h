#ifndef NW4R_G3D_CALC_MATERIAL_H
#define NW4R_G3D_CALC_MATERIAL_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/res/g3d_resmdl.h"

namespace nw4r {
namespace g3d {

class AnmObjTexPat;
class AnmObjTexSrt;
class AnmObjMatClr;

void CalcMaterialDirectly(ResMdl mdl, AnmObjTexPat *pAnmTexPat, AnmObjTexSrt *pAnmTexSrt, AnmObjMatClr *pAnmMatClr);

}
}

#endif
