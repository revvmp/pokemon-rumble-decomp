#include <game/CRemoteMemory.h>
#include <wchar.h>
#include <revolution/os.h>
#include <revolution/os/OSMessage.h>
#include <revolution/os/OSThread.h>
#include <game/Memory.h>
#include <game/Message.h>
#include <revolution/wpad.h>
#include <game/CPiiPersonalData.h>
#include <game/NPpuVM.h>

#pragma opt_optimizecontrolflow off

void SetPadConnectionCallback(int channel, void (*pCallback)(long, long));
extern "C" u32 OSCalcCRC32(const void* pData, u32 size);
std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const SD_PiiPersonalData::Data& rData);
std::tr1::shared_ptr<CPpuEnv> CreateScriptEnvironment();
bool IsGameFlagSet(int flag);

namespace {
struct RemoteDataBlock {
    u32 mMagic;
    u16 mVersion;
    u8 mFormat;
    u8 mGeneration;
    SD_PiiPersonalData::Data mPii[12];
    u32 mChecksum;

    bool IsValid() const {
        if (mMagic != 'WPSJ') {
            return false;
        }
        return OSCalcCRC32(this, 536) == mChecksum;
    }
} ATTRIBUTE_ALIGN(32);

CRemoteMemory* sRemoteMemories[4];
RemoteDataBlock sReadData[4][2];
RemoteDataBlock sStoredData[4][2];
long sTransferResults[4];
long sConnectionResults[4];
CPiiBox sEmptyBox(2, 0);
OSThread sThread;
OSMessageQueue sMessageQueue;
OSMessage sMessages[16];
OSAlarm sAlarms[5];
u8* sThreadStack;
bool sInitialized;

u32 FindValidBlock(RemoteDataBlock* pBlocks) {
    RemoteDataBlock* pSecond = &pBlocks[1];
    if (pBlocks->mGeneration != pSecond->mGeneration) {
        if (pBlocks->IsValid()) {
            return 0;
        }
        if (pSecond->IsValid()) {
            return 1;
        }
    } else {
        if (pSecond->IsValid()) {
            return 1;
        }
        if (pBlocks->IsValid()) {
            return 0;
        }
    }
    return 3;
}

void OnRetryAlarm(OSAlarm* pAlarm, OSContext*) {
    OSSendMessage(&sMessageQueue, OSGetAlarmUserData(pAlarm), 0);
}

inline void ScheduleRetry(OSAlarm* const& pAlarm, const OSMessage& message, const u32& delay) {
    OSCancelAlarm(pAlarm);
    OSSetAlarmUserData(pAlarm, message);
    OSSetAlarm(pAlarm, delay, OnRetryAlarm);
}

void OnPadConnection(long channel, long result) {
    sConnectionResults[channel] = result;
    OSSendMessage(&sMessageQueue, reinterpret_cast<OSMessage>(static_cast<u16>(channel)), 0);
}

void OnRead(long channel, long result) {
    sTransferResults[channel] = result;
    OSSendMessage(&sMessageQueue, reinterpret_cast<OSMessage>(static_cast<u16>(channel) | 0x10000), 0);
}

void OnWrite(long channel, long result) {
    sTransferResults[channel] = result;
    OSSendMessage(&sMessageQueue, reinterpret_cast<OSMessage>(static_cast<u16>(channel) | 0x20000), 0);
}

void OnReadBack(long channel, long result) {
    sTransferResults[channel] = result;
    OSSendMessage(&sMessageQueue, reinterpret_cast<OSMessage>(static_cast<u16>(channel) | 0x30000), 0);
}
}

void InitializeCalculationWorker() {
    sThreadStack = static_cast<u8*>(GameMemory::AllocateMEM1(0x8000, 32));
    OSInitMessageQueue(&sMessageQueue, sMessages, 16);
    OSCreateThread(&sThread, CRemoteMemory::ThreadMain, nullptr, sThreadStack + 0x8000, 0x8000, 0, 0);
    OSResumeThread(&sThread);
    sInitialized = true;
    memset(sAlarms, 0, sizeof(sAlarms));
}

