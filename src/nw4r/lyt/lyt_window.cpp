#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_window.h>

#include "nw4r/lyt/lyt_layout.h"
#include "nw4r/lyt/lyt_types.h"

namespace nw4r {
namespace lyt {

struct TextureFlipInfo {
    u8 coords[4][2];
    u8 idx[2];
};

namespace {

TextureFlipInfo &GetTextureFlipInfo(u8 textureFlip) {
    static TextureFlipInfo flipInfos[6] = {

        {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {0, 1}},
        {{{1, 0}, {0, 0}, {1, 1}, {0, 1}}, {0, 1}},
        {{{0, 1}, {1, 1}, {0, 0}, {1, 0}}, {0, 1}},
        {{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, {1, 0}},
        {{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, {0, 1}},
        {{{1, 0}, {1, 1}, {0, 0}, {0, 1}}, {1, 0}},
    };
    return flipInfos[textureFlip];
}

void GetLTFrameSize(
    math::VEC2 *pPt, Size *pSize, const math::VEC2 &basePt, const Size &windSize, const WindowFrameSize &frameSize
) {
    *pPt = basePt;

    pSize->width = windSize.width - frameSize.r;
    pSize->height = frameSize.t;

}

void GetLTTexCoord(math::VEC2 *texCds, const Size &polSize, const Size &texSize, u8 textureFlip) {
    const TextureFlipInfo &flipInfo = GetTextureFlipInfo(textureFlip);

    int ix = flipInfo.idx[0];
    int iy = flipInfo.idx[1];
    math::VEC2 tSz(texSize.width, texSize.height);

    texCds[TopL][ix] = texCds[BotL][ix] = flipInfo.coords[TopL][ix];
    texCds[TopL][iy] = texCds[TopR][iy] = flipInfo.coords[TopL][iy];

    texCds[BotR][ix] = texCds[TopR][ix] = flipInfo.coords[TopL][ix] +  polSize.width / ((flipInfo.coords[TopR][ix] - flipInfo.coords[TopL][ix]) * tSz[ix]);
    texCds[BotR][iy] = texCds[BotL][iy] = flipInfo.coords[TopL][iy] + polSize.height / ((flipInfo.coords[BotL][iy] - flipInfo.coords[TopL][iy]) * tSz[iy]);

}

void GetRTFrameSize(
    math::VEC2 *pPt, Size *pSize, const math::VEC2 &basePt, const Size &windSize, const WindowFrameSize &frameSize
) {
    *pPt = math::VEC2(basePt.x + windSize.width - frameSize.r, basePt.y);

    pSize->width = frameSize.r;
    pSize->height = windSize.height - frameSize.b;
}

void GetRTTexCoord(math::VEC2 *texCds, const Size &polSize, const Size &texSize, u8 textureFlip) {
    const TextureFlipInfo &flipInfo = GetTextureFlipInfo(textureFlip);
    int ix = flipInfo.idx[0];
    int iy = flipInfo.idx[1];

    math::VEC2 tSz(texSize.width, texSize.height);

    texCds[TopR][ix] = texCds[BotR][ix] = flipInfo.coords[TopR][ix];
    texCds[TopR][iy] = texCds[TopL][iy] = flipInfo.coords[TopR][iy];

    texCds[BotL][ix] = texCds[TopL][ix] = flipInfo.coords[TopR][ix] +  polSize.width / ((flipInfo.coords[TopL][ix] - flipInfo.coords[TopR][ix]) * tSz[ix]);
    texCds[BotL][iy] = texCds[BotR][iy] = flipInfo.coords[TopR][iy] + polSize.height / ((flipInfo.coords[BotR][iy] - flipInfo.coords[TopR][iy]) * tSz[iy]);

}

void GetLBFrameSize(
    math::VEC2 *pPt, Size *pSize, const math::VEC2 &basePt, const Size &windSize, const WindowFrameSize &frameSize
) {
    *pPt = math::VEC2(basePt.x, basePt.y - frameSize.t);

    pSize->width = frameSize.l;
    pSize->height = windSize.height - frameSize.t;
}

void GetLBTexCoord(math::VEC2 *texCds, const Size &polSize, const Size &texSize, u8 textureFlip) {
    const TextureFlipInfo &flipInfo = GetTextureFlipInfo(textureFlip);
    int ix = flipInfo.idx[0];
    int iy = flipInfo.idx[1];

    math::VEC2 tSz(texSize.width, texSize.height);

    texCds[BotL][ix] = texCds[TopL][ix] = flipInfo.coords[BotL][ix];
    texCds[BotL][iy] = texCds[BotR][iy] = flipInfo.coords[BotL][iy];

    texCds[TopR][ix] = texCds[BotR][ix] = flipInfo.coords[BotL][ix] +  polSize.width /  ((flipInfo.coords[BotR][ix] - flipInfo.coords[BotL][ix]) * tSz[ix]);
    texCds[TopR][iy] = texCds[TopL][iy] = flipInfo.coords[BotL][iy] + polSize.height /  ((flipInfo.coords[TopL][iy] - flipInfo.coords[BotL][iy]) * tSz[iy]);

}

void GetRBFrameSize(
    math::VEC2 *pPt, Size *pSize, const math::VEC2 &basePt, const Size &windSize, const WindowFrameSize &frameSize
) {
    *pPt = math::VEC2(basePt.x + frameSize.l, basePt.y - windSize.height + frameSize.b);

    pSize->width = windSize.width - frameSize.l;
    pSize->height = frameSize.b;
}

void GetRBTexCoord(math::VEC2 *texCds, const Size &polSize, const Size &texSize, u8 textureFlip) {
    const TextureFlipInfo &flipInfo = GetTextureFlipInfo(textureFlip);
    int ix = flipInfo.idx[0];
    int iy = flipInfo.idx[1];
    math::VEC2 tSz(texSize.width, texSize.height);

    texCds[BotR][ix] = texCds[TopR][ix] = flipInfo.coords[BotR][ix];
    texCds[BotR][iy] = texCds[BotL][iy] = flipInfo.coords[BotR][iy];

    texCds[TopL][ix] = texCds[BotL][ix] = flipInfo.coords[BotR][ix] +  polSize.width / ((flipInfo.coords[BotL][ix] - flipInfo.coords[BotR][ix]) * tSz[ix]);
    texCds[TopL][iy] = texCds[TopR][iy] = flipInfo.coords[BotR][iy] + polSize.height / ((flipInfo.coords[TopR][iy] - flipInfo.coords[BotR][iy]) * tSz[iy]);

}
}

NW4R_UT_RTTI_DEF_DERIVED(Window, Pane);

Window::Window(const res::Window *pBlock, const ResBlockSet &resBlockSet) : Pane(pBlock), mContent() {
    const res::WindowContent *pResContent =
        detail::ConvertOffsToPtr<const res::WindowContent>(pBlock, pBlock->contentOffset);

    u8 texCoordNum = ut::Min<u8>(pResContent->texCoordNum, 8);

    InitContent(texCoordNum);

    const u32 *matOffsTbl = detail::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(res::MaterialList));

    mContentInflation = pBlock->inflation;
    for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
        mContent.vtxColors[i] = pResContent->vtxCols[i];
    }

