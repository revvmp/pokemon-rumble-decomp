#include <game/graphics/WorkBuffer.h>

#include <algorithm>
#include <game/core/Memory.h>
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

}

void InitializeWorkBuffer() {
    sWork.reset(new Work);
    sWork->mBuffers.resize(1);

    const WorkBuffer::TexSize texSizes[] = {{640, 528, GX_TF_RGB565}};

    sWork->mBuffers[0].Allocate(texSizes, 1);
}

WorkBuffer* GetWorkBuffer(int index) {
    return &sWork->mBuffers[index];
}

void ShutdownWorkBuffer() {
    sWork.reset();
}

inline void WorkBuffer::Allocate(const TexSize* pTexSizes, u32 count) {
    mSize = 0;
    for (u32 i = 0; i < count; i++) {
        u32 size = GXGetTexBufferSize(pTexSizes[i].mWidth, pTexSizes[i].mHeight, pTexSizes[i].mFormat, GX_FALSE, 0);
        mSize = std::max(size, mSize);
    }

    u8* pBuffer = static_cast<u8*>(GameMemory::Allocate(mSize, 32));
    mBuffer.reset(pBuffer, GameMemory::Free);
    DCFlushRange(mBuffer.get(), mSize);
}