void ShutdownCalculationWorker() {
    OSSendMessage(&sMessageQueue, reinterpret_cast<OSMessage>(0x7ffff), 0);
    void* pResult;
    OSJoinThread(&sThread, &pResult);
    GameMemory::Free(sThreadStack);
    sThreadStack = nullptr;
    sInitialized = false;
}

CRemoteMemory::CRemoteMemory(int channel, COyaInfo* pOyaInfo)
    : mFlags(0), mpOyaInfo(pOyaInfo), mChannel(channel), mRemoteBox(2, 12) {
    const int remoteChannel = mChannel;
    sRemoteMemories[remoteChannel] = this;
    mPersonalData.clear();
    SetPadConnectionCallback(remoteChannel, OnPadConnection);
}

CRemoteMemory::~CRemoteMemory() {
    const int channel = mChannel;
    SetPadConnectionCallback(channel, nullptr);
    sRemoteMemories[channel] = nullptr;
}

void CRemoteMemory::QueueAdd(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData) {
    mPersonalData.push_back(rPersonalData);
}

void CRemoteMemory::QueueRemove(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData) {
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = mPersonalData.begin(); it != mPersonalData.end(); ++it) {
        if (*it == rPersonalData) {
            mPersonalData.erase(it);
            return;
        }
    }
}

void YieldFiber();

void CRemoteMemory::WaitUntilIdle(bool) {
    for (;;) {
        int frames = 0;
        for (;;) {
            if ((mFlags & FLAG_BUSY) == 0) {
                return;
            }
            YieldFiber();
            if (++frames > 300) {
                break;
            }
        }
    }
}

#pragma push
#pragma global_optimizer off
CPiiBox* CRemoteMemory::GetPiiBox() {
    if (!IsGameFlagSet(3)) {
        return &sEmptyBox;
    }
    if ((mFlags & (FLAG_HAS_DATA | FLAG_DATA_CHANGED)) == FLAG_HAS_DATA) {
        mRemoteBox.Clear();
        RemoteDataBlock* pBlock = &sStoredData[mChannel][mActiveBlock];
        for (int i = 0; i < 12; ++i) {
            const SD_PiiPersonalData::Data& rData = pBlock->mPii[i];
            if (rData.mSpecies != 0) {
                const std::tr1::shared_ptr<CPiiPersonalData> personalData = CreatePiiPersonalData(mpOyaInfo, rData);
                mRemoteBox.AddPersonalData(personalData);
            }
        }
        mRemoteBox.ApplyEnvironment(false, CreateScriptEnvironment());
        mPersonalData.assign(mRemoteBox.GetPersonalDataList().begin(), mRemoteBox.GetPersonalDataList().end());
        mFlags |= FLAG_DATA_CHANGED;
    } else if ((mFlags & (FLAG_DISCONNECTED | FLAG_DATA_CHANGED)) == FLAG_DISCONNECTED) {
        mRemoteBox.Clear();
        mPersonalData.clear();
        mFlags |= FLAG_DATA_CHANGED;
    } else if (mFlags & FLAG_WRITE_COMPLETE) {
        mRemoteBox.Clear();
        for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = mPersonalData.begin(); it != mPersonalData.end(); ++it) {
            mRemoteBox.AddPersonalData(*it);
        }
        mFlags = (mFlags & ~(FLAG_WRITE_COMPLETE | FLAG_HAS_DATA | FLAG_DATA_CHANGED)) | (FLAG_HAS_DATA | FLAG_DATA_CHANGED);
    }
    return &mRemoteBox;
}

#pragma pop

inline void CRemoteMemory::SetState(Flag state) {
    u32 stateMask = FLAG_STATE_MASK;
    if (static_cast<Flag>(mFlags & stateMask) == state) {
        mFlags &= ~FLAG_BUSY;
    } else {
        volatile u32 a = stateMask | FLAG_BUSY;
        volatile u32 b = a | FLAG_WRITE_COMPLETE;
        volatile u32 c = b | FLAG_DATA_CHANGED;
        mFlags &= ~c;
        volatile u32 d = FLAG_STATE_CHANGED | state;
        mFlags |= d;
    }
}

