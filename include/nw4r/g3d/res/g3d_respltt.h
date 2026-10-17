#ifndef NW4R_G3D_RES_RES_PLTT_H
#define NW4R_G3D_RES_RES_PLTT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct ResPlttData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toPlttData;
    s32 name;
    GXTlutFmt fmt;
    u16 numEntries;
    u16 PADDING_0x1E;
    s32 original_path;
    s32 toResUserData;
};

class ResPltt : public ResCommon<ResPlttData> {
public:
    static const u32 SIGNATURE = 'PLT0';
    static const int REVISION = 3;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResPltt);

    void Init() {
        DCStore(false);
    }

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const;

    void DCStore(bool sync);

    u16 *GetPlttData() {
        ResPlttData &r = ref();

        return r.toPlttData != 0
            ? reinterpret_cast<u16*>(reinterpret_cast<u8*>(&r) + r.toPlttData)
            : NULL;
    }

    const u16 *GetPlttData() const {
        const ResPlttData &r = ref();

        return r.toPlttData != 0
            ? reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(&r) + r.toPlttData)
            : NULL;
    }

    GXTlutFmt GetFmt() const {
        return ref().fmt;
    }

    u32 GetNumEntries() const {
        return ref().numEntries;
    }
};

}
}

#endif
