#include "revolution/hbm/homebutton/HBMRemoteSpk.h"

namespace homebutton {

static const int scPcmSamples = 40;
static const int scAdpcmBytes = 20;

RemoteSpk* RemoteSpk::spInstance;

void RemoteSpk::SetInstance(RemoteSpk* pThis) {
    spInstance = pThis;
}

RemoteSpk* RemoteSpk::GetInstance() {
    return spInstance;
}

void RemoteSpk::GetPCMFromSeID(int id, s16*& rpPcm, int& rLength) {
    ARCFileInfo af;
    ARCFastOpen(&handle, id, &af);

    rpPcm = static_cast<s16*>(ARCGetStartAddrInMem(&af));
    rLength = ARCGetLength(&af);

    ARCClose(&af);
}

static bool MakeVolumeData(const s16* pSrc, s16* pDst, int vol, u32 size) {
    u32 enc_size = size <= scPcmSamples ? size : scPcmSamples;

    for (int i = 0; static_cast<u32>(i) < enc_size; i++) {
        *pDst++ = static_cast<s16>(*pSrc++ * vol / 10);
    }

    if (size > scPcmSamples) {
        return false;
    }

    u32 zero_size = scPcmSamples - size;

    for (int i = 0; static_cast<u32>(i) < zero_size; i++) {
        *pDst++ = 0;
    }

    return true;
}

void RemoteSpk::UpdateSpeaker(OSAlarm*, OSContext*) {
    s16 pcmBuffer[scPcmSamples];
    u8 adpcmBuffer[scAdpcmBytes];

    if (GetInstance() == nullptr) {
        return;
    }

    ChanInfo* pInfo = GetInstance()->info;

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++, pInfo++) {
        if (pInfo->in_pcm != nullptr && WPADIsSpeakerEnabled(i)) {
            BOOL intrStatus = OSDisableInterrupts();

            if (WPADCanSendStreamData(i)) {
                MakeVolumeData(pInfo->in_pcm, pcmBuffer, pInfo->vol, static_cast<u32>(pInfo->length) / sizeof(s16));
                WENCGetEncodeData(&pInfo->wencinfo, pInfo->first ? 0 : WENC_FLAG_USER_INFO, pcmBuffer, scPcmSamples, adpcmBuffer);
                WPADSendStreamData(i, adpcmBuffer, scAdpcmBytes);

                pInfo->first = false;
                pInfo->cannotSendCnt = 0;
                pInfo->in_pcm += scPcmSamples;
                pInfo->length -= scPcmSamples * sizeof(s16);

                if (pInfo->length <= 0) {
                    pInfo->seId = -1;
                    pInfo->in_pcm = nullptr;
                }
            } else {
                pInfo->cannotSendCnt++;

                if (pInfo->cannotSendCnt > 60) {
                    pInfo->in_pcm = nullptr;
                }
            }

            OSRestoreInterrupts(intrStatus);
        }
    }
}

void RemoteSpk::ClearPcm() {
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        info[i].in_pcm = nullptr;
        info[i].seId = -1;
    }
}

RemoteSpk::RemoteSpk(void* pSpkSeBuf) {
    SetInstance(this);

    if (pSpkSeBuf != nullptr) {
        available = ARCInitHandle(pSpkSeBuf, &handle) ? true : false;
    } else {
        available = false;
    }

    OSCreateAlarm(&speakerAlarm);

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        OSCreateAlarm(&info[i].alarm);
        info[i].in_pcm = nullptr;
        info[i].seId = -1;
        info[i].first = true;
        info[i].playReady = true;
    }
}

RemoteSpk::~RemoteSpk() {
    SetInstance(nullptr);
    available = false;

    OSCancelAlarm(&speakerAlarm);

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        OSCancelAlarm(&info[i].alarm);
    }
}

