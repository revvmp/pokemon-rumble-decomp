#ifndef NW4R_SND_BASIC_PLAYER_H
#define NW4R_SND_BASIC_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Types.h>

#include <revolution/wpad.h>

namespace nw4r {
namespace snd {
namespace detail {

struct PlayerParamSet {
    PlayerParamSet() {
        Init();
    }

    void Init();

    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 lpfFreq;
    f32 biquadValue;
    u8 biquadType;
    u8 remoteFilter;
    int outputLineFlag;
    f32 mainOutVolume;
    f32 mainSend;
    PanMode panMode;
    PanCurve panCurve;
    f32 fxSend[AUX_BUS_NUM];
    f32 remoteOutVolume[WPAD_MAX_CONTROLLERS];
    VoiceOutParam voiceOutParam[WPAD_MAX_CONTROLLERS];
};

class BasicPlayer {
public:
    BasicPlayer();
    virtual ~BasicPlayer() {}

    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual void Pause(bool flag) = 0;
    virtual bool IsActive() const = 0;
    virtual bool IsStarted() const = 0;
    virtual bool IsPause() const = 0;

    void InitParam();

    u32 GetId() const {
        return mId;
    }
    void SetId(u32 id) {
        mId = id;
    }

    f32 GetVolume() const {
        return mPlayerParamSet.volume;
    }
    void SetVolume(f32 volume) {
        mPlayerParamSet.volume = volume;
    }

    f32 GetPitch() const {
        return mPlayerParamSet.pitch;
    }
    void SetPitch(f32 pitch) {
        mPlayerParamSet.pitch = pitch;
    }

    f32 GetPan() const {
        return mPlayerParamSet.pan;
    }
    void SetPan(f32 pan) {
        mPlayerParamSet.pan = pan;
    }

    f32 GetSurroundPan() const {
        return mPlayerParamSet.surroundPan;
    }
    void SetSurroundPan(f32 pan) {
        mPlayerParamSet.surroundPan = pan;
    }

    f32 GetLpfFreq() const {
        return mPlayerParamSet.lpfFreq;
    }
    void SetLpfFreq(f32 freq) {
        mPlayerParamSet.lpfFreq = freq;
    }

    int GetBiquadType() const {
        return mPlayerParamSet.biquadType;
    }
    f32 GetBiquadValue() const {
        return mPlayerParamSet.biquadValue;
    }
    void SetBiquadFilter(int type, f32 value);

    int GetRemoteFilter() const {
        return mPlayerParamSet.remoteFilter;
    }
    void SetRemoteFilter(int filter);

    int GetOutputLine() const {
        return mPlayerParamSet.outputLineFlag;
    }
    void SetOutputLine(int flags) {
        mPlayerParamSet.outputLineFlag = flags;
    }

    f32 GetMainOutVolume() const {
        return mPlayerParamSet.mainOutVolume;
    }
    void SetMainOutVolume(f32 volume) {
        mPlayerParamSet.mainOutVolume = volume;
    }

    f32 GetMainSend() const {
        return mPlayerParamSet.mainSend;
    }
    void SetMainSend(f32 send) {
        mPlayerParamSet.mainSend = send;
    }

    PanMode GetPanMode() const {
        return mPlayerParamSet.panMode;
    }
    void SetPanMode(PanMode mode) {
        mPlayerParamSet.panMode = mode;
    }

    PanCurve GetPanCurve() const {
        return mPlayerParamSet.panCurve;
    }
    void SetPanCurve(PanCurve curve) {
        mPlayerParamSet.panCurve = curve;
    }

    f32 GetFxSend(AuxBus bus) const;
    void SetFxSend(AuxBus bus, f32 send);

    f32 GetRemoteOutVolume(int remote) const;
    void SetRemoteOutVolume(int remote, f32 volume);

    const VoiceOutParam& GetVoiceOutParam(int remote) const {
        return mPlayerParamSet.voiceOutParam[remote];
    }
    void SetVoiceOutParam(int remote, const VoiceOutParam& rParam) {
        mPlayerParamSet.voiceOutParam[remote] = rParam;
    }

private:
    PlayerParamSet mPlayerParamSet;
    u32 mId;
};

}
}
}

#endif
