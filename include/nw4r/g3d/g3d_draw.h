#ifndef NW4R_G3D_DRAW_H
#define NW4R_G3D_DRAW_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/res/g3d_resmat.h"
#include "nw4r/g3d/res/g3d_resmdl.h"
#include "nw4r/g3d/res/g3d_resnode.h"
#include "nw4r/g3d/res/g3d_resshp.h"

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

namespace G3DState {
class IndMtxOp;
}

namespace detail {

G3DState::IndMtxOp *GetIndMtxOp(ResMat mat, ResNode node, ResShp shp);

}

struct DrawResMdlReplacement {
    u32 flag;
    u8 *visArray;
    ResTexObjData *texObjDataArray;
    ResTlutObjData *tlutObjDataArray;
    ResTexSrtData *texSrtDataArray;
    ResChanData *chanDataArray;
    ResGenModeData *genModeDataArray;
    ResMatMiscData *matMiscDataArray;
    ResPixDL *pixDLArray;
    ResTevColorDL *tevColorDLArray;
    ResIndMtxAndScaleDL *indMtxAndScaleDLArray;
    ResTexCoordGenDL *texCoordGenDLArray;
    ResTevData *tevDataArray;
    ResVtxPosData **vtxPosTable;
    ResVtxNrmData **vtxNrmTable;
    ResVtxClrData **vtxClrTable;
};

void DrawResMdlDirectly(
    const ResMdl mdl, const math::MTX34 *pViewPosMtxArray, const math::MTX33 *pViewNrmMtxArray,
    const math::MTX34 *pViewEnvMtxArray, const u8 *pByteCodeOpa, const u8 *pByteCodeXlu,
    DrawResMdlReplacement *pReplacement, u32 drawMode
);

}
}

#endif
