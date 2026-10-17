#ifndef NW4R_SND_FX_REVERB_STD_H
#define NW4R_SND_FX_REVERB_STD_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_AxfxImpl.h>
#include <nw4r/snd/snd_FxBase.h>

#include <revolution/ax.h>
#include <revolution/axfx.h>

namespace nw4r {
namespace snd {
namespace detail {

struct FxReverbStdParam {
    FxReverbStdParam()
        : preDelayTime(0.02f),
          fusedTime(3.0f),
          coloration(0.6f),
          damping(0.4f),
          outGain(1.0f),
          earlyMode(5),
          preDelayTimeMax(0.02f),
          fusedMode(0),
          earlyGain(0.0f),
          fusedGain(1.0f) {}

    f32 preDelayTime;
    f32 fusedTime;
    f32 coloration;
    f32 damping;
    f32 outGain;
    u32 earlyMode;
    f32 preDelayTimeMax;
    u32 fusedMode;
    f32 earlyGain;
    f32 fusedGain;
};

}

class FxReverbStd : public FxBase {
public:
    FxReverbStd();

    virtual ~FxReverbStd() {
        Shutdown();
        ReleaseWorkBuffer();
    }

    virtual bool StartUp();
    virtual void Shutdown();

    virtual void UpdateBuffer(int channels, void** ppBuffer, u32 size,
                              SampleFormat format, f32 sampleRate,
                              OutputMode mode);

    virtual bool AssignWorkBuffer(void* pBuffer, u32 size);
    virtual void ReleaseWorkBuffer();

    u32 GetRequiredMemSize();
    bool SetParam(const detail::FxReverbStdParam& rParam);

private:
    bool mIsActive;
    detail::AxfxImpl mImpl;
    detail::FxReverbStdParam mParam;
    AXFX_REVERBSTD_EXP mAxfxParam;
};

}
}

#endif
