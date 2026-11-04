#ifndef NW4R_SND_FX_CHORUS_H
#define NW4R_SND_FX_CHORUS_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_AxfxImpl.h>
#include <nw4r/snd/snd_FxBase.h>

#include <revolution/ax.h>
#include <revolution/axfx.h>

namespace nw4r {
namespace snd {
namespace detail {

struct FxChorusParam {
#if !defined(GAMEUTIL_STATIC_FX_PRESETS)
    FxChorusParam()
        : delayTime(10.0f),
          depth(0.5f),
          rate(1.0f),
          feedback(0.0f),
          outGain(1.0f) {}
#endif

    f32 delayTime;
    f32 depth;
    f32 rate;
    f32 feedback;
    f32 outGain;
};

}

class FxChorus : public FxBase {
public:
    FxChorus();

    virtual ~FxChorus() {
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
    bool SetParam(const detail::FxChorusParam& rParam);

private:
    bool mIsActive;
    detail::AxfxImpl mImpl;
    detail::FxChorusParam mParam;
    AXFX_CHORUS_EXP mAxfxParam;
};

}
}

#endif
