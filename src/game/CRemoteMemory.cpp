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

void SetPadConnectionCallback(int channel, void (*pCallback)(long, long));
extern "C" u32 OSCalcCRC32(const void* pData, u32 size);
std::tr1::shared_ptr<CPiiPersonalData> CreatePiiPersonalData(const COyaInfo* pOyaInfo, const SD_PiiPersonalData::Data& rData);
std::tr1::shared_ptr<CPpuEnv> CreatePpuEnvironment();
bool IsGameFlagSet(int flag);

namespace {
struct RemoteDataBlock {
    u32 mMagic;
    u16 mVersion;
    u8 mFormat;
    u8 mGeneration;
    SD_PiiPersonalData::Data mPii[12];
    u32 mChecksum;
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

void OnRetryAlarm(OSAlarm* pAlarm, OSContext*) {
    OSSendMessage(&sMessageQueue, OSGetAlarmUserData(pAlarm), 0);
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
        mRemoteBox.ApplyEnvironment(false, CreatePpuEnvironment());
        mPersonalData = mRemoteBox.GetPersonalDataList();
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

inline void CRemoteMemory::SetState(Flag state) {
    if ((mFlags & FLAG_STATE_MASK) == state) {
        mFlags &= ~FLAG_BUSY;
    } else {
        volatile u32 a = FLAG_STATE_AND_BUSY_MASK;
        volatile u32 b = a | FLAG_WRITE_COMPLETE;
        volatile u32 c = b | FLAG_DATA_CHANGED;
        u32 flags = mFlags & ~c;
        volatile u32 d = state | FLAG_STATE_CHANGED;
        mFlags = flags | d;
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
    case -1:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
        break;
    default:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
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

void* CRemoteMemory::ThreadMain(void*) {
    OSMessage message;
    for (;;) {
        OSReceiveMessage(&sMessageQueue, &message, OS_MESSAGE_BLOCK);
        const u32 command = reinterpret_cast<u32>(message);
        const int channel = static_cast<u16>(command);
        switch (command >> 16) {
        case 0:
            switch (sConnectionResults[channel]) {
            case 0:
                break;
            case -1:
                sRemoteMemories[channel]->mFlags |= FLAG_CONNECTION_CHANGED;
                break;
            }
            break;
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
                if (++pRemote->mTransferRetries >= 3) {
                    pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
                } else {
                    OSAlarm* pAlarm = &sAlarms[channel + 1];
                    const u32 delay = OS_MSEC_TO_TICKS(1000);
                    OSCancelAlarm(pAlarm);
                    OSSetAlarmUserData(pAlarm, reinterpret_cast<void*>(channel | 0x50000));
                    OSSetAlarm(pAlarm, delay, OnRetryAlarm);
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

void CRemoteMemory::BeginRead(int channel) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    switch (WPADReadGameData(channel, sReadData[channel], sizeof(sReadData[channel]), 0, OnRead)) {
    case -2:
    case 0:
        return;
    case -5: {
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_FOREIGN_DATA));
        const BOOL enabled = OSDisableInterrupts();
        if (pRemote->mFlags & FLAG_CONNECTION_CHANGED) {
            pRemote->mFlags = (pRemote->mFlags | FLAG_RETRY_PENDING) & ~FLAG_CONNECTION_CHANGED;
        }
        OSRestoreInterrupts(enabled);
        break;
    }
    case -6: {
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
        const BOOL enabled = OSDisableInterrupts();
        if (pRemote->mFlags & FLAG_CONNECTION_CHANGED) {
            pRemote->mFlags = (pRemote->mFlags | FLAG_RETRY_PENDING) & ~FLAG_CONNECTION_CHANGED;
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

void CRemoteMemory::FinishRead(int channel, long result) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    switch (result) {
    case 0: {
        int activeBlock;
        RemoteDataBlock* pBlocks = sReadData[channel];
        RemoteDataBlock* pSecond = &pBlocks[1];
        if (pBlocks[0].mGeneration == pSecond->mGeneration) {
            if (pSecond->mMagic == 'WPSJ' && OSCalcCRC32(pSecond, 536) == pSecond->mChecksum) {
                activeBlock = 1;
            } else if (pBlocks[0].mMagic == 'WPSJ' && OSCalcCRC32(&pBlocks[0], 536) == pBlocks[0].mChecksum) {
                activeBlock = 0;
            } else {
                activeBlock = 3;
            }
        } else {
            if (pBlocks[0].mMagic == 'WPSJ' && OSCalcCRC32(&pBlocks[0], 536) == pBlocks[0].mChecksum) {
                activeBlock = 0;
            } else if (pSecond->mMagic == 'WPSJ' && OSCalcCRC32(pSecond, 536) == pSecond->mChecksum) {
                activeBlock = 1;
            } else {
                activeBlock = 3;
            }
        }
        if (activeBlock == 3) {
            if (++pRemote->mValidationRetries >= 3) {
                pRemote->mFlags &= ~(FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING);
                pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
            } else {
                pRemote->mTransferRetries = 0;
                BeginRead(channel);
            }
        } else {
            bool changed = true;
            if (pRemote->mFlags & FLAG_HAS_DATA) {
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
        if (++pRemote->mTransferRetries >= 3) {
            pRemote->mFlags &= ~(FLAG_CONNECTION_CHANGED | FLAG_RETRY_PENDING);
            pRemote->SetState(FLAG_DISCONNECTED);
            pRemote->mFlags |= FLAG_TRANSFER_ERROR;
        } else {
            OSAlarm* pAlarm = &sAlarms[channel + 1];
            const u32 delay = OS_MSEC_TO_TICKS(1000);
            OSCancelAlarm(pAlarm);
            OSSetAlarmUserData(pAlarm, reinterpret_cast<void*>(static_cast<u16>(channel) | 0x40000));
            OSSetAlarm(pAlarm, delay, OnRetryAlarm);
        }
        break;
    case -1:
        pRemote->SetState(FLAG_DISCONNECTED);
        pRemote->mFlags |= FLAG_NO_CONTROLLER;
        break;
    case -6:
        pRemote->SetState(static_cast<Flag>(FLAG_DISCONNECTED | FLAG_INVALID_DATA));
        break;
    }
}

void CRemoteMemory::PrepareWrite(int mode) {
    int nextBlock = 0;
    mWriteMode = mode;
    mFlags = (mFlags | FLAG_BUSY) & ~FLAG_NO_CONTROLLER;
    const int channel = mChannel;
    mValidationRetries = 0;
    mTransferRetries = 0;
    const u8 activeBlock = mActiveBlock;
    if (activeBlock != 3) {
        nextBlock = (activeBlock + 1) & 1;
    }
    if (mWriteMode != 2) {
        u8 generation = sStoredData[channel][(nextBlock - 1) & 1].mGeneration;
        if (nextBlock == 0) {
            generation = (generation + 1) & 1;
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
        pBlock->mChecksum = OSCalcCRC32(sReadData[channel], 536);
        WPADSetGameTitleUtf16(reinterpret_cast<const u16*>(GetMessageString(1, nullptr)));
    }
    if (mWriteMode > 1) {
        mTransferOffset = 536;
        mTransferSize = 4;
    } else {
        mTransferOffset = 0;
        mTransferSize = 536;
    }
    mBlockOffset = nextBlock * sizeof(RemoteDataBlock);
    WriteBlock(channel);
}

void CRemoteMemory::FinishWrite(int channel, long result) {
    CRemoteMemory* pRemote = sRemoteMemories[channel];
    bool complete = false;
    switch (result) {
    case 0: {
        const u16 size = pRemote->mTransferSize;
        const u16 offset = pRemote->mTransferOffset;
        u8* pData = reinterpret_cast<u8*>(sReadData[channel]);
        if (memcmp(pData + offset, pData + offset + sizeof(RemoteDataBlock), size) != 0) {
            if (++pRemote->mValidationRetries >= 3) {
                pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
            } else {
                pRemote->mTransferRetries = 0;
                WriteBlock(channel);
            }
        } else if (pRemote->mWriteMode == 1) {
            pRemote->mFlags &= ~FLAG_BUSY;
        } else if (offset != 0) {
            complete = true;
        } else {
            pRemote->mTransferOffset = size;
            pRemote->mTransferSize = 4;
            pRemote->mValidationRetries = 0;
            pRemote->mTransferRetries = 0;
            WriteBlock(channel);
        }
        break;
    }
    case -1:
        if (pRemote->mTransferOffset != 0) {
            complete = true;
        } else {
            pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_NO_CONTROLLER;
        }
        break;
    case -2:
    case -3:
        if (++pRemote->mTransferRetries >= 3) {
            if (pRemote->mTransferOffset != 0) {
                complete = true;
            } else {
                pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
            }
        } else {
            OSAlarm* pAlarm = &sAlarms[channel + 1];
            const u32 delay = OS_MSEC_TO_TICKS(1000);
            OSCancelAlarm(pAlarm);
            OSSetAlarmUserData(pAlarm, reinterpret_cast<void*>(static_cast<u16>(channel) | 0x60000));
            OSSetAlarm(pAlarm, delay, OnRetryAlarm);
        }
        break;
    default:
        pRemote->mFlags = (pRemote->mFlags & ~FLAG_BUSY) | FLAG_TRANSFER_ERROR;
        break;
    }
    if (complete) {
        memcpy(reinterpret_cast<u8*>(sStoredData[channel]) + pRemote->mBlockOffset, sReadData[channel], 540);
        if (pRemote->mActiveBlock == 3) {
            pRemote->mActiveBlock = 0;
        } else {
            pRemote->mActiveBlock = (pRemote->mActiveBlock + 1) & 1;
        }
        pRemote->mFlags = (pRemote->mFlags & ~0xc3f) | 0x16;
    }
}
