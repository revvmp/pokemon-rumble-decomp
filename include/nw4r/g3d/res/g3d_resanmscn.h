#ifndef NW4R_G3D_RES_RES_ANM_SCN_H
#define NW4R_G3D_RES_RES_ANM_SCN_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_resanmamblight.h>
#include <nw4r/g3d/res/g3d_resanmcamera.h>
#include <nw4r/g3d/res/g3d_resanmfog.h>
#include <nw4r/g3d/res/g3d_resanmlight.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_reslightset.h>

namespace nw4r {
namespace g3d {

struct ResAnmScnInfoData {
    u16 numFrame;
    u16 numSpecularLight;
    AnmPolicy policy;
    u16 numResLightSetData;
    u16 numResAnmAmbLightData;
    u16 numResAnmLightData;
    u16 numResAnmFogData;
    u16 numResAnmCameraData;
    u8 PADDING_0x12[0x14 - 0x12];
};

struct ResAnmScnData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toScnTopLevelDic;
    s32 toResLightSetDataArray;
    s32 toResAnmAmbLightDataArray;
    s32 toResAnmLightDataArray;
    s32 toResAnmFogDataArray;
    s32 toResAnmCameraDataArray;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmScnInfoData info;
};

class ResAnmScn : public ResCommon<ResAnmScnData> {
public:
    static const u32 SIGNATURE = 'SCN0';
    static const int REVISION = 5;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmScn);

    bool Bind(const ResAnmScn scene);

    bool Bind() {
        return Bind(*this);
    }

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    bool HasResLightSet() const;
    bool HasResAnmAmbLight() const;
    bool HasResAnmLight() const;
    bool HasResAnmFog() const;
    bool HasResAnmCamera() const;

    ResLightSet GetResLightSet(int idx) const;
    ResLightSet GetResLightSet(u32 idx) const;
    u32 GetResLightSetNumEntries() const;

    ResAnmAmbLight GetResAnmAmbLight(const char *name) const;
    ResAnmAmbLight GetResAnmAmbLight(const ResName name) const;
    ResAnmAmbLight GetResAnmAmbLight(int idx) const;
    ResAnmAmbLight GetResAnmAmbLight(u32 idx) const;
    u32 GetResAnmAmbLightNumEntries() const;

    ResAnmLight GetResAnmLight(const char *name) const;
    ResAnmLight GetResAnmLight(const ResName name) const;
    ResAnmLight GetResAnmLight(int idx) const;
    ResAnmLight GetResAnmLight(u32 idx) const;
    u32 GetResAnmLightNumEntries() const;

    ResAnmFog GetResAnmFog(const char *name) const;
    ResAnmFog GetResAnmFog(const ResName name) const;
    ResAnmFog GetResAnmFog(int idx) const;
    ResAnmFog GetResAnmFog(u32 idx) const;
    u32 GetResAnmFogNumEntries() const;

    ResAnmCamera GetResAnmCamera(const char *name) const;
    ResAnmCamera GetResAnmCamera(const ResName name) const;
    ResAnmCamera GetResAnmCamera(int idx) const;
    ResAnmCamera GetResAnmCamera(u32 idx) const;
    u32 GetResAnmCameraNumEntries() const;

    ResLightSet GetResLightSetByRefNumber(u32 refNumber) const;
    ResAnmAmbLight GetResAnmAmbLightByRefNumber(u32 refNumber) const;
    ResAnmLight GetResAnmLightByRefNumber(u32 refNumber) const;
    ResAnmFog GetResAnmFogByRefNumber(u32 refNumber) const;
    ResAnmCamera GetResAnmCameraByRefNumber(u32 refNumber) const;

    u16 GetResAnmFogMaxRefNumber() const {
        return ref().info.numResAnmFogData;
    }

    u16 GetResAnmLightMaxRefNumber() const {
        return ref().info.numResAnmLightData;
    }

    u16 GetResAnmAmbLightMaxRefNumber() const {
        return ref().info.numResAnmAmbLightData;
    }

    u16 GetResLightSetMaxRefNumber()  const {
        return ref().info.numResLightSetData;
    }

    u16 GetSpecularLightNumber() const {
        return ref().info.numSpecularLight;
    }
};

}
}

#endif