    if (texCoordNum && !mContent.texCoordAry.IsEmpty()) {
        mContent.texCoordAry.Copy(&pResContent[1], texCoordNum);
    }

    const res::Material *pResMaterial =
        detail::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pResContent->materialIdx]);
    mpMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);

    mFrameNum = 0;
    mFrames = nullptr;
    if (pBlock->frameNum) {
        InitFrame(pBlock->frameNum);
        const u32 *frameOffsetTable = detail::ConvertOffsToPtr<u32>(pBlock, pBlock->frameOffsetTableOffset);
        for (int i = 0; i < mFrameNum; i++) {
            const res::WindowFrame *pResWindowFrame =
                detail::ConvertOffsToPtr<res::WindowFrame>(pBlock, frameOffsetTable[i]);
            mFrames[i].textureFlip = pResWindowFrame->textureFlip;
            const res::Material *pResMaterial = detail::ConvertOffsToPtr<res::Material>(
                resBlockSet.pMaterialList, matOffsTbl[pResWindowFrame->materialIdx]
            );
            mFrames[i].pMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
        }
    }
}

void Window::InitContent(u8 texNum) {

    if (texNum) {
        ReserveTexCoord(texNum);
    }
}

void Window::InitFrame(u8 frameNum) {
    mFrameNum = 0;
    mFrames = Layout::NewArray<Frame>(frameNum);
    if (mFrames) {
        mFrameNum = frameNum;
    }
}

Window::~Window() {
    Layout::DeleteArray(mFrames, mFrameNum);
    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
        mpMaterial = nullptr;
    }
    mContent.texCoordAry.Free();
}

void Window::ReserveTexCoord(u8 num) {

    mContent.texCoordAry.Reserve(num);
}

