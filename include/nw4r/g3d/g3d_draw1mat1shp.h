#ifndef NW4R_G3D_DRAW_1MAT_1SHP_H
#define NW4R_G3D_DRAW_1MAT_1SHP_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resmat.h>
#include <nw4r/g3d/res/g3d_resshp.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

namespace G3DState {
class IndMtxOp;
}

enum Draw1Mat1ShpCtrl {
    DRAW1MAT1SHP_CTRL_NOPPCSYNC = (1 << 0),
    DRAW1MAT1SHP_CTRL_NOSWAPSHP = (1 << 1),
    DRAW1MAT1SHP_CTRL_CULL_FRONT = (1 << 2),
    DRAW1MAT1SHP_CTRL_FORCE_LIGHTOFF = (1 << 3),
    DRAW1MAT1SHP_CTRL_IGNORE_MAT = (1 << 4),
};

struct Draw1Mat1ShpSwap {
    ResTexObj texObj;
    ResTlutObj tlutObj;
    ResGenMode genMode;
    ResTev tev;
    ResMatPix pix;
    ResMatTevColor tevColor;
    ResMatIndMtxAndScale indMtxAndScale;
    ResMatChan chan;
    ResMatTexCoordGen texCoordGen;
    ResMatMisc misc;
    ResTexSrt texSrt;
    ResVtxPosData **vtxPosTable;
    ResVtxNrmData **vtxNrmTable;
    ResVtxClrData **vtxClrTable;

    Draw1Mat1ShpSwap() : vtxPosTable(NULL), vtxNrmTable(NULL), vtxClrTable(NULL) {}
};

void Draw1Mat1ShpDirectly(
    ResMat mat, ResShp shp, const math::MTX34 *pViewPos, const math::MTX34 *pViewNrm, u32 ctrl, Draw1Mat1ShpSwap *pSwap,
    G3DState::IndMtxOp *pIndMtxOp
);

}
}

#endif
