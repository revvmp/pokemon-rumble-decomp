#include "nw4hbm/lyt/lyt_window.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"

inline void* operator new(size_t, void* p) { return p; }

namespace {

using namespace nw4hbm;
using namespace nw4hbm::lyt;

struct TextureFlipInfo {
    u8 coords[VERTEXCOLOR_MAX][2];
    u8 idx[2];
};

TextureFlipInfo& GetTexutreFlipInfo(u8 textureFlip) {
    static TextureFlipInfo flipInfos[] = {
        {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {0, 1}},
        {{{1, 0}, {0, 0}, {1, 1}, {0, 1}}, {0, 1}},
        {{{0, 1}, {1, 1}, {0, 0}, {1, 0}}, {0, 1}},
        {{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, {1, 0}},
        {{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, {0, 1}},
        {{{1, 0}, {1, 1}, {0, 0}, {0, 1}}, {1, 0}},
    };

    return flipInfos[textureFlip];
}

void GetLTFrameSize(math::VEC2* pPt, Size* pSize, const math::VEC2& rBase, const Size& rWinSize,
                    const WindowFrameSize& rFrameSize) {
    *pPt = rBase;

    pSize->width = rWinSize.width - rFrameSize.r;
    pSize->height = rFrameSize.t;
}

void GetLTTexCoord(math::VEC2* pTexCoords, const Size& rPolSize, const Size& rTexSize, u8 textureFlip) {
    TextureFlipInfo& rFlipInfo = GetTexutreFlipInfo(textureFlip);
    int ix = rFlipInfo.idx[0];
    int iy = rFlipInfo.idx[1];
    const math::VEC2 texSize(rTexSize.width, rTexSize.height);

    pTexCoords[0][ix] = pTexCoords[2][ix] = rFlipInfo.coords[0][ix];
    pTexCoords[0][iy] = pTexCoords[1][iy] = rFlipInfo.coords[0][iy];

    pTexCoords[3][ix] = pTexCoords[1][ix] =
        rFlipInfo.coords[0][ix] +
        rPolSize.width / (texSize[ix] * (rFlipInfo.coords[1][ix] - rFlipInfo.coords[0][ix]));

    pTexCoords[3][iy] = pTexCoords[2][iy] =
        rFlipInfo.coords[0][iy] +
        rPolSize.height / (texSize[iy] * (rFlipInfo.coords[2][iy] - rFlipInfo.coords[0][iy]));
}

void GetRTFrameSize(math::VEC2* pPt, Size* pSize, const math::VEC2& rBase, const Size& rWinSize,
                    const WindowFrameSize& rFrameSize) {
    *pPt = math::VEC2(rBase.x + rWinSize.width - rFrameSize.r, rBase.y);

    pSize->width = rFrameSize.r;
    pSize->height = rWinSize.height - rFrameSize.b;
}

void GetRTTexCoord(math::VEC2* pTexCoords, const Size& rPolSize, const Size& rTexSize, u8 textureFlip) {
    TextureFlipInfo& rFlipInfo = GetTexutreFlipInfo(textureFlip);
    int ix = rFlipInfo.idx[0];
    int iy = rFlipInfo.idx[1];
    const math::VEC2 texSize(rTexSize.width, rTexSize.height);

    pTexCoords[1][ix] = pTexCoords[3][ix] = rFlipInfo.coords[1][ix];
    pTexCoords[1][iy] = pTexCoords[0][iy] = rFlipInfo.coords[1][iy];

    pTexCoords[2][ix] = pTexCoords[0][ix] =
        rFlipInfo.coords[1][ix] +
        rPolSize.width / (texSize[ix] * (rFlipInfo.coords[0][ix] - rFlipInfo.coords[1][ix]));

    pTexCoords[2][iy] = pTexCoords[3][iy] =
        rFlipInfo.coords[1][iy] +
        rPolSize.height / (texSize[iy] * (rFlipInfo.coords[3][iy] - rFlipInfo.coords[1][iy]));
}

void GetLBFrameSize(math::VEC2* pPt, Size* pSize, const math::VEC2& rBase, const Size& rWinSize,
                    const WindowFrameSize& rFrameSize) {
    *pPt = math::VEC2(rBase.x, rBase.y + rFrameSize.t);

    pSize->width = rFrameSize.l;
    pSize->height = rWinSize.height - rFrameSize.t;
}

void GetLBTexCoord(math::VEC2* pTexCoords, const Size& rPolSize, const Size& rTexSize, u8 textureFlip) {
    TextureFlipInfo& rFlipInfo = GetTexutreFlipInfo(textureFlip);
    int ix = rFlipInfo.idx[0];
    int iy = rFlipInfo.idx[1];
    const math::VEC2 texSize(rTexSize.width, rTexSize.height);

    pTexCoords[2][ix] = pTexCoords[0][ix] = rFlipInfo.coords[2][ix];
    pTexCoords[2][iy] = pTexCoords[3][iy] = rFlipInfo.coords[2][iy];

    pTexCoords[1][ix] = pTexCoords[3][ix] =
        rFlipInfo.coords[2][ix] +
        rPolSize.width / (texSize[ix] * (rFlipInfo.coords[3][ix] - rFlipInfo.coords[2][ix]));

    pTexCoords[1][iy] = pTexCoords[0][iy] =
        rFlipInfo.coords[2][iy] +
        rPolSize.height / (texSize[iy] * (rFlipInfo.coords[0][iy] - rFlipInfo.coords[2][iy]));
}

void GetRBFrameSize(math::VEC2* pPt, Size* pSize, const math::VEC2& rBase, const Size& rWinSize,
                    const WindowFrameSize& rFrameSize) {
    *pPt = math::VEC2(rBase.x + rFrameSize.l, rBase.y + rWinSize.height - rFrameSize.b);

    pSize->width = rWinSize.width - rFrameSize.l;
    pSize->height = rFrameSize.b;
}

void GetRBTexCoord(math::VEC2* pTexCoords, const Size& rPolSize, const Size& rTexSize, u8 textureFlip) {
    TextureFlipInfo& rFlipInfo = GetTexutreFlipInfo(textureFlip);
    int ix = rFlipInfo.idx[0];
    int iy = rFlipInfo.idx[1];
    const math::VEC2 texSize(rTexSize.width, rTexSize.height);

    pTexCoords[3][ix] = pTexCoords[1][ix] = rFlipInfo.coords[3][ix];
    pTexCoords[3][iy] = pTexCoords[2][iy] = rFlipInfo.coords[3][iy];

    pTexCoords[0][ix] = pTexCoords[2][ix] =
        rFlipInfo.coords[3][ix] +
        rPolSize.width / (texSize[ix] * (rFlipInfo.coords[2][ix] - rFlipInfo.coords[3][ix]));

    pTexCoords[0][iy] = pTexCoords[1][iy] =
        rFlipInfo.coords[3][iy] +
        rPolSize.height / (texSize[iy] * (rFlipInfo.coords[1][iy] - rFlipInfo.coords[3][iy]));
}

}

namespace nw4hbm {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(Window, Pane);

Window::Window(const res::Window* pRes, const ResBlockSet& rBlockSet) : Pane(pRes) {
    mContentInflation = pRes->inflation;

    const u32* const pMatOffsTbl = detail::ConvertOffsToPtr<u32>(rBlockSet.pMaterialList, sizeof(res::MaterialList));

    {
        const res::WindowContent* pResContent =
            detail::ConvertOffsToPtr<res::WindowContent>(pRes, pRes->contentOffset);

        for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
            mContent.vtxColors[i] = pResContent->vtxCols[i];
        }

        if (pResContent->texCoordNum) {
            u8 texCoordNum = ut::Min<u8>(pResContent->texCoordNum, 8);
            mContent.texCoordAry.Reserve(texCoordNum);

            if (!mContent.texCoordAry.IsEmpty()) {
                mContent.texCoordAry.Copy(&pResContent[1], texCoordNum);
            }
        }

        if (void* pMem = Layout::AllocMemory(sizeof(Material))) {
            const res::Material* pResMaterial =
                detail::ConvertOffsToPtr<res::Material>(rBlockSet.pMaterialList, pMatOffsTbl[pResContent->materialIdx]);

            mpMaterial = new (pMem) Material(pResMaterial, rBlockSet);
        }
    }

    mFrameNum = 0;
    mFrames = nullptr;

    if (pRes->frameNum) {
        if ((mFrames = static_cast<Frame*>(Layout::AllocMemory(sizeof(Frame) * pRes->frameNum)))) {
            mFrameNum = pRes->frameNum;
            const u32* pFrameOffsTbl = detail::ConvertOffsToPtr<u32>(pRes, pRes->frameOffsetTableOffset);

            for (int i = 0; i < mFrameNum; i++) {
                const res::WindowFrame* pResFrame = detail::ConvertOffsToPtr<res::WindowFrame>(pRes, pFrameOffsTbl[i]);

                mFrames[i].textureFlip = pResFrame->textureFlip;
                mFrames[i].pMaterial = nullptr;

                if (void* pMem = Layout::AllocMemory(sizeof(Material))) {
                    const res::Material* pResMaterial = detail::ConvertOffsToPtr<res::Material>(
                        rBlockSet.pMaterialList, pMatOffsTbl[pResFrame->materialIdx]);

                    mFrames[i].pMaterial = new (pMem) Material(pResMaterial, rBlockSet);
                }
            }
        }
    }
}

Window::~Window() {
    if (mFrames) {
        for (int i = 0; i < mFrameNum; i++) {
            mFrames[i].pMaterial->~Material();
            Layout::FreeMemory(mFrames[i].pMaterial);
        }

        Layout::FreeMemory(mFrames);
    }

    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        mpMaterial->~Material();
        Layout::FreeMemory(mpMaterial);
        mpMaterial = nullptr;
    }

    mContent.texCoordAry.Free();
}

Material* Window::FindMaterialByName(const char* pName, bool recursive) {
    if (mpMaterial && detail::EqualsMaterialName(mpMaterial->GetName(), pName)) {
        return mpMaterial;
    }

    for (int i = 0; i < mFrameNum; i++) {
        if (detail::EqualsMaterialName(mFrames[i].pMaterial->GetName(), pName)) {
            return mFrames[i].pMaterial;
        }
    }

    if (recursive) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            if (Material* pMaterial = it->FindMaterialByName(pName, true)) {
                return pMaterial;
            }
        }
    }

    return nullptr;
}