Material *Window::FindMaterialByName(const char *findName, bool bRecursive) {
    if (mpMaterial && detail::EqualsMaterialName(mpMaterial->GetName(), findName)) {
        return mpMaterial;
    }

    for (int i = 0; i < mFrameNum; i++) {
        if (detail::EqualsMaterialName(mFrames[i].pMaterial->GetName(), findName)) {
            return mFrames[i].pMaterial;
        }
    }

    if (bRecursive) {
        for (ut::LinkList<Pane, 4>::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); it++) {
            Material *pMat = it->FindMaterialByName(findName, bRecursive);
            if (pMat) {
                return pMat;
            }
        }
    }
    return nullptr;
}

ut::Color Window::GetVtxColor(u32 idx) const {
    return mContent.vtxColors[idx];
}

void Window::SetVtxColor(u32 idx, ut::Color value) {
    mContent.vtxColors[idx] = value;
}

u8 Window::GetVtxColorElement(u32 idx) const {
    return detail::GetVtxColorElement(mContent.vtxColors, idx);
}

void Window::SetVtxColorElement(u32 idx, u8 value) {
    detail::SetVtxColorElement(mContent.vtxColors, idx, value);
}

void Window::DrawSelf(const DrawInfo &drawInfo) {
    LoadMtx(drawInfo);
    WindowFrameSize frameSize = GetFrameSize(mFrameNum, mFrames);
    math::VEC2 basePt = GetVtxPos();
    DrawContent(basePt, frameSize, mGlbAlpha);
    switch (mFrameNum) {
        case 1: DrawFrame(basePt, mFrames[0], frameSize, mGlbAlpha); break;
        case 4: DrawFrame4(basePt, mFrames, frameSize, mGlbAlpha); break;
        case 8: DrawFrame8(basePt, mFrames, frameSize, mGlbAlpha); break;
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

void Window::UnbindAnimationSelf(AnimTransform *pAnimTrans) {
    for (int i = 0; i < mFrameNum; i++) {
        mFrames[i].pMaterial->UnbindAnimation(pAnimTrans);
    }
    Pane::UnbindAnimationSelf(pAnimTrans);
}

void Window::DrawContent(const math::VEC2 &basePt, const WindowFrameSize &frameSize, u8 alpha) {
    bool bUseVtxCol = detail::IsModulateVertexColor(mContent.vtxColors, alpha);
    bUseVtxCol = mpMaterial->SetupGX(bUseVtxCol, alpha);
    detail::SetVertexFormat(bUseVtxCol, mContent.texCoordAry.GetSize());

    detail::DrawQuad(
        math::VEC2(basePt.x + frameSize.l - mContentInflation.l, basePt.y - frameSize.t + mContentInflation.t),
        Size(
            mSize.width - frameSize.l + mContentInflation.l - frameSize.r + mContentInflation.r,
            mSize.height - frameSize.t + mContentInflation.t - frameSize.b + mContentInflation.b
        ),
        mContent.texCoordAry.GetSize(), mContent.texCoordAry.GetArray(), bUseVtxCol ? mContent.vtxColors : nullptr,
        alpha
    );
}

void Window::DrawFrame(const math::VEC2 &basePt, const Frame &frame, const WindowFrameSize &frameSize, u8 alpha) {
    if (frame.pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = detail::IsModulateVertexColor(nullptr, alpha);
        bUseVtxCol = frame.pMaterial->SetupGX(bUseVtxCol, alpha);

        detail::SetVertexFormat(bUseVtxCol, 1);
        Size texSize = detail::GetTextureSize(frame.pMaterial, 0);

        ut::Color vtxColors[4];
        detail::TexCoord texCds[1];
        math::VEC2 polPt;
        Size polSize;

        GetLTFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetLTTexCoord(texCds[0], polSize, texSize, TEXFLIP_NONE);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);

        GetRTFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetRTTexCoord(texCds[0], polSize, texSize, TEXFLIP_HORIZONTAL);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);

        GetRBFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetRBTexCoord(texCds[0], polSize, texSize, TEXFLIP_ROTATE_180);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);

        GetLBFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetLBTexCoord(texCds[0], polSize, texSize, TEXFLIP_VERTICAL);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }
}

