#ifndef NW4R_SND_FX_DELAY_H
#define NW4R_SND_FX_DELAY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_AxfxImpl.h>
#include <nw4r/snd/snd_FxBase.h>

#include <revolution/ax.h>
#include <revolution/axfx.h>

namespace nw4r {
namespace snd {
namespace detail {

struct FxDelayParam {
#if !defined(GAMEUTIL_STATIC_FX_PRESETS)
    FxDelayParam()
        : delay(160.0f),
          feedback(0.4f),
          outGain(1.0f),
          maxDelay(160.0f),
          lpf(1.0f) {}
#endif

    f32 delay;
    f32 feedback;
    f32 outGain;
    f32 maxDelay;
    f32 lpf;
};

}

class FxDelay : public FxBase {
public:
    FxDelay();

    virtual ~FxDelay() {
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
    bool SetParam(const detail::FxDelayParam& rParam);

private:
    bool mIsActive;
    detail::AxfxImpl mImpl;
    detail::FxDelayParam mParam;
    AXFX_DELAY_EXP mAxfxParam;
};

}
}

#endif
