#ifndef GAME_CREMOTEMEMORY_H
#define GAME_CREMOTEMEMORY_H
#include <revolution/types.h>
#include <game/CPiiBox.h>
class COyaInfo;
class CRemoteMemory {
public:
    CRemoteMemory(int channel, COyaInfo* pOyaInfo);
    ~CRemoteMemory();
    bool HasFlag(const u32& flag) const { return (mFlags & flag) != 0; }
    bool IsBusy() const { return HasFlag(1); }
    CPiiBox* GetPiiBox();
    void WaitUntilIdle(bool waitForWrite);
    void Update();
    void QueueAdd(const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    void QueueRemove(const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    void PrepareWrite(int mode);
    bool IsAborted() const { return (mFlags >> 9) & 1; }
    bool HasFailed() const { return (mFlags & 0x180) != 0; }
    bool IsRecoveryPending() const { return (mFlags & 0x20) != 0; }
    bool IsStorageLocked() const { return (mFlags & 0x400) != 0; }
    bool IsBlockInvalid() const { return (mFlags >> 6) & 1; }
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
