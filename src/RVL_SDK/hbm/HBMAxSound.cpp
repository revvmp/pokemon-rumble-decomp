#include "revolution/hbm/homebutton/HBMAxSound.h"

#include <math.h>

#include "revolution/ai.h"
#include "revolution/arc.h"
#include "revolution/ax.h"
#include "revolution/ax/AX.h"
#include "revolution/hbm/HBMBase.h"
#include "revolution/hbm/sound/mix.h"
#include "revolution/hbm/sound/seq.h"
#include "revolution/hbm/sound/syn.h"
#include "revolution/os.h"
#include "revolution/os/OSMessage.h"
#include "revolution/os/OSThread.h"

const char* SOUND_FILENAME[HBMSE_MAX] = {
    "midi\\00_home_button.mid",
    "midi\\01_return_app.mid",
    "midi\\02_goto_menu.mid",
    "midi\\03_reset_app.mid",
    "midi\\04_focus.mid",
    "midi\\05_select.mid",
    "midi\\06_cancel.mid",
    "midi\\07_open_controller.mid",
    "midi\\08_close_controller.mid",
    "midi\\09_volume_plus.mid",
    "midi\\10_volume_minus.mid",
    "midi\\11_volume_plus_limit.mid",
    "midi\\12_volume_minus_limit.mid",
    "midi\\13_nothing_done.mid",
    "midi\\14_vibe_on.mid",
    "midi\\15_vibe_off.mid",
    "midi\\16_start_connect_window.mid",
    "midi\\17_connected1.mid",
    "midi\\18_connected2.mid",
    "midi\\19_connected3.mid",
    "midi\\20_connected4.mid",
    "midi\\21_end_connect_window.mid",
    "midi\\22_manual_open.mid",
    "midi\\23_manual_focus.mid",
    "midi\\24_manual_select.mid",
    "midi\\25_manual_scroll.mid",
    "midi\\26_manual_cancel.mid",
    "midi\\27_manual_return_app.mid",
};

#define IS_CURSOR_SOUND(num) ((num) == HBMSE_FOCUS || (num) == HBMSE_MANUAL_FOCUS || (num) == HBMSE_MANUAL_SCROLL)

