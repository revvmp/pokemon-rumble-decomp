#include <game/graphics/COffScreenTexture.h>
#include <game/core/Memory.h>

#include <nw4r/g3d/g3d_state.h>
#include <revolution/gx.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os/OSCache.h>

namespace {

inline u32 GetTexelBlockWidth(GXTexFmt format) {
    u32 width;
    switch (format) {
    default:
        width = 4;
        break;
    case GX_TF_I4:
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_Z8:
    case GX_CTF_R4:
    case GX_CTF_RA4:
    case GX_CTF_A8:
    case GX_CTF_R8:
    case GX_CTF_G8:
    case GX_CTF_B8:
    case GX_CTF_Z4:
    case GX_CTF_Z8M:
    case GX_CTF_Z8L:
        width = 8;
        break;
    }
    return width;
}

}

COffScreenTexture::COffScreenTexture() : mBuffer(), mBufferSize(0) {}

COffScreenTexture::~COffScreenTexture() {}

void COffScreenTexture::Create(u16 width, u16 height, GXTexFmt format, GXTexWrapMode wrap, GXTexFilter filter) {
    u32 size = GXGetTexBufferSize(width, height, format, GX_FALSE, 0);
    std::tr1::shared_ptr<u8> buffer(static_cast<u8*>(GameMemory::Allocate(size)), GameMemory::Free);
    DCFlushRange(buffer.get(), size);
    Init(buffer, size, width, height, format, wrap, filter);
}

void COffScreenTexture::Init(std::tr1::shared_ptr<u8> buffer, u32 bufferSize, u16 width, u16 height, GXTexFmt format,
                          GXTexWrapMode wrap, GXTexFilter filter) {
    mBuffer = buffer;
    mBufferSize = bufferSize;
    GXInitTexObj(&mTexObj, mBuffer.get(), width, height, format, wrap, wrap, GX_FALSE);
    GXInitTexObjLOD(&mTexObj, filter, filter, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
}

void COffScreenTexture::CopyFromEFB(bool clear) {
    GXTexFmt format = GXGetTexObjFmt(&mTexObj);
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    GXSetCopyFilter(pMode->aa, pMode->sample_pattern, GX_FALSE, pMode->vfilter);
    u16 width = GXGetTexObjWidth(&mTexObj);
    u16 height = GXGetTexObjHeight(&mTexObj) + 1 & 0xFFFE;
    GXSetTexCopySrc(0, 0, width + 1 & 0xFFFE, height);
    u16 mask = GetTexelBlockWidth(format) - 1;
    GXSetTexCopyDst(width + mask & ~mask, height, format, GX_FALSE);
    GXCopyTex(mBuffer.get(), clear);
    GXPixModeSync();
    GXSetCopyFilter(pMode->aa, pMode->sample_pattern, GX_TRUE, pMode->vfilter);
}

void COffScreenTexture::CopyFromEFB(GXTexFmt format, bool clear) {
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    GXSetCopyFilter(pMode->aa, pMode->sample_pattern, GX_FALSE, pMode->vfilter);
    u16 width = GXGetTexObjWidth(&mTexObj);
    u16 height = GXGetTexObjHeight(&mTexObj) + 1 & 0xFFFE;
    GXSetTexCopySrc(0, 0, width + 1 & 0xFFFE, height);
    u16 mask = GetTexelBlockWidth(format) - 1;
    GXSetTexCopyDst(width + mask & ~mask, height, format, GX_FALSE);
    GXCopyTex(mBuffer.get(), clear);
    GXPixModeSync();
    GXSetCopyFilter(pMode->aa, pMode->sample_pattern, GX_TRUE, pMode->vfilter);
}
