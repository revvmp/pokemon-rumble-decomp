#ifndef HBM_REMOTE_SPK_H
#define HBM_REMOTE_SPK_H

#include "revolution/os.h"
#include "revolution/os/OSMutex.h"
#include "revolution/arc.h"
#include "revolution/wenc.h"
#include "revolution/wpad.h"

namespace homebutton {

class RemoteSpk {
private:
    struct ChanInfo {
        OSAlarm alarm;
        WENCInfo wencinfo;
        const s16* in_pcm;
        int length;
        int seId;
        bool first;
        s8 vol;
        s8 cannotSendCnt;
        u16 pad;
        bool playReady;
    };

    static RemoteSpk* spInstance;

    ChanInfo info[WPAD_MAX_CONTROLLERS];
    OSAlarm speakerAlarm;
    ARCHandle handle;
    bool available;

public:
    static void SetInstance(RemoteSpk* pThis);
    static RemoteSpk* GetInstance();

    RemoteSpk(void* pSpkSeBuf);
    virtual ~RemoteSpk();

    bool isPlayReady(s32 chan) const;
    bool isPlaying(s32 chan) const;
    bool isPlayingId(s32 chan, int seId) const;

    void GetPCMFromSeID(int id, s16*& rpPcm, int& rLength);
    void ClearPcm();
    void Start();
    void Stop();
    void Connect(s32 chan);
    void Play(s32 chan, int seId, s8 vol);

private:
    static void UpdateSpeaker(OSAlarm* pAlarm, OSContext* pContext);
    static void SpeakerOnCallback(s32 chan, s32 result);
    static void DelaySpeakerOnCallback(OSAlarm* pAlarm, OSContext* pContext);
    static void SpeakerPlayCallback(s32 chan, s32 result);
    static void DelaySpeakerPlayCallback(OSAlarm* pAlarm, OSContext* pContext);
};

}

#endif
