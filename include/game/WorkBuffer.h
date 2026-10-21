#ifndef GAME_WORKBUFFER_H
#define GAME_WORKBUFFER_H

#include <memory>

class WorkBuffer {
public:
    WorkBuffer() : mSize(0) {}

    std::tr1::shared_ptr<u8> GetBuffer() const { return mBuffer; }
    u32 GetSize() const { return mSize; }

    void Allocate();

    friend void InitializeWorkBuffer();

private:
    std::tr1::shared_ptr<u8> mBuffer;
    u32 mSize;
};

void InitializeWorkBuffer();
void ShutdownWorkBuffer();
WorkBuffer* GetWorkBuffer(int index);

#endif