void CRemoteMemory::Update() {
    mFlags &= ~FLAG_STATE_CHANGED;
    if ((mFlags & (FLAG_HAS_DATA | FLAG_DISCONNECTED)) != 0 && (mFlags & (FLAG_NO_CONTROLLER | FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING)) == 0) {
        return;
    }
    if (mFlags & FLAG_CONNECTION_CHANGED) {
        mFlags &= ~(FLAG_TRANSFER_ERROR | FLAG_NO_CONTROLLER);
    }
    const int channel = mChannel;
    u16* pTitle;
    const s32 result = WPADGetGameTitleUtf16(channel, &pTitle);
    const wchar_t* pGameTitle = GetMessageString(1, nullptr);
    if (result != 0 || wcsncmp(reinterpret_cast<const wchar_t*>(pTitle), pGameTitle, 16) != 0) {
        if (result == 0) {
            SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_FOREIGN_DATA));
        } else {
            SetState(FLAG_DISCONNECTED);
            if (result != -1) {
                const BOOL enabled = OSDisableInterrupts();
                if (mFlags & FLAG_CONNECTION_CHANGED) {
                    mFlags = (mFlags | FLAG_RETRY_PENDING) & ~FLAG_CONNECTION_CHANGED;
                }
                OSRestoreInterrupts(enabled);
            }
        }
    } else {
        mFlags = (mFlags | FLAG_BUSY) & ~FLAG_NO_CONTROLLER;
        mValidationRetries = 0;
        mTransferRetries = 0;
        BeginRead(channel);
    }
}

inline void CRemoteMemory::ReadBack(int channel) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    const s32 result = WPADReadGameData(channel, reinterpret_cast<u8*>(&sReadData[channel][1]) + pRemote->mTransferOffset,
                                      pRemote->mTransferSize, pRemote->mBlockOffset + pRemote->mTransferOffset, OnReadBack);
    switch (result) {
    case -2:
    case 0:
        break;
    case -6:
    case -5:
    default:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
        break;
    case -1:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
        break;
    }
}

inline void CRemoteMemory::WriteBlock(int channel) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    const s32 result = WPADWriteGameData(channel, reinterpret_cast<u8*>(sReadData[channel]) + pRemote->mTransferOffset,
                                       pRemote->mTransferSize, pRemote->mBlockOffset + pRemote->mTransferOffset, OnWrite);
    switch (result) {
    case -2:
    case 0:
        break;
    case -1:
    default:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
        break;
    }
}

#pragma push
#pragma opt_propagation off
void* CRemoteMemory::ThreadMain(void*) {
    OSMessage message;
    for (;;) {
        OSReceiveMessage(&sMessageQueue, &message, OS_MESSAGE_BLOCK);
        const u32 command = reinterpret_cast<u32>(message);
        const int channel = static_cast<u16>(command);
        switch (command >> 16) {
        case 0: {
            CRemoteMemory* pRemote = sRemoteMemories[channel];
            const long result = sConnectionResults[channel];
            if (result != 0 && result == -1) {
                pRemote->mFlags |= FLAG_CONNECTION_CHANGED;
            }
            break;
        }
        case 1:
            FinishRead(channel, sTransferResults[channel]);
            break;
        case 2: {
            CRemoteMemory* pRemote = sRemoteMemories[channel];
            switch (sTransferResults[channel]) {
            case 0:
                pRemote->mTransferRetries = 0;
                ReadBack(channel);
                break;
            case -3:
            case -2:
                if (++pRemote->mTransferRetries < 3) {
                    ScheduleRetry(&sAlarms[channel + 1], reinterpret_cast<OSMessage>(channel | 0x50000), OS_MSEC_TO_TICKS(1000));
                } else {
                    pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
                }
                break;
            case -1:
            default:
                pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
                break;
            }
            break;
        }
        case 3:
            FinishWrite(channel, sTransferResults[channel]);
            break;
        case 4:
            BeginRead(channel);
            break;
        case 5:
            WriteBlock(channel);
            break;
        case 6:
            ReadBack(channel);
            break;
        case 7:
            return nullptr;
        }
    }
}
#pragma pop