void Window::DrawFrame4(const math::VEC2 &basePt, const Frame *frame, const WindowFrameSize &frameSize, u8 alpha) {
    ut::Color vtxColors[4];
    detail::TexCoord texCds[1];
    math::VEC2 polPt;
    Size polSize;

    bool bModVtxCol = detail::IsModulateVertexColor(nullptr, alpha);
    if (frame[0].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[0].pMaterial->SetupGX(bModVtxCol, alpha);
        GetLTFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetLTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[0].pMaterial, 0), frame[0].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }

    if (frame[1].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[1].pMaterial->SetupGX(bModVtxCol, alpha);
        GetRTFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetRTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[1].pMaterial, 0), frame[1].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }

    if (frame[3].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[3].pMaterial->SetupGX(bModVtxCol, alpha);
        GetRBFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetRBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[3].pMaterial, 0), frame[3].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }

    if (frame[2].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[2].pMaterial->SetupGX(bModVtxCol, alpha);
        GetLBFrameSize(&polPt, &polSize, basePt, mSize, frameSize);
        GetLBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[2].pMaterial, 0), frame[2].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(polPt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }
}

void Window::DrawFrame8(const math::VEC2 &basePt, const Frame *frame, const WindowFrameSize &frameSize, u8 alpha) {
    ut::Color vtxColors[4];
    detail::TexCoord texCds[1];
    Size polSize;

    bool bModVtxCol = detail::IsModulateVertexColor(nullptr, alpha);

    if (frame[0].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[0].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.l, frameSize.t);
        GetLTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[0].pMaterial, 0), frame[0].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(basePt, polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha);
    }

    if (frame[6].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[6].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(mSize.width - frameSize.l - frameSize.r, frameSize.t);
        GetLTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[6].pMaterial, 0), frame[6].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x + frameSize.l, basePt.y), polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[1].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[1].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.r, frameSize.t);
        GetRTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[1].pMaterial, 0), frame[1].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x + mSize.width - frameSize.r, basePt.y), polSize, 1, texCds,
            bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[5].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[5].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.r, mSize.height - frameSize.t - frameSize.b);
        GetRTTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[5].pMaterial, 0), frame[5].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x + mSize.width - frameSize.r, basePt.y - frameSize.t), polSize, 1, texCds,
            bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[3].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[3].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.r, frameSize.b);
        GetRBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[3].pMaterial, 0), frame[3].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x + mSize.width - frameSize.r, basePt.y - mSize.height + frameSize.b), polSize, 1, texCds,
            bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[7].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[7].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(mSize.width - frameSize.l - frameSize.r, frameSize.b);
        GetRBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[7].pMaterial, 0), frame[7].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x + frameSize.l, basePt.y - mSize.height + frameSize.b), polSize, 1, texCds,
            bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[2].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[2].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.l, frameSize.b);
        GetLBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[2].pMaterial, 0), frame[2].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x, basePt.y - mSize.height + frameSize.b), polSize, 1, texCds,
            bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }

    if (frame[4].pMaterial->GetTextureNum() != 0) {
        bool bUseVtxCol = frame[4].pMaterial->SetupGX(bModVtxCol, alpha);
        polSize = Size(frameSize.l, mSize.height - frameSize.t - frameSize.b);
        GetLBTexCoord(texCds[0], polSize, detail::GetTextureSize(frame[4].pMaterial, 0), frame[4].textureFlip);
        detail::SetVertexFormat(bUseVtxCol, 1);
        detail::DrawQuad(
            math::VEC2(basePt.x, basePt.y - frameSize.t), polSize, 1, texCds, bUseVtxCol ? vtxColors : nullptr, alpha
        );
    }
}

WindowFrameSize Window::GetFrameSize(u8 frameNum, const Frame *frames) {
    WindowFrameSize ret = {0.0f, 0.0f, 0.0f, 0.0f};
    switch (frameNum) {
        case 1: {
            Size texSize = detail::GetTextureSize(frames[0].pMaterial, 0);
            ret.l = texSize.width;
            ret.t = texSize.height;
            ret.r = texSize.width;
            ret.b = texSize.height;
        } break;
        case 4:
        case 8: {
            Size texSize = detail::GetTextureSize(frames[0].pMaterial, 0);
            ret.l = texSize.width;
            ret.t = texSize.height;
            texSize = detail::GetTextureSize(frames[3].pMaterial, 0);
            ret.r = texSize.width;
            ret.b = texSize.height;
        } break;
    }
    return ret;
}

u8 Window::GetMaterialNum() const {
    return mFrameNum + 1;
}

Material *Window::GetMaterial(u32 idx) const {
    if (idx == 0) {
        return GetContentMaterial();
    } else {
        return GetFrameMaterial(idx - 1);
    }
}

Material *Window::GetFrameMaterial(u32 frameIdx) const {
    if (frameIdx >= mFrameNum) {
        return nullptr;
    }
    return mFrames[frameIdx].pMaterial;
}

Material *Window::GetContentMaterial() const {
    return reinterpret_cast<const Pane *>(this)->GetMaterial();
}

}

}

