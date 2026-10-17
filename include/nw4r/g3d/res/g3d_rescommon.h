#ifndef NW4R_G3D_RESCOMMON_H
#define NW4R_G3D_RESCOMMON_H
#include <nw4r/types_nw4r.h>

#include <revolution/GX.h>
#include <nw4r/g3d/platform/g3d_gxhardware.h>

#define NW4R_G3D_RESFILE_NAME_DEF(VAR, STR)                                                                            \
    nw4r::g3d::ResNameData27 ResNameData_##VAR ATTRIBUTE_ALIGN(32) = {sizeof(STR) - 1, STR}

#define NW4R_G3D_OFS_TO_RESNAME(BASE, OFS) nw4r::g3d::ResName((char *)(BASE) + (OFS) - sizeof(u32))

#define NW4R_G3D_RESOURCE_FUNC_DEF(T) NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(T, T##Data)
#define NW4R_G3D_RESOURCE_FUNC_DEF_EX(TCLS, TDATA) NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(TCLS, TDATA)

#define NW4R_G3D_RESOURCE_FUNC_DEF_IMPL(TCLS, TDATA)                                                                   \
    explicit TCLS(void *pData = NULL) : nw4r::g3d::ResCommon<TDATA>(pData) {}                                          \
                                                                                                                       \
    TDATA &ref() {                                                                                                     \
        return *ptr();                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    const TDATA &ref() const {                                                                                         \
        return *ptr();                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    bool operator==(const TCLS &rOther) const {                                                                        \
        return ptr() == rOther.ptr();                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    bool operator!=(const TCLS &rOther) const {                                                                        \
        return ptr() != rOther.ptr();                                                                                  \
    }

namespace nw4r {
namespace g3d {
struct AnmObjVis;
struct AnmObjVisNode;
struct AnmObjVisOR;
struct AnmObjVisRes;
struct AnmObj;
struct AnmObjChr;
struct FuncObjCalcWorld;
struct AnmScn;
struct AnmScnRes;
struct CameraAnmResult;
struct ChrAnmResult;
struct Draw1Mat1ShpSwap;
struct DrawResMdlReplacement;
struct FogAnmResult;
struct ResFile;
struct ResMdl;
struct ResPltt;
struct ResTex;
struct ResTexSrt;
struct ResMat;
struct ResTexPlttInfo;
struct ResShp;
struct ResFog;
struct ResAnmChr;
struct ResAnmVis;
struct ResAnmClr;
struct ResAnmTexPat;
struct ResAnmTexSrt;
struct ResAnmShp;
struct ResAnmScn;
struct ResAnmFog;
struct ResAnmCamera;
struct ResVtxPos;
struct ResVtxNrm;
struct ResVtxClr;
struct ResVtxTexCoord;
struct ClrAnmResult;
struct TexPatAnmResult;
struct TexSrtAnmResult;
struct ScnMdl1Mat1Shp;
struct TexSrt;
struct TexSrtTypedef;
struct LightObj;
struct LightAnmResult;
struct LightSet;
struct LightSetting;
struct LightSetData;
struct AmbLightObj;
struct AmbLightAnmResult;
struct Fog;
struct ScnRoot;
struct ScnProc;
struct ScnLeaf;
struct ScnObj;
struct ScnRfl;
struct ScnGroup;
struct ScnMdlSimple;
struct ScnMdl;
struct IScnObjGather;
struct IScnObjCallback;

namespace G3DState {
struct IndMtxOp;
}
}
}

namespace nw4r {
namespace g3d {

template <typename T>
class ResCommon {
public:
    explicit ResCommon(void *pData) : mpData(static_cast<T *>(pData)) {}

    explicit ResCommon(const void *pData) : mpData(static_cast<const T *>(pData)) {}

    bool IsValid() const {
        return mpData != NULL;
    }

    T *ptr() {
        return mpData;
    }
    const T *ptr() const {
        return mpData;
    }

    T &ref() {
        return *mpData;
    }
    const T &ref() const {
        return *mpData;
    }

    template <typename PTR_T>
    PTR_T *ofs_to_ptr_raw(s32 ofs) {
        return reinterpret_cast<PTR_T *>((char *)mpData + ofs);
    }
    template <typename PTR_T>
    const PTR_T *ofs_to_ptr_raw(s32 ofs) const {
        return reinterpret_cast<const PTR_T *>((char *)mpData + ofs);
    }

    template <typename PTR_T>
    PTR_T *ofs_to_ptr(s32 ofs) {
        u8 *pPtr = reinterpret_cast<u8 *>(mpData);

        if (ofs != 0) {
            return reinterpret_cast<PTR_T *>(pPtr + ofs);
        }

        return NULL;
    }
    template <typename PTR_T>
    const PTR_T *ofs_to_ptr(s32 ofs) const {
        const u8 *pPtr = reinterpret_cast<const u8 *>(mpData);

        if (ofs != 0) {
            return reinterpret_cast<const PTR_T *>(pPtr + ofs);
        }

        return NULL;
    }

    template <typename OBJ_T>
    OBJ_T ofs_to_obj(s32 ofs) {
        u8 *pPtr = reinterpret_cast<u8 *>(mpData);

        if (ofs != 0) {
            return OBJ_T(pPtr + ofs);
        }

        return OBJ_T(NULL);
    }
    template <typename OBJ_T>
    const OBJ_T ofs_to_obj(s32 ofs) const {
        const u8 *pPtr = reinterpret_cast<const u8 *>(mpData);

        if (ofs != 0) {
            return OBJ_T(const_cast<u8 *>(pPtr + ofs));
        }

        return OBJ_T(NULL);
    }

private:
    T *mpData;
};

struct ResBlockHeaderData {
    char kind[4];
    u32 size;
};

struct ResNameData27 {
    u32 len;
    char str[32 - sizeof(u32)];
};

struct ResNameData {
    u32 len;
    char str[4];
};

class ResName : public ResCommon<const ResNameData> {
public:
    explicit ResName(const void *pData) : ResCommon(pData) {}

    u32 GetLength() const {
        return ref().len;
    }

    const char *GetName() const {
        return ref().str;
    }

    bool operator==(const ResName rhs) const;
};

struct ResTagDLData {
    u32 bufSize;
    u32 cmdSize;
    s32 toDL;
};

class ResTagDL : public ResCommon<ResTagDLData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResTagDL);

    u32 GetBufSize() const {
        return ref().bufSize;
    }

    u32 GetCmdSize() const {
        return ref().cmdSize;
    }

    u8 *GetDL() {
        return const_cast<u8 *>(ofs_to_ptr<u8>(ref().toDL));
    }
    const u8 *GetDL() const {
        return ofs_to_ptr<u8>(ref().toDL);
    }
};

namespace ResByteCodeData {

enum OpCode {
    NOOP,
    END,
    CALC,
    WEIGHT,
    DRAW,
    EVPMTX,
    MTXDUP
};

struct CalcParams {
    u8 opcode;
    u8 nodeIdHi;
    u8 nodeIdLo;
    u8 parentMtxIdHi;
    u8 parentMtxIdLo;
};

struct WeightParams {
    u8 opcode;
    u8 tgtIdHi;
    u8 tgtIdLo;
    u8 numBlendMtx;
};
struct WeightEntry {
    u8 mtxIdHi;
    u8 mtxIdLo;
    u8 fWeight0;
    u8 fWeight1;
    u8 fWeight2;
    u8 fWeight3;
};

struct DrawParams {
    u8 opcode;
    u8 matIdHi;
    u8 matIdLo;
    u8 shpIdHi;
    u8 shpIdLo;
    u8 nodeIdHi;
    u8 nodeIdLo;
    u8 priority;
};

struct EvpMtxParams {
    u8 opcode;
    u8 mtxIdHi;
    u8 mtxIdLo;
    u8 nodeIdHi;
    u8 nodeIdLo;
};

struct MtxDupParams {
    u8 opcode;
    u8 toMtxIdHi;
    u8 toMtxIdLo;
    u8 fromMtxIdHi;
    u8 fromMtxIdLo;
};

}

namespace detail {
typedef u8 CPCmd[6];
typedef u8 BPCmd[5];

inline u8 ResRead_u8(const u8 *pPtr) {
    return *pPtr;
}

inline u32 ResRead_u32(const u8 *pPtr) {
    u32 value = ResRead_u8(pPtr++) << 24;
    value |= ResRead_u8(pPtr++) << 16;
    value |= ResRead_u8(pPtr++) << 8;
    value |= ResRead_u8(pPtr++) << 0;
    return value;
}

inline void ResWrite_u8(u8 *pPtr, u8 data) {
    *pPtr = data;
}

inline void ResWrite_u16(u8 *pPtr, u16 data) {
    ResWrite_u8(pPtr++, data >> 8);
    ResWrite_u8(pPtr++, data >> 0);
}

inline void ResWrite_u32(u8 *pPtr, u32 data) {
    ResWrite_u8(pPtr++, data >> 24);
    ResWrite_u8(pPtr++, data >> 16);
    ResWrite_u8(pPtr++, data >> 8);
    ResWrite_u8(pPtr++, data >> 0);
}

inline void ResReadBPCmd(const u8 *pPtr, u32 *pOut) {
    *pOut = ResRead_u32(pPtr + 1);
}

void ResWriteBPCmd(u8 *pPtr, u32 reg);
void ResWriteBPCmd(u8 *pPtr, u32 reg, u32 mask);
void ResWriteSSMask(u8 *pPtr, u32 value);

inline void ResReadCPCmd(const u8 *pPtr, u32 *pOut) {
    *pOut = ResRead_u32(pPtr + 2);
}

void ResWriteCPCmd(u8 *pPtr, u8 addr, u32 value);

inline void ResReadXFCmd(const u8 *pPtr, u32 *pOut) {
    *pOut = ResRead_u32(pPtr + 5);
}

void ResWriteXFCmd(u8 *pPtr, u16 addr, u32 value);

inline GXColor GetRGBA(u8 r, u8 g, u8 b, u8 a) {
    return (GXColor){r, g, b, a};
}
inline GXColorS10 GetRGBAS10(s16 r, s16 g, s16 b, s16 a) {
    return (GXColorS10){r, g, b, a};
}

}
}
}

#endif