AnimationLink* Window::FindAnimationLink(AnimTransform* pAnimTrans) {
    if (AnimationLink* pAnimLink = Pane::FindAnimationLink(pAnimTrans)) {
        return pAnimLink;
    }

    for (int i = 0; i < mFrameNum; i++) {
        if (AnimationLink* pAnimLink = mFrames[i].pMaterial->FindAnimationLink(pAnimTrans)) {
            return pAnimLink;
        }
    }

    return nullptr;
}

void Window::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable, bool recursive) {
    for (int i = 0; i < mFrameNum; i++) {
        mFrames[i].pMaterial->SetAnimationEnable(pAnimTrans, enable);
    }

    Pane::SetAnimationEnable(pAnimTrans, enable, recursive);
}

ut::Color Window::GetVtxColor(u32 idx) const {
    return mContent.vtxColors[idx];
}

void Window::SetVtxColor(u32 idx, ut::Color color) {
    mContent.vtxColors[idx] = color;
}

u8 Window::GetVtxColorElement(u32 idx) const {
    return detail::GetVtxColorElement(mContent.vtxColors, idx);
}

void Window::SetVtxColorElement(u32 idx, u8 value) {
    detail::SetVtxColorElement(mContent.vtxColors, idx, value);
}

void Window::DrawSelf(const DrawInfo& rInfo) {
    LoadMtx(rInfo);

    WindowFrameSize frameSize = GetFrameSize(mFrameNum, mFrames);
    math::VEC2 basePt = GetVtxPos();

    DrawContent(basePt, frameSize, mGlbAlpha);

    switch (mFrameNum) {
    case 1:
        DrawFrame(basePt, *mFrames, frameSize, mGlbAlpha);
        break;
    case 4:
        DrawFrame4(basePt, mFrames, frameSize, mGlbAlpha);
        break;
    case 8:
        DrawFrame8(basePt, mFrames, frameSize, mGlbAlpha);
        break;
    }
}

