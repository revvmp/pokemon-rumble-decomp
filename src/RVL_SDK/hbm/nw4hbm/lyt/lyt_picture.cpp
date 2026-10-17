#include "nw4hbm/lyt/lyt_picture.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"

inline void* operator new(size_t, void* p) { return p; }

namespace nw4hbm {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(Picture, Pane);

Picture::Picture(const res::Picture* pRes, const ResBlockSet& rBlockSet) : Pane(pRes) {
    u8 texCoordNum = ut::Min<u8>(pRes->texCoordNum, 8);

    Init(texCoordNum);

    for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
        mVtxColors[i] = pRes->vtxCols[i];
    }

    if (texCoordNum && !mTexCoordAry.IsEmpty()) {
        mTexCoordAry.Copy(&pRes[1], texCoordNum);
    }

    if (void* pMem = Layout::AllocMemory(sizeof(Material))) {
        const u32* pMatOffsTbl = detail::ConvertOffsToPtr<u32>(rBlockSet.pMaterialList, sizeof(res::MaterialList));
        const res::Material* pResMaterial =
            detail::ConvertOffsToPtr<res::Material>(rBlockSet.pMaterialList, pMatOffsTbl[pRes->materialIdx]);

        mpMaterial = new (pMem) Material(pResMaterial, rBlockSet);
    }
}

void Picture::Init(u8 num) {
    if (num) {
        ReserveTexCoord(num);
    }
}

Picture::~Picture() {
    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        mpMaterial->~Material();
        Layout::FreeMemory(mpMaterial);
        mpMaterial = nullptr;
    }

    mTexCoordAry.Free();
}

void Picture::Append(TPLPalette* pTplRes) {
    GXTexObj texObj;
    detail::InitGXTexObjFromTPL(&texObj, pTplRes, 0);
    Append(texObj);
}

void Picture::Append(const GXTexObj& rTexObj) {
    if (mpMaterial->GetTextureNum() >= mpMaterial->GetTextureCap() ||
        mpMaterial->GetTextureNum() >= mpMaterial->GetTexCoordGenCap()) {
        return;
    }

    u8 texIdx = mpMaterial->GetTextureNum();
    mpMaterial->SetTextureNum(texIdx + 1);
    mpMaterial->SetTexture(texIdx, rTexObj);
    mpMaterial->SetTexCoordGenNum(mpMaterial->GetTextureNum());
    mpMaterial->SetTexCoordGen(texIdx, TexCoordGen());

    SetTexCoordNum(mpMaterial->GetTextureNum());

    if (mSize == Size(0.0f, 0.0f) && mpMaterial->GetTextureNum() == 1) {
        mSize = detail::GetTextureSize(mpMaterial, 0);
    }
}

void Picture::ReserveTexCoord(u8 num) {
    mTexCoordAry.Reserve(num);
}

void Picture::SetTexCoordNum(u8 num) {
    mTexCoordAry.SetSize(num);
}

ut::Color Picture::GetVtxColor(u32 idx) const {
    return mVtxColors[idx];
}

void Picture::SetVtxColor(u32 idx, ut::Color color) {
    mVtxColors[idx] = color;
}

u8 Picture::GetVtxColorElement(u32 idx) const {
    return detail::GetVtxColorElement(mVtxColors, idx);
}

void Picture::SetVtxColorElement(u32 idx, u8 value) {
    detail::SetVtxColorElement(mVtxColors, idx, value);
}

void Picture::DrawSelf(const DrawInfo& rInfo) {
    if (!mpMaterial) {
        return;
    }

    LoadMtx(rInfo);

    bool useVtxColor = mpMaterial->SetupGX(detail::IsModulateVertexColor(mVtxColors, mGlbAlpha), mGlbAlpha);
    detail::SetVertexFormat(useVtxColor, mTexCoordAry.GetSize());
    detail::DrawQuad(GetVtxPos(), mSize, mTexCoordAry.GetSize(), mTexCoordAry.GetArray(),
                     useVtxColor ? mVtxColors : nullptr, mGlbAlpha);
}

}
}
