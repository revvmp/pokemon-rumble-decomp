#include <game/script/CPpuEngine.h>
#include <game/pokemon/CPii.h>
#include <game/world/CNullObj.h>
#include <game/world/BWorldCamera.h>
#include <game/world/BWorldLightSet.h>

int method_BModelObj_getCurrentMPID(NPpu::CEngine* pEngine);
int method_BModelObj_getMotionLength(NPpu::CEngine* pEngine);
int method_BModelObj_lengthPlay(NPpu::CEngine* pEngine);
int method_BModelObj_lengthRequest(NPpu::CEngine* pEngine);
int method_BModelObj_lengthRequestNext(NPpu::CEngine* pEngine);
int method_BModelObj_mpGetCurrentFrame(NPpu::CEngine* pEngine);
int method_BModelObj_mpGetLength(NPpu::CEngine* pEngine);
int method_BModelObj_mpGetLoopCount(NPpu::CEngine* pEngine);
int method_BModelObj_mpGetSpeed(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsCompleted(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsPausing(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsPlaying(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine);
int method_BModelObj_mpIsStarted(NPpu::CEngine* pEngine);
int method_BModelObj_mpPause(NPpu::CEngine* pEngine);
int method_BModelObj_mpResume(NPpu::CEngine* pEngine);
int method_BModelObj_pause(NPpu::CEngine* pEngine);
int method_BModelObj_play(NPpu::CEngine* pEngine);
int method_BModelObj_playEx(NPpu::CEngine* pEngine);
int method_BModelObj_request(NPpu::CEngine* pEngine);
int method_BModelObj_requestEx(NPpu::CEngine* pEngine);
int method_BModelObj_requestNext(NPpu::CEngine* pEngine);
int method_BModelObj_requestNextEx(NPpu::CEngine* pEngine);
int method_BModelObj_resume(NPpu::CEngine* pEngine);
int method_BModelObj_stop(NPpu::CEngine* pEngine);
int method_BWorldCamera_getCurrentMPID(NPpu::CEngine* pEngine);
int method_BWorldCamera_getMotionLength(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpGetCurrentFrame(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpGetLength(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpGetLoopCount(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpGetSpeed(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsCompleted(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsPausing(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsPlaying(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpIsStarted(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpPause(NPpu::CEngine* pEngine);
int method_BWorldCamera_mpResume(NPpu::CEngine* pEngine);
int method_BWorldCamera_pause(NPpu::CEngine* pEngine);
int method_BWorldCamera_play(NPpu::CEngine* pEngine);
int method_BWorldCamera_playEx(NPpu::CEngine* pEngine);
int method_BWorldCamera_request(NPpu::CEngine* pEngine);
int method_BWorldCamera_requestEx(NPpu::CEngine* pEngine);
int method_BWorldCamera_requestNext(NPpu::CEngine* pEngine);
int method_BWorldCamera_requestNextEx(NPpu::CEngine* pEngine);
int method_BWorldCamera_resume(NPpu::CEngine* pEngine);
int method_BWorldCamera_stop(NPpu::CEngine* pEngine);
int method_BWorldLightSet_getCurrentMPID(NPpu::CEngine* pEngine);
int method_BWorldLightSet_getMotionLength(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpGetCurrentFrame(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpGetLength(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpGetLoopCount(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpGetSpeed(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsCompleted(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsPausing(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsPlaying(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpIsStarted(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpPause(NPpu::CEngine* pEngine);
int method_BWorldLightSet_mpResume(NPpu::CEngine* pEngine);
int method_BWorldLightSet_pause(NPpu::CEngine* pEngine);
int method_BWorldLightSet_play(NPpu::CEngine* pEngine);
int method_BWorldLightSet_playEx(NPpu::CEngine* pEngine);
int method_BWorldLightSet_request(NPpu::CEngine* pEngine);
int method_BWorldLightSet_requestEx(NPpu::CEngine* pEngine);
int method_BWorldLightSet_requestNext(NPpu::CEngine* pEngine);
int method_BWorldLightSet_requestNextEx(NPpu::CEngine* pEngine);
int method_BWorldLightSet_resume(NPpu::CEngine* pEngine);
int method_BWorldLightSet_stop(NPpu::CEngine* pEngine);
int method_CNullObj_getCurrentMPID(NPpu::CEngine* pEngine);
int method_CNullObj_getMotionLength(NPpu::CEngine* pEngine);
int method_CNullObj_mpGetCurrentFrame(NPpu::CEngine* pEngine);
int method_CNullObj_mpGetLength(NPpu::CEngine* pEngine);
int method_CNullObj_mpGetLoopCount(NPpu::CEngine* pEngine);
int method_CNullObj_mpGetSpeed(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsCompleted(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsPausing(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsPlaying(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine);
int method_CNullObj_mpIsStarted(NPpu::CEngine* pEngine);
int method_CNullObj_mpPause(NPpu::CEngine* pEngine);
int method_CNullObj_mpResume(NPpu::CEngine* pEngine);
int method_CNullObj_pause(NPpu::CEngine* pEngine);
int method_CNullObj_play(NPpu::CEngine* pEngine);
int method_CNullObj_playEx(NPpu::CEngine* pEngine);
int method_CNullObj_request(NPpu::CEngine* pEngine);
int method_CNullObj_requestEx(NPpu::CEngine* pEngine);
int method_CNullObj_requestNext(NPpu::CEngine* pEngine);
int method_CNullObj_requestNextEx(NPpu::CEngine* pEngine);
int method_CNullObj_resume(NPpu::CEngine* pEngine);
int method_CNullObj_stop(NPpu::CEngine* pEngine);
int method_CPii_getMotionLength(NPpu::CEngine* pEngine);
int method_CPii_lengthPlay(NPpu::CEngine* pEngine);
int method_CPii_lengthRequest(NPpu::CEngine* pEngine);
int method_CPii_lengthRequestNext(NPpu::CEngine* pEngine);
int method_CPii_play(NPpu::CEngine* pEngine);
int method_CPii_playEx(NPpu::CEngine* pEngine);
int method_CPii_request(NPpu::CEngine* pEngine);
int method_CPii_requestEx(NPpu::CEngine* pEngine);
int method_CPii_requestNext(NPpu::CEngine* pEngine);
int method_CPii_requestNextEx(NPpu::CEngine* pEngine);

int method_play(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & CPii::TYPE) {
        return method_CPii_play(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_play(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_play(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_play(pEngine);
    }
    return method_BModelObj_play(pEngine);
}

int method_playEx(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_playEx(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_playEx(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_playEx(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_playEx(pEngine);
    }
    return method_BModelObj_playEx(pEngine);
}

int method_lengthPlay(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_lengthPlay(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return 0;
    }
    if (type & CNullObj::TYPE) {
        return 0;
    }
    if (type & BWorldLightSet::TYPE) {
        return 0;
    }
    return method_BModelObj_lengthPlay(pEngine);
}

int method_request(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & CPii::TYPE) {
        return method_CPii_request(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_request(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_request(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_request(pEngine);
    }
    return method_BModelObj_request(pEngine);
}

int method_requestEx(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_requestEx(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_requestEx(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_requestEx(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_requestEx(pEngine);
    }
    return method_BModelObj_requestEx(pEngine);
}

int method_lengthRequest(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_lengthRequest(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return 0;
    }
    if (type & CNullObj::TYPE) {
        return 0;
    }
    if (type & BWorldLightSet::TYPE) {
        return 0;
    }
    return method_BModelObj_lengthRequest(pEngine);
}

int method_requestNext(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & CPii::TYPE) {
        return method_CPii_requestNext(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_requestNext(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_requestNext(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_requestNext(pEngine);
    }
    return method_BModelObj_requestNext(pEngine);
}

int method_requestNextEx(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_requestNextEx(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_requestNextEx(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_requestNextEx(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_requestNextEx(pEngine);
    }
    return method_BModelObj_requestNextEx(pEngine);
}

int method_lengthRequestNext(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(3);
    if (type & CPii::TYPE) {
        return method_CPii_lengthRequestNext(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return 0;
    }
    if (type & CNullObj::TYPE) {
        return 0;
    }
    if (type & BWorldLightSet::TYPE) {
        return 0;
    }
    return method_BModelObj_lengthRequestNext(pEngine);
}

int method_getMotionLength(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & CPii::TYPE) {
        return method_CPii_getMotionLength(pEngine);
    }
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_getMotionLength(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_getMotionLength(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_getMotionLength(pEngine);
    }
    return method_BModelObj_getMotionLength(pEngine);
}

int method_pause(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(0);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_pause(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_pause(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_pause(pEngine);
    }
    return method_BModelObj_pause(pEngine);
}

int method_resume(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(0);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_resume(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_resume(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_resume(pEngine);
    }
    return method_BModelObj_resume(pEngine);
}

int method_stop(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(0);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_stop(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_stop(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_stop(pEngine);
    }
    return method_BModelObj_stop(pEngine);
}

int method_getCurrentMPID(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(0);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_getCurrentMPID(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_getCurrentMPID(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_getCurrentMPID(pEngine);
    }
    return method_BModelObj_getCurrentMPID(pEngine);
}

int method_mpIsStarted(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsStarted(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsStarted(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsStarted(pEngine);
    }
    return method_BModelObj_mpIsStarted(pEngine);
}

int method_mpIsPlaying(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsPlaying(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsPlaying(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsPlaying(pEngine);
    }
    return method_BModelObj_mpIsPlaying(pEngine);
}

int method_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsPlayingNotYetLooped(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsPlayingNotYetLooped(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsPlayingNotYetLooped(pEngine);
    }
    return method_BModelObj_mpIsPlayingNotYetLooped(pEngine);
}

int method_mpIsCompleted(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsCompleted(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsCompleted(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsCompleted(pEngine);
    }
    return method_BModelObj_mpIsCompleted(pEngine);
}

int method_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsCompletedOrIsLooped(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsCompletedOrIsLooped(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsCompletedOrIsLooped(pEngine);
    }
    return method_BModelObj_mpIsCompletedOrIsLooped(pEngine);
}

int method_mpGetCurrentFrame(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpGetCurrentFrame(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpGetCurrentFrame(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpGetCurrentFrame(pEngine);
    }
    return method_BModelObj_mpGetCurrentFrame(pEngine);
}

int method_mpGetLength(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpGetLength(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpGetLength(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpGetLength(pEngine);
    }
    return method_BModelObj_mpGetLength(pEngine);
}

int method_mpGetLoopCount(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpGetLoopCount(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpGetLoopCount(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpGetLoopCount(pEngine);
    }
    return method_BModelObj_mpGetLoopCount(pEngine);
}

int method_mpGetSpeed(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpGetSpeed(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpGetSpeed(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpGetSpeed(pEngine);
    }
    return method_BModelObj_mpGetSpeed(pEngine);
}

int method_mpPause(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpPause(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpPause(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpPause(pEngine);
    }
    return method_BModelObj_mpPause(pEngine);
}

int method_mpResume(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpResume(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpResume(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpResume(pEngine);
    }
    return method_BModelObj_mpResume(pEngine);
}

int method_mpIsPausing(NPpu::CEngine* pEngine) {
    u32 type = static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1);
    if (type & BWorldCamera::TYPE) {
        return method_BWorldCamera_mpIsPausing(pEngine);
    }
    if (type & CNullObj::TYPE) {
        return method_CNullObj_mpIsPausing(pEngine);
    }
    if (type & BWorldLightSet::TYPE) {
        return method_BWorldLightSet_mpIsPausing(pEngine);
    }
    return method_BModelObj_mpIsPausing(pEngine);
}
