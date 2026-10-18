#include <game/BModelObj.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>

nw4r::math::VEC3 GetNodePosition(std::tr1::shared_ptr<BModelObj> pObj, const char* pName) {
    if (pObj && pObj->GetModel()) {
        nw4r::g3d::ScnMdlSimple* pScnMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(pObj->GetModel()->GetScnObj());
        if (pScnMdl != nullptr) {
            nw4r::math::MTX34 mtx;
            nw4r::g3d::ResNode node = pScnMdl->GetResMdl().GetResNode(pName);
            pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, node.GetID());
            return nw4r::math::VEC3(mtx._03, mtx._13, mtx._23);
        }
    }

    return nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
}
