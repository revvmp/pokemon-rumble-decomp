#ifndef GAME_CUILISTLAYOUTSTORAGE_H
#define GAME_CUILISTLAYOUTSTORAGE_H

#include <memory>
#include <vector>

class CLayout;

class CLayoutVectorStorage {
public:
    typedef std::tr1::shared_ptr<CLayout> Value;
    typedef std::allocator<Value> Allocator;

    ~CLayoutVectorStorage();

    unsigned long& Capacity() { return mCapacity.first(); }
    Allocator& GetAllocator() { return mCapacity.second(); }
    Value* End() { return mpData + mSize; }
    void Clear() { Erase(mSize); }
    void Erase(unsigned long count) { EraseTail(count, Metrowerks::int2type<false>()); }
    void EraseTail(unsigned long count, Metrowerks::int2type<false>) {
        Value* p = End();
        mSize -= count;
        for (; count > 0; --count) GetAllocator().destroy(--p);
    }

    Value* mpData;
    unsigned long mSize;
    Metrowerks::compressed_pair<unsigned long, Allocator> mCapacity;
};

#endif
