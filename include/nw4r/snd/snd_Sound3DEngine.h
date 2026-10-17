#ifndef NW4R_SND_SOUND_3D_ENGINE_H
#define NW4R_SND_SOUND_3D_ENGINE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Sound3DCalculator.h>

namespace nw4r {
namespace snd {

class Sound3DManager;
struct Sound3DParam;
struct SoundAmbientParam;

class Sound3DEngine {
public:
    enum UpdateFlag {
        UPDATE_VOLUME = (1 << 0),
        UPDATE_PAN = (1 << 1),
        UPDATE_SURROUND_PAN = (1 << 2),
        UPDATE_PRIORITY = (1 << 3),
        UPDATE_PITCH = (1 << 4),
        UPDATE_BIQUAD_FILTER = (1 << 5),
        UPDATE_START_PRIORITY = (1 << 24)
    };

    Sound3DEngine();
    virtual ~Sound3DEngine() {}

    virtual void UpdateAmbientParam(const Sound3DManager* pManager,
                                    const Sound3DParam* pParam, u32 soundId,
                                    int voiceOutCount,
                                    SoundAmbientParam* pAmbientParam);
    virtual int GetAmbientPriority(const Sound3DManager* pManager,
                                   const Sound3DParam* pParam, u32 soundId);
    virtual int GetRequiredVoiceOutCount(const Sound3DManager* pManager,
                                         const Sound3DParam* pParam,
                                         u32 soundId);

protected:
    virtual void UpdateAmbientParam(const Sound3DManager* pManager,
                                    const Sound3DParam* pParam, u32 soundId,
                                    u32 updateFlag,
                                    SoundAmbientParam* pAmbientParam);

private:
    Sound3DCalculator::CalcPanParam mCalcPanParam;
};

}
}

#endif
