#ifndef NW4R_SND_FX_BASE_H
#define NW4R_SND_FX_BASE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Types.h>

#include <nw4r/ut/ut_LinkList.h>
#include <nw4r/ut/ut_NonCopyable.h>

namespace nw4r {
namespace snd {

enum SampleFormat {
    SAMPLE_FORMAT_PCM_S32,
    SAMPLE_FORMAT_PCM_S16,
    SAMPLE_FORMAT_PCM_S8,
    SAMPLE_FORMAT_DSP_ADPCM
};

class FxBase : private ut::NonCopyable {
public:
    virtual ~FxBase() {}

    virtual bool StartUp() {
        return true;
    }

    virtual void Shutdown() {}

    virtual void UpdateBuffer(int channels, void** ppBuffer, u32 size,
                              SampleFormat format, f32 sampleRate,
                              OutputMode mode) {}

    virtual void OnChangeOutputMode() {}

public:
    NW4R_UT_LINKLIST_NODE_DECL();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(FxBase);

}
}

#endif
