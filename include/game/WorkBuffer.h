#ifndef GAME_WORKBUFFER_H
#define GAME_WORKBUFFER_H

#include <memory>

class WorkBuffer {
public:
    std::tr1::shared_ptr<u8> GetBuffer() const { return mBuffer; }
    u32 GetSize() const { return mSize; }

private:
    std::tr1::shared_ptr<u8> mBuffer;
    u32 mSize;
};

WorkBuffer* GetWorkBuffer(int index);

#endif