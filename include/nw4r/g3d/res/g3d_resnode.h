#ifndef NW4R_G3D_RES_RES_NODE_H
#define NW4R_G3D_RES_RES_NODE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resuser.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

struct ChrAnmResult;

struct ResNodeDataTypedef {
    enum Billboard {
        BILLBOARD_OFF,
        BILLBOARD_STD,
        BILLBOARD_PERSP_STD,
        BILLBOARD_ROT,
        BILLBOARD_PERSP_ROT,
        BILLBOARD_Y_OFF,
        BILLBOARD_PERSP_Y,

        NUM_BILLBOARD,
    };
};

struct ResNodeData : ResNodeDataTypedef {
    enum Flag {
        FLAG_IDENTITY = (1 << 0),
        FLAG_TRANS_ZERO = (1 << 1),
        FLAG_ROT_ZERO = (1 << 2),
        FLAG_SCALE_ONE = (1 << 3),
        FLAG_SCALE_UNIFORM = (1 << 4),

        FLAG_SSC_APPLY = (1 << 5),
        FLAG_SSC_PARENT = (1 << 6),

        FLAG_XSI_SCALING = (1 << 7),

        FLAG_VISIBLE = (1 << 8),
        FLAG_GEOMETRY = (1 << 9),
        FLAG_BILLBOARD_PARENT = (1 << 10)
    };

    u32 size;
    s32 toResMdlData;
    s32 name;
    u32 id;
    u32 mtxID;
    u32 flags;
    Billboard bbmode;
    u32 bbref_nodeid;
    math::_VEC3 scale;
    math::_VEC3 rot;
    math::_VEC3 translate;
    math::_VEC3 volume_min;
    math::_VEC3 volume_max;
    s32 toParentNode;
    s32 toChildNode;
    s32 toNextSibling;
    s32 toPrevSibling;
    s32 toResUserData;
    math::_MTX34 modelMtx;
    math::_MTX34 invModelMtx;
};

class ResNode : public ResCommon<ResNodeData>, public ResNodeDataTypedef {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResNode);

    void PatchChrAnmResult(ChrAnmResult *pResult) const;
    void CalcChrAnmResult(ChrAnmResult *pResult) const;

    ResName GetResName() const {
        const ResNodeData &r = ref();

        if (r.name != 0) {
            return NW4R_G3D_OFS_TO_RESNAME(&r, r.name);
        }

        return ResName(NULL);
    }

    const char *GetName() const {
        const ResNodeData &r = ref();
        return ofs_to_ptr<const char>(r.name);
    }

    u32 GetID() const {
        if (IsValid()) {
            return ptr()->id;
        }

        return 0;
    }

    u32 GetMtxID() const {
        return IsValid() ? ptr()->mtxID : 0;
    }

    bool IsVisible() const {
        if (IsValid()) {
            return ptr()->flags & ResNodeData::FLAG_VISIBLE;
        }

        return false;
    }

    void SetVisibility(bool visible) {
        if (!IsValid()) {
            return;
        }

        if (visible) {
            ptr()->flags |= ResNodeData::FLAG_VISIBLE;
        } else {
            ptr()->flags &= ~ResNodeData::FLAG_VISIBLE;
        }
    }

    Billboard GetBillboardMode() const {
        if (IsValid()) {
            return ptr()->bbmode;
        }

        return BILLBOARD_OFF;
    }

    const math::VEC3 &GetTranslate() const {
        return *(const math::VEC3 *)&ref().translate;
    }
    const math::VEC3 &GetScale() const {
        return *(const math::VEC3 *)&ref().scale;
    }

    const math::VEC3 &GetBoundsMin() const {
        return *(const math::VEC3 *)&ref().volume_min;
    }

    const math::VEC3 &GetBoundsMax() const {
        return *(const math::VEC3 *)&ref().volume_max;
    }

    ResNode GetParentNode() {
        return ofs_to_obj<ResNode>(ref().toParentNode);
    }
    ResNode GetParentNode() const {
        return ofs_to_obj<ResNode>(ref().toParentNode);
    }

    ResNode GetChildNode() {
        return ofs_to_obj<ResNode>(ref().toChildNode);
    }
    ResNode GetChildNode() const {
        return ofs_to_obj<ResNode>(ref().toChildNode);
    }

    ResNode GetNextSibling() {
        return ofs_to_obj<ResNode>(ref().toNextSibling);
    }
    ResNode GetNextSibling() const {
        return ofs_to_obj<ResNode>(ref().toNextSibling);
    }

    ResNode GetPrevSibling() {
        return ofs_to_obj<ResNode>(ref().toPrevSibling);
    }
    ResNode GetPrevSibling() const {
        return ofs_to_obj<ResNode>(ref().toPrevSibling);
    }

    ResUserData GetResUserData();

    void EndEdit() {}
};

}
}

#endif