void Window::AnimateSelf(u32 option) {
    Pane::AnimateSelf(option);

    if (IsVisible() || !(option & ANIMOPTION_SKIP_INVISIBLE)) {
        for (int i = 0; i < mFrameNum; i++) {
            mFrames[i].pMaterial->Animate();
        }
    }
}

void Window::UnbindAnimationSelf(AnimTransform* pAnimTrans) {
    for (int i = 0; i < mFrameNum; i++) {
        mFrames[i].pMaterial->UnbindAnimation(pAnimTrans);
    }

    Pane::UnbindAnimationSelf(pAnimTrans);
}

void Window::DrawContent(const math::VEC2& rBase, const WindowFrameSize& rFrameSize, u8 alpha) {
    bool useVtxColor = mpMaterial->SetupGX(detail::IsModulateVertexColor(mContent.vtxColors, alpha), alpha);

    detail::SetVertexFormat(useVtxColor, mContent.texCoordAry.GetSize());

    detail::DrawQuad(math::VEC2(rBase.x + rFrameSize.l - mContentInflation.l,
                                rBase.y + rFrameSize.t - mContentInflation.t),
                     Size(mSize.width - rFrameSize.l + mContentInflation.l - rFrameSize.r + mContentInflation.r,
                          mSize.height - rFrameSize.t + mContentInflation.t - rFrameSize.b + mContentInflation.b),
                     mContent.texCoordAry.GetSize(), mContent.texCoordAry.GetArray(),
                     useVtxColor ? mContent.vtxColors : nullptr, alpha);
}

