#include <game/WorkBuffer.h>

#include <algorithm>
#include <game/Memory.h>
#include <memory>
#include <revolution/gx.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os/OSCache.h>
#include <vector>

namespace {

struct Work {
    std::vector<WorkBuffer> mBuffers;
};

std::tr1::shared_ptr<Work> sWork;

struct TexSize {
    u16 mWidth;
    u16 mHeight;
    GXTexFmt mFormat;
};

}

void InitializeWorkBuffer() {
    sWork.reset(new Work);
    sWork->mBuffers.resize(1);

    const TexSize texSizes[] = {{640, 528, GX_TF_RGB565}};

    WorkBuffer& rBuffer = sWork->mBuffers[0];
    rBuffer.mSize = 0;
    for (u32 i = 0; i < 1; i++) {
        u32 size = GXGetTexBufferSize(texSizes[i].mWidth, texSizes[i].mHeight, texSizes[i].mFormat, GX_FALSE, 0);
        rBuffer.mSize = std::max(size, rBuffer.mSize);
    }

    rBuffer.Allocate();
}

WorkBuffer* GetWorkBuffer(int index) {
    return &sWork->mBuffers[index];
}

void ShutdownWorkBuffer() {
    sWork.reset();
}

inline void WorkBuffer::Allocate() {
    u8* pBuffer = static_cast<u8*>(GameMemory::Allocate(mSize, 32));
    mBuffer.reset(pBuffer, GameMemory::Free);
    DCFlushRange(mBuffer.get(), mSize);
}