void RemoteSpk::Start() {
    if (!available) {
        return;
    }

    OSCreateAlarm(&speakerAlarm);
    OSSetPeriodicAlarm(&speakerAlarm, OSGetTime(), OSNanosecondsToTicks(6666667), &UpdateSpeaker);

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        OSCreateAlarm(&info[i].alarm);
        info[i].in_pcm = nullptr;
        info[i].seId = -1;
        info[i].first = true;
        info[i].playReady = true;
    }
}

void RemoteSpk::Stop() {
    OSCancelAlarm(&speakerAlarm);

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        OSCancelAlarm(&info[i].alarm);
    }
}

void RemoteSpk::DelaySpeakerOnCallback(OSAlarm* pAlarm, OSContext*) {
    s32 chan = reinterpret_cast<s32>(OSGetAlarmUserData(pAlarm));
    WPADControlSpeaker(chan, WPAD_SPEAKER_ON, &SpeakerOnCallback);
}

void RemoteSpk::SpeakerOnCallback(s32 chan, s32 result) {
    RemoteSpk* pRmtSpk = GetInstance();

    if (pRmtSpk == nullptr) {
        return;
    }

    switch (result) {
    case WPAD_ERR_NONE:
        pRmtSpk->info[chan].first = true;
        WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
        break;

    case WPAD_ERR_TRANSFER:
        pRmtSpk->info[chan].playReady = false;
        break;

    case WPAD_ERR_BUSY:
        OSSetAlarmUserData(&pRmtSpk->info[chan].alarm, reinterpret_cast<void*>(chan));
        OSCancelAlarm(&pRmtSpk->info[chan].alarm);
        OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50), &DelaySpeakerOnCallback);
        break;
    }
}

void RemoteSpk::DelaySpeakerPlayCallback(OSAlarm* pAlarm, OSContext*) {
    s32 chan = reinterpret_cast<s32>(OSGetAlarmUserData(pAlarm));
    WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
}

void RemoteSpk::SpeakerPlayCallback(s32 chan, s32 result) {
    RemoteSpk* pRmtSpk = GetInstance();

    if (pRmtSpk == nullptr) {
        return;
    }

    switch (result) {
    case WPAD_ERR_NONE:
        pRmtSpk->info[chan].playReady = true;
        break;

    case WPAD_ERR_TRANSFER:
        pRmtSpk->info[chan].playReady = false;
        break;

    case WPAD_ERR_NO_CONTROLLER:
        pRmtSpk->info[chan].playReady = false;
        break;

    case WPAD_ERR_BUSY:
        OSSetAlarmUserData(&pRmtSpk->info[chan].alarm, reinterpret_cast<void*>(chan));
        OSCancelAlarm(&pRmtSpk->info[chan].alarm);
        OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50), &DelaySpeakerPlayCallback);
        break;
    }
}

void RemoteSpk::Connect(s32 chan) {
    if (!available) {
        return;
    }

    WPADControlSpeaker(chan, WPAD_SPEAKER_ON, &SpeakerOnCallback);

    u32* pWork = reinterpret_cast<u32*>(&info[chan].wencinfo);
    for (u32 i = 0; i < sizeof(WENCInfo) / sizeof(u32); i++) {
        pWork[i] = 0;
    }

    info[chan].first = true;
    info[chan].playReady = false;
}

void RemoteSpk::Play(s32 chan, int seId, s8 vol) {
    if (!available) {
        return;
    }

    s16* pPcm;
    int length;
    GetPCMFromSeID(seId, pPcm, length);

    info[chan].cannotSendCnt = 0;
    info[chan].seId = seId;
    info[chan].length = length;
    info[chan].vol = vol;
    info[chan].in_pcm = pPcm;
}

bool RemoteSpk::isPlaying(s32 chan) const {
    return info[chan].in_pcm != nullptr;
}

bool RemoteSpk::isPlayingId(s32 chan, int seId) const {
    if (isPlaying(chan) && info[chan].seId == seId) {
        return true;
    }

    return false;
}

bool RemoteSpk::isPlayReady(s32 chan) const {
    return info[chan].playReady != false;
}

}