void Window::DrawFrame(const math::VEC2& rBase, const Frame& rFrame, const WindowFrameSize& rFrameSize, u8 alpha) {
    bool useVtxColor = rFrame.pMaterial->SetupGX(detail::IsModulateVertexColor(nullptr, alpha), alpha);
    detail::SetVertexFormat(useVtxColor, 1);

    const Size& texSize = detail::GetTextureSize(rFrame.pMaterial, 0);
    ut::Color vtxColors[VERTEXCOLOR_MAX];
    detail::TexCoord texCoords[1];
    math::VEC2 polPt;
    Size polSize;

    GetLTFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
    GetLTTexCoord(*texCoords, polSize, texSize, 0);
    detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);

    GetRTFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
    GetRTTexCoord(*texCoords, polSize, texSize, 1);
    detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);

    GetRBFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
    GetRBTexCoord(*texCoords, polSize, texSize, 4);
    detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);

    GetLBFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
    GetLBTexCoord(*texCoords, polSize, texSize, 2);
    detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
}

void Window::DrawFrame4(const math::VEC2& rBase, const Frame* pFrames, const WindowFrameSize& rFrameSize,
                        u8 alpha) {
    ut::Color vtxColors[VERTEXCOLOR_MAX];
    detail::TexCoord texCoords[1];
    math::VEC2 polPt;
    Size polSize;
    bool modulate = detail::IsModulateVertexColor(nullptr, alpha);

    {
        bool useVtxColor = pFrames[WINDOWFRAME_LT].pMaterial->SetupGX(modulate, alpha);
        GetLTFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
        GetLTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_LT].pMaterial, 0),
                      pFrames[WINDOWFRAME_LT].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_RT].pMaterial->SetupGX(modulate, alpha);
        GetRTFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
        GetRTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_RT].pMaterial, 0),
                      pFrames[WINDOWFRAME_RT].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_RB].pMaterial->SetupGX(modulate, alpha);
        GetRBFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
        GetRBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_RB].pMaterial, 0),
                      pFrames[WINDOWFRAME_RB].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_LB].pMaterial->SetupGX(modulate, alpha);
        GetLBFrameSize(&polPt, &polSize, rBase, mSize, rFrameSize);
        GetLBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_LB].pMaterial, 0),
                      pFrames[WINDOWFRAME_LB].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(polPt, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }
}

