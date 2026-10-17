#include <nw4r/lyt/lyt_picture.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_texMap.h>
namespace nw4r {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(Picture, Pane);



Picture::Picture(const res::Picture *pResPic, const ResBlockSet &resBlockSet)
    : Pane(pResPic), mVtxColors(), mTexCoordAry() {
    u8 texCoordNum = ut::Min<u8>(pResPic->texCoordNum, 8);

    Init(texCoordNum);

    for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
        mVtxColors[i] = pResPic->vtxCols[i];
    }
    if (texCoordNum != 0 && !mTexCoordAry.IsEmpty()) {
        mTexCoordAry.Copy((void *)&pResPic[1], texCoordNum);
    }
    const u32 *matOffsTbl = detail::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(res::MaterialList));
    const res::Material *pResMaterial =
        detail::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pResPic->materialIdx]);
    mpMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
}

void Picture::Init(u8 texNum) {
    ReserveTexCoord(texNum);
}

Picture::~Picture() {
    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
        mpMaterial = nullptr;
    }
    mTexCoordAry.Free();
}

void Picture::Append(const TexMap &texMap) {
    if (mpMaterial->GetTextureNum() >= mpMaterial->GetTextureCap() ||
        mpMaterial->GetTextureNum() >= mpMaterial->GetTexCoordGenCap()) {
        return;
    }
    u8 texIdx = mpMaterial->GetTextureNum();
    mpMaterial->SetTextureNum(texIdx + 1);
    mpMaterial->SetTexture(texIdx, texMap);
    mpMaterial->SetTexCoordGenNum(mpMaterial->GetTextureNum());
    mpMaterial->SetTexCoordGen(texIdx, TexCoordGen());
    SetTexCoordNum(mpMaterial->GetTextureNum());
    if (mSize == Size(0.0f, 0.0f) && mpMaterial->GetTextureNum() == 1) {
        mSize = detail::GetTextureSize(mpMaterial, 0);
    }
}

void Picture::ReserveTexCoord(u8 texNum) {
    if (texNum != 0) {
        mTexCoordAry.Reserve(texNum);
    }
}

void Picture::SetTexCoordNum(u8 num) {
    mTexCoordAry.SetSize(num);
}

ut::Color Picture::GetVtxColor(u32 idx) const {
    return mVtxColors[idx];
}

void Picture::SetVtxColor(u32 idx, ut::Color value) {
    mVtxColors[idx] = value;
}

u8 Picture::GetVtxColorElement(u32 idx) const {
    return detail::GetVtxColorElement(mVtxColors, idx);
}

void Picture::SetVtxColorElement(u32 idx, u8 value) {
    detail::SetVtxColorElement(mVtxColors, idx, value);
}

void Picture::DrawSelf(const DrawInfo &drawInfo) {
    if (mpMaterial) {
        LoadMtx(drawInfo);
        bool bUseVtxCol = mpMaterial->SetupGX(detail::IsModulateVertexColor(mVtxColors, mGlbAlpha), mGlbAlpha);
        detail::SetVertexFormat(bUseVtxCol, mTexCoordAry.GetSize());
        detail::DrawQuad(
            GetVtxPos(), mSize, mTexCoordAry.GetSize(), mTexCoordAry.GetArray(), bUseVtxCol ? mVtxColors : nullptr,
            mGlbAlpha
        );
    }
}

}

}
