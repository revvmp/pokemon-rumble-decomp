#include <nw4r/g3d/res/g3d_resshp.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/platform/g3d_cpu.h>

#include <string.h>

namespace nw4r {
namespace g3d {

ResMdl ResShp::GetParent() const {
    return ofs_to_obj<ResMdl>(ref().toResMdlData);
}

void ResShp::GXSetArray(GXAttr attr, const void* pBase, u8 stride) {
    u8* pCmd = GetResShpPrePrim().ref().dl.array[attr - GX_VA_POS];
    u32 cpAttr = attr != GX_VA_NBT ? attr - GX_VA_POS : 1;

    detail::ResWriteCPCmd(&pCmd[GX_CP_CMD_SZ * 0], cpAttr + GX_CP_REG_ARRAYBASE,
                          OSCachedToPhysical(pBase));

    detail::ResWriteCPCmd(&pCmd[GX_CP_CMD_SZ * 1], cpAttr + GX_CP_REG_ARRAYSTRIDE, stride);
}

void ResShp::DisableSetArray(GXAttr attr) {
    u8* pCmd = GetResShpPrePrim().ref().dl.array[attr - GX_VA_POS];
    memset(pCmd, 0, GX_CP_CMD_SZ * 2);
}

ResVtxPos ResShp::GetResVtxPos() const {
    return GetParent().GetResVtxPos(ref().idVtxPosition);
}

ResVtxNrm ResShp::GetResVtxNrm() const {
    const ResShpData& r = ref();

    if (r.idVtxNormal != -1) {
        return GetParent().GetResVtxNrm(r.idVtxNormal);
    }

    return ResVtxNrm(nullptr);
}

ResVtxClr ResShp::GetResVtxClr(u32 idx) const {
    const ResShpData& r = ref();

    if (r.idVtxColor[idx] != -1) {
        return GetParent().GetResVtxClr(r.idVtxColor[idx]);
    }

    return ResVtxClr(nullptr);
}

ResVtxTexCoord ResShp::GetResVtxTexCoord(u32 idx) const {
    const ResShpData& r = ref();

    if (r.idVtxTexCoord[idx] != -1) {
        return GetParent().GetResVtxTexCoord(r.idVtxTexCoord[idx]);
    }

    return ResVtxTexCoord(nullptr);
}

ResVtxFurPos ResShp::GetResVtxFurPos() const {
    const ResShpData& r = ref();

    if (r.idVtxFurPos != -1) {
        return GetParent().GetResVtxFurPos(r.idVtxFurPos);
    }

    return ResVtxFurPos(nullptr);
}

void ResShp::Init() {
    const void* pBase;
    u8 stride;

    GetResVtxPos().GetArray(&pBase, &stride);
    GXSetArray(GX_VA_POS, pBase, stride);

    ResVtxNrm nrm = GetResVtxNrm();
    if (nrm.IsValid()) {
        nrm.GetArray(&pBase, &stride);
        GXSetArray(GX_VA_NRM, pBase, stride);
    }

    u32 i;

    for (i = 0; i < GX_VA_TEX0 - GX_VA_CLR0; i++) {
        ResVtxClr clr = GetResVtxClr(i);

        if (clr.IsValid()) {
            clr.GetArray(&pBase, &stride);
            GXSetArray(static_cast<GXAttr>(GX_VA_CLR0 + i), pBase, stride);
        }
    }

    for (i = 0; i < GX_POS_MTX_ARRAY - GX_VA_TEX0; i++) {
        ResVtxTexCoord txc = GetResVtxTexCoord(i);

        if (txc.IsValid()) {
            txc.GetArray(&pBase, &stride);
            GXSetArray(static_cast<GXAttr>(GX_VA_TEX0 + i), pBase, stride);
        }
    }

    GetResShpPrePrim().DCStore(false);

    ResShpData& r = ref();
    DC::StoreRangeNoSync(GetPrimDLTag().GetDL(), r.tagPrimDL.bufSize);
}

void ResShp::Terminate() {
    DisableSetArray(GX_VA_POS);
    DisableSetArray(GX_VA_NRM);

    DisableSetArray(GX_VA_CLR0);
    DisableSetArray(GX_VA_CLR1);

    DisableSetArray(GX_VA_TEX0);
    DisableSetArray(GX_VA_TEX1);
    DisableSetArray(GX_VA_TEX2);
    DisableSetArray(GX_VA_TEX3);
    DisableSetArray(GX_VA_TEX4);
    DisableSetArray(GX_VA_TEX5);
    DisableSetArray(GX_VA_TEX6);
    DisableSetArray(GX_VA_TEX7);
}

void ResShp::CallPrePrimitiveDisplayList(bool sync, bool cacheIsSame) const {
    const ResTagDL& rTag = GetPrePrimDLTag();

    if (cacheIsSame) {
        if (sync) {
            GXCallDisplayList(const_cast<u8*>(rTag.GetDL() + 32), rTag.GetCmdSize() - 32);
        } else {
            GXFastCallDisplayList(const_cast<u8*>(rTag.GetDL() + 32), rTag.GetCmdSize() - 32);
        }

        return;
    }

    if (sync) {
        GXCallDisplayList(const_cast<u8*>(rTag.GetDL()), rTag.GetCmdSize());
    } else {
        GXFastCallDisplayList(const_cast<u8*>(rTag.GetDL()), rTag.GetCmdSize());
    }
}

void ResShp::CallPrimitiveDisplayList(bool sync) const {
    const ResTagDL& rTag = GetPrimDLTag();

    if (sync) {
        GXCallDisplayList(const_cast<u8*>(rTag.GetDL()), rTag.GetCmdSize());
    } else {
        GXFastCallDisplayList(const_cast<u8*>(rTag.GetDL()), rTag.GetCmdSize());
    }
}

void ResShpPrePrim::DCStore(bool sync) {
    ResPrePrimDL& r = ref();
    u32 size = sizeof(ResPrePrimDL);

    if (sync) {
        DC::StoreRange(&r, size);
    } else {
        DC::StoreRangeNoSync(&r, size);
    }
}

void ResShp::DCStore(bool sync) {
    ResShpData& r = ref();
    u32 size = r.size;

    if (sync) {
        DC::StoreRange(&r, size);
    } else {
        DC::StoreRangeNoSync(&r, size);
    }
}

}
}