#pragma push
#pragma opt_propagation off
void CRemoteMemory::BeginRead(int channel) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    switch (WPADReadGameData(channel, sReadData[channel], sizeof(sReadData[channel]), 0, OnRead)) {
    case -2:
    case 0:
        return;
    case -5: {
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_FOREIGN_DATA));
        const BOOL enabled = OSDisableInterrupts();
        const u32 flags = pRemote->mFlags;
        if ((flags >> 6) & 1) {
            pRemote->mFlags = (flags | FLAG_RETRY_PENDING) & ~FLAG_CONNECTION_CHANGED;
        }
        OSRestoreInterrupts(enabled);
        break;
    }
    case -6: {
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
        const BOOL enabled = OSDisableInterrupts();
        const u32 flags = pRemote->mFlags;
        if ((flags >> 6) & 1) {
            pRemote->mFlags = (flags | FLAG_RETRY_PENDING) & ~FLAG_CONNECTION_CHANGED;
        }
        OSRestoreInterrupts(enabled);
        break;
    }
    case -1:
        pRemote->SetState(FLAG_DISCONNECTED);
        pRemote->mFlags |= FLAG_NO_CONTROLLER;
        break;
    }
}

#pragma pop

#pragma push
#pragma opt_propagation off
#pragma opt_common_subs off
void CRemoteMemory::FinishRead(int channel, long result) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    switch (result) {
    case 0: {
        RemoteDataBlock* pBlocks = sReadData[channel];
        const u32 activeBlock = FindValidBlock(pBlocks);
        if (activeBlock == 3) {
            if (++pRemote->mValidationRetries < 3) {
                pRemote->mTransferRetries = 0;
                BeginRead(channel);
            } else {
                pRemote->mFlags &= ~(FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING);
                pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
            }
        } else {
            bool changed = true;
            if ((pRemote->mFlags >> 1) & 1) {
                changed = memcmp(&sReadData[channel][activeBlock], &sStoredData[channel][activeBlock], 540) != 0;
                if (changed) {
                    pRemote->mFlags = (pRemote->mFlags & ~FLAG_DATA_CHANGED) | FLAG_STATE_CHANGED;
                }
            }
            if (changed) {
                memcpy(sStoredData[channel], sReadData[channel], sizeof(sReadData[channel]));
                pRemote->mActiveBlock = activeBlock;
            }
            pRemote->mFlags &= ~(FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING);
            pRemote->SetState(FLAG_HAS_DATA);
        }
        break;
    }
    case -2:
    case -3:
        if (++pRemote->mTransferRetries < 3) {
            ScheduleRetry(&sAlarms[channel + 1], reinterpret_cast<OSMessage>(static_cast<u16>(channel) | 0x40000), OS_MSEC_TO_TICKS(1000));
        } else {
            pRemote->mFlags &= ~(FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING);
            pRemote->SetState(FLAG_DISCONNECTED);
            pRemote->mFlags |= FLAG_TRANSFER_ERROR;
        }
        break;
    case -1:
        pRemote->SetState(FLAG_DISCONNECTED);
        pRemote->mFlags |= FLAG_NO_CONTROLLER;
        break;
    case -6:
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
        break;
    case -4:
        break;
    }
}
#pragma pop

