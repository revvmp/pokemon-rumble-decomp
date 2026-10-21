#ifndef GAME_CAPTURETEXTURE_H
#define GAME_CAPTURETEXTURE_H

#include <memory>
#include <revolution/gx.h>
#include <revolution/gx/GXGet.h>

class CaptureTexture {
public:
    CaptureTexture();
    ~CaptureTexture();

    void Create(u16 width, u16 height, GXTexFmt format, GXTexWrapMode wrap, GXTexFilter filter);
    void Init(const std::tr1::shared_ptr<u8>& rBuffer, u32 bufferSize, u16 width, u16 height, GXTexFmt format,
              GXTexWrapMode wrap, GXTexFilter filter);
    void CopyFromEFB(bool clear);
    void CopyFromEFB(GXTexFmt format, bool clear);

    const GXTexObj& GetTexObj() const { return mTexObj; }
    u16 GetWidth() const { return GXGetTexObjWidth(&mTexObj); }
    u16 GetHeight() const { return GXGetTexObjHeight(&mTexObj); }
    GXTexFmt GetFormat() const { return GXGetTexObjFmt(&mTexObj); }
    u32 GetBufferSize() const { return mBufferSize; }
    u8* GetBuffer() const { return mBuffer.get(); }
    std::tr1::shared_ptr<u8> GetBufferOwner() const { return mBuffer; }

private:
    GXTexObj mTexObj;
    std::tr1::shared_ptr<u8> mBuffer;
    u32 mBufferSize;
};

#endif