void Window::DrawFrame8(const math::VEC2& rBase, const Frame* pFrames, const WindowFrameSize& rFrameSize,
                        u8 alpha) {
    ut::Color vtxColors[VERTEXCOLOR_MAX];
    detail::TexCoord texCoords[1];
    Size polSize;
    bool modulate = detail::IsModulateVertexColor(nullptr, alpha);

    {
        bool useVtxColor = pFrames[WINDOWFRAME_LT].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.l, rFrameSize.t);
        GetLTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_LT].pMaterial, 0),
                      pFrames[WINDOWFRAME_LT].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(rBase, polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_T].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(mSize.width - rFrameSize.l - rFrameSize.r, rFrameSize.t);
        GetLTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_T].pMaterial, 0),
                      pFrames[WINDOWFRAME_T].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x + rFrameSize.l, rBase.y), polSize, 1, texCoords,
                         useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_RT].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.r, rFrameSize.t);
        GetRTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_RT].pMaterial, 0),
                      pFrames[WINDOWFRAME_RT].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x + mSize.width - rFrameSize.r, rBase.y), polSize, 1, texCoords,
                         useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_R].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.r, mSize.height - rFrameSize.t - rFrameSize.b);
        GetRTTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_R].pMaterial, 0),
                      pFrames[WINDOWFRAME_R].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x + mSize.width - rFrameSize.r, rBase.y + rFrameSize.t), polSize, 1,
                         texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_RB].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.r, rFrameSize.b);
        GetRBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_RB].pMaterial, 0),
                      pFrames[WINDOWFRAME_RB].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x + mSize.width - rFrameSize.r, rBase.y + mSize.height - rFrameSize.b),
                         polSize, 1, texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_B].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(mSize.width - rFrameSize.l - rFrameSize.r, rFrameSize.b);
        GetRBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_B].pMaterial, 0),
                      pFrames[WINDOWFRAME_B].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x + rFrameSize.l, rBase.y + mSize.height - rFrameSize.b), polSize, 1,
                         texCoords, useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_LB].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.l, rFrameSize.b);
        GetLBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_LB].pMaterial, 0),
                      pFrames[WINDOWFRAME_LB].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x, rBase.y + mSize.height - rFrameSize.b), polSize, 1, texCoords,
                         useVtxColor ? vtxColors : nullptr, alpha);
    }

    {
        bool useVtxColor = pFrames[WINDOWFRAME_L].pMaterial->SetupGX(modulate, alpha);
        polSize = Size(rFrameSize.l, mSize.height - rFrameSize.t - rFrameSize.b);
        GetLBTexCoord(*texCoords, polSize, detail::GetTextureSize(pFrames[WINDOWFRAME_L].pMaterial, 0),
                      pFrames[WINDOWFRAME_L].textureFlip);
        detail::SetVertexFormat(useVtxColor, 1);
        detail::DrawQuad(math::VEC2(rBase.x, rBase.y + rFrameSize.t), polSize, 1, texCoords,
                         useVtxColor ? vtxColors : nullptr, alpha);
    }
}

WindowFrameSize Window::GetFrameSize(u8 frameNum, const Frame* pFrames) {
    WindowFrameSize frameSize = {};

    switch (frameNum) {
    case 1: {
        Size texSize = detail::GetTextureSize(pFrames->pMaterial, 0);
        frameSize.l = texSize.width;
        frameSize.t = texSize.height;
        frameSize.r = texSize.width;
        frameSize.b = texSize.height;
        break;
    }
    case 4:
    case 8: {
        Size texSize = detail::GetTextureSize(pFrames[WINDOWFRAME_LT].pMaterial, 0);
        frameSize.l = texSize.width;
        frameSize.t = texSize.height;

        texSize = detail::GetTextureSize(pFrames[WINDOWFRAME_RB].pMaterial, 0);
        frameSize.r = texSize.width;
        frameSize.b = texSize.height;
        break;
    }
    }

    return frameSize;
}

Material* Window::GetFrameMaterial(u32 idx) const {
    if (idx >= mFrameNum) {
        return nullptr;
    }

    return mFrames[idx].pMaterial;
}

Material* Window::GetContentMaterial() const {
    return GetMaterial();
}

}
}
