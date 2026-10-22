#ifndef GAME_CREMOTEMEMORY_H
#define GAME_CREMOTEMEMORY_H

#include <revolution/types.h>
#include <game/CPiiBox.h>

class COyaInfo;

class CRemoteMemory {
public:
    enum Flag {
        FLAG_BUSY = 0x1,
        FLAG_HAS_DATA = 0x2,
        FLAG_DATA_CHANGED = 0x4,
        FLAG_DISCONNECTED = 0x8,
        FLAG_WRITE_COMPLETE = 0x10,
        FLAG_FOREIGN_DATA = 0x20,
        FLAG_CONNECTION_CHANGED = 0x40,
        FLAG_TRANSFER_ERROR = 0x80,
        FLAG_NO_CONTROLLER = 0x100,
        FLAG_STATE_CHANGED = 0x200,
        FLAG_INVALID_DATA = 0x400,
        FLAG_RETRY_PENDING = 0x800,
        FLAG_STATE_MASK = 0x42a,
        FLAG_STATE_AND_BUSY_MASK = 0x42b
    };

    CRemoteMemory(int channel, COyaInfo* pOyaInfo);
    ~CRemoteMemory();
    bool HasFlag(volatile u32 flag) const {
        const volatile u32& rFlags = mFlags;
        return (rFlags & flag) != 0;
    }
    bool HasPiiBox() const { return (mFlags & 0x1A) != 0; }
    bool IsBusy() const { return HasFlag(FLAG_BUSY); }
    CPiiBox* GetPiiBox();
    void WaitUntilIdle(bool waitForWrite);
    void Update();
    void QueueAdd(const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    void QueueRemove(const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    void PrepareWrite(int mode);
    bool IsAborted() const { return (mFlags & FLAG_STATE_CHANGED) != 0; }
    bool HasFailed() const { return (mFlags & 0x180) != 0; }
    bool IsRecoveryPending() const { return (mFlags & 0x20) != 0; }
    bool IsStorageLocked() const { return (mFlags & 0x400) != 0; }
    bool IsBlockInvalid() const { return (mFlags >> 6) & 1; }
    bool ConsumeFatalError() {
        u32 flags = mFlags;
        mFlags = flags & ~0x80;
        return (flags >> 7) & 1;
    }
    friend void InitializeCalculationWorker();
    friend void ShutdownCalculationWorker();

private:
    static void* ThreadMain(void* pArgument);
    static void BeginRead(int channel);
    static void ReadBack(int channel);
    static void WriteBlock(int channel);
    static void FinishRead(int channel, long result);
    static void FinishWrite(int channel, long result);
    void SetState(Flag state);

    u32 mFlags;
    COyaInfo* mpOyaInfo;
    int mChannel;
    std::list<std::tr1::shared_ptr<CPiiPersonalData> > mPersonalData;
    CPiiBox mRemoteBox;
    int mValidationRetries;
    int mTransferRetries;
    u16 mTransferSize;
    u16 mTransferOffset;
    u16 mBlockOffset;
    u8 mActiveBlock;
    int mWriteMode;
};

#endif
