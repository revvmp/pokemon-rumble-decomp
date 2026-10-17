#ifndef NW4R_G3D_CALC_VTX_H
#define NW4R_G3D_CALC_VTX_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/res/g3d_resmdl.h"

namespace nw4r {
namespace g3d {

class AnmObjShp;
struct ResVtxPosData;
struct ResVtxNrmData;
struct ResVtxClrData;

void CalcVtx(
    ResMdl mdl, AnmObjShp *pAnmShp, ResVtxPosData **ppVtxPosTable, ResVtxNrmData **ppVtxNrmTable,
    ResVtxClrData **ppVtxClrTable
);

}
}

#endif