#pragma push
#pragma opt_rebuildconditionals off
#pragma opt_treetransformation off
#pragma opt_strength_reduction off
#pragma opt_dead_assignments off
#pragma opt_common_subs off
#pragma opt_generateconditionalassignments off
void CRemoteMemory::PrepareWrite(int mode) {
    int nextBlock;
    mWriteMode = mode;
    mFlags = (mFlags | FLAG_BUSY) & ~FLAG_NO_CONTROLLER;
    const int channel = mChannel;
    mValidationRetries = 0;
    mTransferRetries = 0;
    const u8 activeBlock = mActiveBlock;
    if (activeBlock == 3) {
        nextBlock = 0;
    } else {
        nextBlock = (activeBlock + 1) & 1;
    }
    if (mWriteMode != 2) {
        u8 generation = sStoredData[channel][(nextBlock - 1) & 1].mGeneration;
        if (nextBlock == 0) {
            generation += 1;
            generation &= 1;
        }
        RemoteDataBlock* pBlock = sReadData[channel];
        pBlock->mMagic = 'WPSJ';
        pBlock->mVersion = 0;
        pBlock->mFormat = 1;
        pBlock->mGeneration = generation;
        std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = mPersonalData.begin();
        for (int i = 0; i < 12; ++i) {
            SD_PiiPersonalData::Data& rData = pBlock->mPii[i];
            const bool hasData = it != mPersonalData.end();
            if (hasData) {
                (*it)->Export(rData);
                ++it;
            } else {
                rData.mSpecies = 0;
            }
        }
        pBlock->mChecksum = OSCalcCRC32(pBlock, 536);
        WPADSetGameTitleUtf16(reinterpret_cast<const u16*>(GetMessageString(1, nullptr)));
    }
    if (mWriteMode == 0 || mWriteMode == 1) {
        mTransferOffset = 0;
        mTransferSize = 536;
    } else {
        mTransferOffset = 536;
        mTransferSize = 4;
    }
    mBlockOffset = nextBlock * sizeof(RemoteDataBlock);
    WriteBlock(channel);
}
#pragma pop

#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
void CRemoteMemory::FinishWrite(int channel, long result) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    bool complete = false;
    switch (result) {
    case 0: {
        const u16 offset = pRemote->mTransferOffset;
        const u16 size = pRemote->mTransferSize;
        u8* pWritten = reinterpret_cast<u8*>(sReadData[channel]) + offset;
        bool mismatch = memcmp(pWritten, pWritten + sizeof(RemoteDataBlock), size) != 0;
        if (mismatch) {
            if (++pRemote->mValidationRetries < 3) {
                pRemote->mTransferRetries = 0;
                WriteBlock(channel);
            } else {
                pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
            }
        } else if (pRemote->mWriteMode == 1) {
            pRemote->mFlags &= ~FLAG_BUSY;
        } else if (offset == 0) {
            pRemote->mTransferOffset = size;
            pRemote->mTransferSize = 4;
            pRemote->mValidationRetries = 0;
            pRemote->mTransferRetries = 0;
            WriteBlock(channel);
        } else {
            complete = true;
        }
        break;
    }
    case -2:
    case -3:
        if (++pRemote->mTransferRetries < 3) {
            OSTime delay = OS_MSEC_TO_TICKS(1000);
            OSMessage message = reinterpret_cast<OSMessage>(static_cast<u16>(channel) | 0x60000);
            OSAlarm* pAlarm = &sAlarms[channel + 1];
            OSCancelAlarm(pAlarm);
            OSSetAlarmUserData(pAlarm, message);
            OSSetAlarm(pAlarm, delay, OnRetryAlarm);
        } else if (pRemote->mTransferOffset == 0) {
            pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
        } else {
            complete = true;
        }
        break;
    case -4:
    default:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
        break;
    case -1:
        if (pRemote->mTransferOffset == 0) {
            pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
        } else {
            complete = true;
        }
        break;
    }
    if (complete) {
        memcpy(reinterpret_cast<u8*>(sStoredData[channel]) + pRemote->mBlockOffset, sReadData[channel], 540);
        pRemote->mActiveBlock = pRemote->mActiveBlock == 3 ? 0 : (pRemote->mActiveBlock + 1) & 1;
        pRemote->mFlags = (pRemote->mFlags & ~(FLAG_BUSY | FLAG_DISCONNECTED | FLAG_FOREIGN_DATA | FLAG_INVALID_DATA | FLAG_RETRY_PENDING)) |
                          FLAG_HAS_DATA | FLAG_DATA_CHANGED | FLAG_WRITE_COMPLETE;
    }
}
#pragma pop
