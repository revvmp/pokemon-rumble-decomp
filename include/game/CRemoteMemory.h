#ifndef GAME_CREMOTEMEMORY_H
#define GAME_CREMOTEMEMORY_H
#include <revolution/types.h>
#include <game/CPiiBox.h>
class COyaInfo;
class CRemoteMemory {
public:
    CRemoteMemory(int channel, COyaInfo* pOyaInfo);
    ~CRemoteMemory();
    bool IsBusy() const { return (mFlags & 1) != 0; }
    void WaitUntilIdle(bool waitForWrite);
    void Update();
    bool ConsumeFatalError() {
        u32 flags = mFlags;
        mFlags = flags & ~0x80;
        return (flags & 0x80) != 0;
    }
private:
    u32 mFlags;
    COyaInfo* mpOyaInfo;
    int mChannel;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPendingPiis;
    CPiiBox mRemoteBox;
    int mRetryCount;
    int mWaitCount;
    u16 mTransferSize;
    u16 mTransferOffset;
    u16 mBlockOffset;
    u8 mActiveBlock;
    u32 mWriteMode;
};
#endif
