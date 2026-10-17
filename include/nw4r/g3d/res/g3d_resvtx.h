#ifndef NW4R_G3D_RES_RES_VTX_H
#define NW4R_G3D_RES_RES_VTX_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/math/math_types.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct ResVtxPosData {
    u32 size;
    s32 toResMdlData;
    s32 toVtxPosArray;
    s32 name;
    u32 id;
    GXCompCnt cmpcnt;
    GXCompType tp;
    u8 frac;
    u8 stride;
    u16 numPos;
    math::_VEC3 min;
    math::_VEC3 max;
};

class ResVtxPos : public ResCommon<ResVtxPosData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxPos);

    void Init() {
        DCStore(false);
    }

    void SetArray();
    void GetArray(const void **ppBase, u8 *pStride) const;

    void CopyTo(void *pDst) const;
    void DCStore(bool sync);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData() {
        ResVtxPosData &r = ref();

        if (r.toVtxPosArray != 0) {
            return reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toVtxPosArray);
        }

        return NULL;
    }
    const void *GetData() const {
        const ResVtxPosData &r = ref();

        if (r.toVtxPosArray != 0) {
            return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(&r) + r.toVtxPosArray);
        }

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumVtxPos() const {
        return ref().numPos;
    }
};

struct ResVtxNrmData {
    u32 size;
    s32 toResMdlData;
    s32 toVtxNrmArray;
    s32 name;
    u32 id;
    GXCompCnt cmpcnt;
    GXCompType tp;
    u8 frac;
    u8 stride;
    u16 numNrm;
};

class ResVtxNrm : public ResCommon<ResVtxNrmData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxNrm);

    void Init() {
        DCStore(false);
    }

    void SetArray();
    void GetArray(const void **ppBase, u8 *pStride) const;

    void CopyTo(void *pDst) const;
    void DCStore(bool);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData() {
        ResVtxNrmData &r = ref();

        if (r.toVtxNrmArray != 0) {
            return reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toVtxNrmArray);
        }

        return NULL;
    }
    const void *GetData() const {
        const ResVtxNrmData &r = ref();

        if (r.toVtxNrmArray != 0) {
            return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(&r) + r.toVtxNrmArray);
        }

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumVtxNrm() const {
        return ref().numNrm;
    }
};

struct ResVtxClrData {
    u32 size;
    s32 toResMdlData;
    s32 toVtxClrArray;
    s32 name;
    u32 id;
    GXCompCnt cmpcnt;
    GXCompType tp;
    u8 stride;
    u8 PADDING_0x1D;
    u16 numClr;
};

class ResVtxClr : public ResCommon<ResVtxClrData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxClr);

    void Init() {
        DCStore(false);
    }

    void SetArray(GXAttr attr);
    void GetArray(const void **ppBase, u8 *pStride) const;

    void CopyTo(void *pDst) const;
    void DCStore(bool sync);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData() {
        ResVtxClrData &r = ref();

        if (r.toVtxClrArray != 0) {
            return reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toVtxClrArray);
        }

        return NULL;
    }
    const void *GetData() const {
        const ResVtxClrData &r = ref();

        if (r.toVtxClrArray != 0) {
            return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(&r) + r.toVtxClrArray);
        }

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumVtxClr() const {
        return ref().numClr;
    }
};

struct ResVtxTexCoordData {
    u32 size;
    s32 toResMdlData;
    s32 toTexCoordArray;
    s32 name;
    u32 id;
    GXCompCnt cmpcnt;
    GXCompType tp;
    u8 frac;
    u8 stride;
    u16 numTexCoord;
    math::_VEC2 min;
    math::_VEC2 max;
};

class ResVtxTexCoord : public ResCommon<ResVtxTexCoordData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxTexCoord);

    void Init() {
        DCStore(false);
    }

    void GetArray(const void **ppBase, u8 *pStride) const;
    void DCStore(bool sync);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData() {
        ResVtxTexCoordData &r = ref();

        if (r.toTexCoordArray != 0) {
            return reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toTexCoordArray);
        }

        return NULL;
    }
    const void *GetData() const {
        const ResVtxTexCoordData &r = ref();

        if (r.toTexCoordArray != 0) {
            return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(&r) + r.toTexCoordArray);
        }

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumTexCoord() const {
        return ref().numTexCoord;
    }
};

struct ResVtxFurPosData {
    u32 size;
    s32 toResMdlData;
    s32 toFurPosArray;
    s32 name;
    u32 id;
    GXCompCnt cmpcnt;
    GXCompType tp;
    u8 frac;
    u8 stride;
    u16 numFurPos;
    u32 numLayer;
    u32 ofsLayer;
};
class ResVtxFurPos : public ResCommon<ResVtxFurPosData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxFurPos);

    void Init() {
        DCStore(false);
    }

    void GetArray(const void **ppBase, u8 *pStride) const;
    void SetArray(u16 idx);
    void DCStore(bool sync);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData(int idx) {
        ResVtxFurPosData &r = ref();

        return (r.toFurPosArray != 0 && idx < (int)r.numLayer && idx >= 0) ?
                   reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toFurPosArray + idx * r.ofsLayer) :
                   NULL;
    }

    const void *GetData(int idx) const {
        const ResVtxFurPosData &r = ref();

        return (r.toFurPosArray != 0 && idx < (int)r.numLayer && idx >= 0) ?
                   reinterpret_cast<const void *>(
                       reinterpret_cast<const u8 *>(&r) + r.toFurPosArray + idx * r.ofsLayer
                   ) :
                   NULL;

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumFurPos() const {
        return ref().toFurPosArray;
    }
    u32 GetNumLayer() const {
        return ref().numLayer;
    }
};

struct ResVtxFurVecData {
    u32 size;
    s32 toResMdlData;
    s32 toFurVecArray;
    s32 name;
    u32 id;
    u16 numFurVec;
};
class ResVtxFurVec : public ResCommon<ResVtxFurVecData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResVtxFurVec);

    void Init() {
        DCStore(false);
    }

    void GetArray(const void **ppBase, u8 *pStride) const;
    void DCStore(bool sync);

    u32 GetSize() const {
        return ref().size;
    }

    void *GetData() {
        ResVtxFurVecData &r = ref();

        if (r.toFurVecArray != 0) {
            return reinterpret_cast<void *>(reinterpret_cast<u8 *>(&r) + r.toFurVecArray);
        }

        return NULL;
    }
    const void *GetData() const {
        const ResVtxFurVecData &r = ref();

        if (r.toFurVecArray != 0) {
            return reinterpret_cast<const void *>(reinterpret_cast<const u8 *>(&r) + r.toFurVecArray);
        }

        return NULL;
    }

    u32 GetID() const {
        return ref().id;
    }

    u16 GetNumFurVec() const {
        return ref().toFurVecArray;
    }
};

}
}

#endif
