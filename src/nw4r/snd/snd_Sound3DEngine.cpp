#include <nw4r/snd/snd_Sound3DEngine.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <nw4r/snd/snd_Sound3DManager.h>

#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {

Sound3DEngine::Sound3DEngine() {}

void Sound3DEngine::UpdateAmbientParam(const Sound3DManager* pManager,
                                       const Sound3DParam* pParam,
                                       u32 soundId, u32 updateFlag,
                                       SoundAmbientParam* pAmbientParam) {
    if (updateFlag & UPDATE_VOLUME) {
        pAmbientParam->volume = 0.0f;
    }

    if (updateFlag & UPDATE_PRIORITY) {
        pAmbientParam->priority = -pManager->GetMaxPriorityReduction();
    }

    const Sound3DListenerList& rListenerList = pManager->GetListenerList();

    if (rListenerList.GetSize() > 1) {
        updateFlag &= ~(UPDATE_PAN | UPDATE_SURROUND_PAN | UPDATE_PITCH);
    }

    for (Sound3DListenerList::ConstIterator it = rListenerList.GetBeginIter();
         it != rListenerList.GetEndIter(); ++it) {
        const Sound3DListener& rListener = *it;

        if (updateFlag & (UPDATE_VOLUME | UPDATE_PRIORITY)) {
            f32 volume;
            int priority;
            Sound3DCalculator::CalcVolumeAndPriority(
                *pManager, rListener, *pParam, &volume, &priority);

            if (updateFlag & UPDATE_VOLUME) {
                pAmbientParam->volume =
                    ut::Max(volume, pAmbientParam->volume);
            }

            if (updateFlag & UPDATE_PRIORITY) {
                pAmbientParam->priority =
                    ut::Max(priority, pAmbientParam->priority);
            }
        }

        if (updateFlag & (UPDATE_PAN | UPDATE_SURROUND_PAN)) {
            f32 pan;
            f32 surroundPan;
            Sound3DCalculator::CalcPan(*pManager, rListener, *pParam,
                                       mCalcPanParam, &pan, &surroundPan);

            if (updateFlag & UPDATE_PAN) {
                pAmbientParam->pan = pan;
            }

            if (updateFlag & UPDATE_SURROUND_PAN) {
                pAmbientParam->surroundPan = surroundPan;
            }
        }

        if (updateFlag & UPDATE_PITCH) {
            f32 pitch;
            Sound3DCalculator::CalcPitch(*pManager, rListener, *pParam,
                                         &pitch);
            pAmbientParam->pitch = pitch;
        }

        if (updateFlag & UPDATE_BIQUAD_FILTER) {
            f32 biquadFilterValue;
            Sound3DCalculator::CalcBiquadFilterValue(
                *pManager, rListener, *pParam, &biquadFilterValue);

            pAmbientParam->biquadFilterType = pManager->GetBiquadFilterType();
            pAmbientParam->biquadFilterValue =
                ut::Min(biquadFilterValue, pAmbientParam->biquadFilterValue);
        }
    }
}

void Sound3DEngine::UpdateAmbientParam(const Sound3DManager* pManager,
                                       const Sound3DParam* pParam,
                                       u32 soundId, int voiceOutCount,
                                       SoundAmbientParam* pAmbientParam) {
    u32 updateFlag = UPDATE_VOLUME | UPDATE_PAN | UPDATE_SURROUND_PAN |
                     UPDATE_PRIORITY | UPDATE_PITCH;

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_NO_VOLUME) {
        updateFlag &= ~UPDATE_VOLUME;
    }

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_NO_PAN) {
        updateFlag &= ~UPDATE_PAN;
    }

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_NO_SURROUND_PAN) {
        updateFlag &= ~UPDATE_SURROUND_PAN;
    }

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_NO_PRIORITY) {
        updateFlag &= ~UPDATE_PRIORITY;
    }

    if (pParam->dopplerFactor == 0) {
        updateFlag &= ~UPDATE_PITCH;
    }

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_BIQUAD_FILTER) {
        updateFlag |= UPDATE_BIQUAD_FILTER;
    }

    UpdateAmbientParam(pManager, pParam, soundId, updateFlag, pAmbientParam);
}

int Sound3DEngine::GetAmbientPriority(const Sound3DManager* pManager,
                                      const Sound3DParam* pParam,
                                      u32 soundId) {
    u32 updateFlag = UPDATE_START_PRIORITY | UPDATE_PRIORITY;

    if (pParam->ctrlFlag & Sound3DParam::PARAM_CTRL_FLAG_NO_PRIORITY) {
        updateFlag &= ~UPDATE_PRIORITY;
    }

    SoundAmbientParam ambientParam;
    UpdateAmbientParam(pManager, pParam, soundId, updateFlag, &ambientParam);

    return ambientParam.priority;
}

int Sound3DEngine::GetRequiredVoiceOutCount(const Sound3DManager* pManager,
                                            const Sound3DParam* pParam,
                                            u32 soundId) {
    return 1;
}

}
}