namespace {

enum {
    SEQ_PLAYER_NUM = 4,
    CURSOR_SEQ_PLAYER_NUM = 3,
    SOUND_THREAD_PRIORITY = 4,
    SOUND_THREAD_STACK_SIZE = 0x4000,
    SOUND_MESSAGE_NUM = 4
};

enum {
    SOUND_MESSAGE_AUDIO_FRAME = 1,
    SOUND_MESSAGE_QUIT = 8
};

const s32 MIN_VOLUME_DB = -904;
const s32 MAX_VOLUME_DB = 60;

const char WT_FILENAME[] = "wt\\HomeButtonSe.wt";
const char PCM_FILENAME[] = "wt\\HomeButtonSe.pcm";

struct SeqPlayer {
    HBMSEQSEQUENCE seq;
    bool inUse;
    SeqPlayer* next;
    SeqPlayer* prev;
    int num;
};

struct SeqPlayerList {
    SeqPlayer* head;
    SeqPlayer* tail;
};

struct AxSoundWork {
    SeqPlayer player[SEQ_PLAYER_NUM];
    SeqPlayer cursorPlayer[CURSOR_SEQ_PLAYER_NUM];
    SeqPlayerList playerList;
    SeqPlayerList cursorPlayerList;
    u32 reserved;
    AXOutCallback oldAxCallback;
    ARCHandle arcHandle;
    OSThread thread;
    OSMessageQueue messageQueue;
    OSMessage messages[SOUND_MESSAGE_NUM];
    void* threadStack;
    void* wavetable;
    void* samples;
    u8 padding[0x44];
};

AxSoundWork* sWork;

inline SeqPlayerList* GetPlayerList(int num) {
    if (IS_CURSOR_SOUND(num)) {
        return &sWork->cursorPlayerList;
    }

    return &sWork->playerList;
}

inline void AppendPlayer(SeqPlayerList* pList, SeqPlayer* pPlayer) {
    if (pList->tail == nullptr) {
        pList->head = pPlayer;
        pList->tail = pPlayer;
        pPlayer->next = nullptr;
        pPlayer->prev = nullptr;
    } else {
        pList->tail->next = pPlayer;
        pPlayer->prev = pList->tail;
        pPlayer->next = nullptr;
        pList->tail = pPlayer;
    }
}

inline void RemovePlayer(SeqPlayerList* pList, SeqPlayer* pPlayer) {
    if (pPlayer->prev == nullptr) {
        pList->head = pPlayer->next;
    } else {
        pPlayer->prev->next = pPlayer->next;
    }

    if (pPlayer->next == nullptr) {
        pList->tail = pPlayer->prev;
    } else {
        pPlayer->next->prev = pPlayer->prev;
    }

    pPlayer->next = nullptr;
    pPlayer->prev = nullptr;
}

inline void StopPlayer(SeqPlayer* pPlayer) {
    SeqPlayerList* pList = GetPlayerList(pPlayer->num);

    HBMSEQSetState(&pPlayer->seq, HBMSEQ_STATE_STOP);
    HBMSEQRemoveSequence(&pPlayer->seq);
    pPlayer->inUse = false;
    RemovePlayer(pList, pPlayer);
}

inline void UpdatePlayerList(SeqPlayerList* pList) {
    SeqPlayer* pPlayer = pList->head;

    while (pPlayer != nullptr) {
        SeqPlayer* pNext = pPlayer->next;

        if (pPlayer->inUse && HBMSEQGetState(&pPlayer->seq) == HBMSEQ_STATE_STOP && pPlayer->seq.synth.notes == 0) {
            StopPlayer(pPlayer);
        }

        pPlayer = pNext;
    }
}

SeqPlayer* GetFreePlayer(int num) {
    SeqPlayer* pPlayers;
    int playerNum;

    if (IS_CURSOR_SOUND(num)) {
        pPlayers = sWork->cursorPlayer;
        playerNum = CURSOR_SEQ_PLAYER_NUM;
    } else {
        pPlayers = sWork->player;
        playerNum = SEQ_PLAYER_NUM;
    }

    SeqPlayer* pFree = nullptr;

    for (int i = 0; i < playerNum; i++) {
        if (!pPlayers[i].inUse) {
            pFree = &pPlayers[i];
            pFree->inUse = true;
            break;
        }
    }

    if (pFree == nullptr) {
        pFree = GetPlayerList(num)->head;
        StopPlayer(pFree);
        pFree->inUse = true;
    }

    return pFree;
}

void AudioFrameCallback() {
    if (sWork != nullptr) {
        OSSendMessage(&sWork->messageQueue, reinterpret_cast<OSMessage>(SOUND_MESSAGE_AUDIO_FRAME), OS_MESSAGE_NOBLOCK);

        if (sWork->oldAxCallback != nullptr) {
            sWork->oldAxCallback();
        }
    }
}

void* AudioSoundThreadProc(void*) {
    OSMessage message = nullptr;

    while (true) {
        OSReceiveMessage(&sWork->messageQueue, &message, OS_MESSAGE_BLOCK);

        if (reinterpret_cast<u32>(message) == SOUND_MESSAGE_AUDIO_FRAME) {
            HBMSEQRunAudioFrame();
            HBMSYNRunAudioFrame();
            HBMMIXUpdateSettings();

            if (sWork != nullptr) {
                UpdatePlayerList(&sWork->playerList);
                UpdatePlayerList(&sWork->cursorPlayerList);
            }
        } else if (reinterpret_cast<u32>(message) == SOUND_MESSAGE_QUIT) {
            break;
        }
    }

    return nullptr;
}

}

