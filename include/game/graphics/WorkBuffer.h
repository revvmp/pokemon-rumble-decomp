#ifndef GAME_WORKBUFFER_H
#define GAME_WORKBUFFER_H

#include <memory>
#include <revolution/gx/GXEnum.h>

class WorkBuffer {
public:
    struct TexSize {
        u16 mWidth;
        u16 mHeight;
        GXTexFmt mFormat;
    };

    WorkBuffer() : mSize(0) {}

    std::tr1::shared_ptr<u8> GetBuffer() const { return mBuffer; }
    u32 GetSize() const { return mSize; }

    void Allocate(const TexSize* pTexSizes, u32 count);

private:
    std::tr1::shared_ptr<u8> mBuffer;
    u32 mSize;
};

void InitializeWorkBuffer();
void ShutdownWorkBuffer();
WorkBuffer* GetWorkBuffer(int index);

#endif