namespace homebutton {

void PlaySeq(int num) {
    ARCFileInfo af;
    void* pMidi;

    if (ARCOpen(&sWork->arcHandle, SOUND_FILENAME[num], &af)) {
        pMidi = ARCGetStartAddrInMem(&af);
    } else {
        pMidi = nullptr;
    }

    if (pMidi != nullptr) {
        SeqPlayer* pPlayer = GetFreePlayer(num);

        HBMSEQAddSequence(&pPlayer->seq, static_cast<u8*>(pMidi), static_cast<u8*>(sWork->wavetable), static_cast<u8*>(sWork->samples), nullptr);
        HBMSEQSetState(&pPlayer->seq, HBMSEQ_STATE_RUN);
        pPlayer->num = num;

        AppendPlayer(GetPlayerList(num), pPlayer);
    }
}

void InitAxSound(const void* pSoundData, void* pMem, u32 memSize) {
    if (memSize < sizeof(AxSoundWork) + SOUND_THREAD_STACK_SIZE) {
        return;
    }

    if (!AICheckInit()) {
        AIInit(nullptr);
        AXInit();
    }

    HBMMIXInit();
    HBMSYNInit();
    HBMSEQInit();

    AxSoundWork* pWork = static_cast<AxSoundWork*>(pMem);

    for (int i = 0; i < SEQ_PLAYER_NUM; i++) {
        pWork->player[i].inUse = false;
        pWork->player[i].next = nullptr;
        pWork->player[i].prev = nullptr;
    }

    for (int i = 0; i < CURSOR_SEQ_PLAYER_NUM; i++) {
        pWork->cursorPlayer[i].inUse = false;
        pWork->cursorPlayer[i].next = nullptr;
        pWork->cursorPlayer[i].prev = nullptr;
    }

    pWork->wavetable = nullptr;
    pWork->samples = nullptr;
    pWork->threadStack = pWork + 1;
    pWork->playerList.head = nullptr;
    pWork->playerList.tail = nullptr;
    pWork->cursorPlayerList.head = nullptr;
    pWork->cursorPlayerList.tail = nullptr;
    pWork->oldAxCallback = nullptr;

    if (!ARCInitHandle(const_cast<void*>(pSoundData), &pWork->arcHandle)) {
        return;
    }

    ARCFileInfo wtInfo;
    if (!ARCOpen(&pWork->arcHandle, WT_FILENAME, &wtInfo)) {
        return;
    }
    pWork->wavetable = ARCGetStartAddrInMem(&wtInfo);

    ARCFileInfo pcmInfo;
    if (!ARCOpen(&pWork->arcHandle, PCM_FILENAME, &pcmInfo)) {
        return;
    }
    pWork->samples = ARCGetStartAddrInMem(&pcmInfo);

    OSInitMessageQueue(&pWork->messageQueue, pWork->messages, SOUND_MESSAGE_NUM);

    if (!OSCreateThread(&pWork->thread, &AudioSoundThreadProc, nullptr, static_cast<u8*>(pWork->threadStack) + memSize - sizeof(AxSoundWork), memSize - sizeof(AxSoundWork), SOUND_THREAD_PRIORITY, 0)) {
        return;
    }

    sWork = pWork;
    OSResumeThread(&pWork->thread);

    BOOL enabled = OSDisableInterrupts();
    pWork->oldAxCallback = AXRegisterCallback(&AudioFrameCallback);
    OSRestoreInterrupts(enabled);
}

void ShutdownAxSound() {
    if (sWork == nullptr) {
        return;
    }

    StopAllSeq();
    AXRegisterCallback(sWork->oldAxCallback);
    OSJamMessage(&sWork->messageQueue, reinterpret_cast<OSMessage>(SOUND_MESSAGE_QUIT), OS_MESSAGE_BLOCK);
    OSJoinThread(&sWork->thread, nullptr);

    HBMSEQQuit();
    HBMSYNQuit();
    HBMMIXQuit();

    sWork = nullptr;
}

void AxSoundMain() {}

void StopAllSeq() {
    if (sWork == nullptr) {
        return;
    }

    for (int i = 0; i < SEQ_PLAYER_NUM; i++) {
        SeqPlayer* pPlayer = &sWork->player[i];

        if (pPlayer->inUse) {
            StopPlayer(pPlayer);
        }
    }

    for (int i = 0; i < CURSOR_SEQ_PLAYER_NUM; i++) {
        SeqPlayer* pPlayer = &sWork->cursorPlayer[i];

        if (pPlayer->inUse) {
            StopPlayer(pPlayer);
        }
    }
}

void SetVolumeAllSeq(f32 gain) {
    if (sWork == nullptr) {
        return;
    }

    s32 dB;

    if (gain <= 0.0f) {
        dB = MIN_VOLUME_DB;
    } else {
        dB = static_cast<s32>(10.0f * (20.0f * log10f(gain)));
    }

    if (dB > MAX_VOLUME_DB) {
        dB = MAX_VOLUME_DB;
    }

    if (dB < MIN_VOLUME_DB) {
        dB = MIN_VOLUME_DB;
    }

    for (int i = 0; i < SEQ_PLAYER_NUM; i++) {
        if (sWork->player[i].inUse) {
            HBMSEQSetVolume(&sWork->player[i].seq, dB);
        }
    }

    for (int i = 0; i < CURSOR_SEQ_PLAYER_NUM; i++) {
        SeqPlayer* pPlayer = &sWork->cursorPlayer[i];

        if (pPlayer->inUse) {
            HBMSEQSetVolume(&pPlayer->seq, dB);
        }
    }
}

void SetSoundMode(u32 mode) {
    HBMMIXSetSoundMode(mode);
}

}
