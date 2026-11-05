#include <game/world/BWorldCamera.h>
#include <game/script/CPpuEngine.h>

struct CameraMotionArgs {
    const char* mpName;
    f32 mStartFrame;
    f32 mRate;
};

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

static inline CameraMotionArgs PopCameraMotionArgs(NPpu::CEngine* pEngine, f32 startFrame = 0.0f) {
    CameraMotionArgs args;
    args.mRate = 1.0f;
    args.mStartFrame = startFrame;
    args.mpName = PopString(pEngine);
    return args;
}

static inline CameraMotionArgs PopCameraMotionArgsEx(NPpu::CEngine* pEngine) {
    CameraMotionArgs args;
    args.mRate = pEngine->PopFloat();
    args.mStartFrame = pEngine->PopFloat();
    args.mpName = PopString(pEngine);
    return args;
}

int method_BWorldCamera_play(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CameraMotionArgs args = PopCameraMotionArgs(pBaseEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_playEx(NPpu::CEngine* pEngine) {
    CameraMotionArgs args = PopCameraMotionArgsEx(pEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_request(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CameraMotionArgs args = PopCameraMotionArgs(pBaseEngine, -1.0f);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_requestEx(NPpu::CEngine* pEngine) {
    CameraMotionArgs args = PopCameraMotionArgsEx(pEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_requestNext(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CameraMotionArgs args = PopCameraMotionArgs(pBaseEngine, -1.0f);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_requestNextEx(NPpu::CEngine* pEngine) {
    CameraMotionArgs args = PopCameraMotionArgsEx(pEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_getMotionLength(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::CValue value;
    pBaseEngine->Pop(value);
    const char* pName = pBaseEngine->ToString(value);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    pEngine->PushInt(pCamera != nullptr ? pCamera->GetMotionLength(pName) : 0);
    return 0;
}

int method_BWorldCamera_pause(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        pCamera->Pause(true);
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_resume(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        pCamera->Pause(false);
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_stop(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(pEngine->PopWorldObj());
    if (pCamera != nullptr) {
        pCamera->Stop();
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_getCurrentMPID(NPpu::CEngine* pEngine) {
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int id = 0;
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->GetCurrentMotion();
        if (motion) { id = motion->GetId(); }
    }
    pEngine->PushInt(id);
    return 0;
}

BWorldCamera::MotionPtr PopCameraMotion(NPpu::CEngine* pEngine) {
    u32 id = pEngine->PopInt();
    BWorldCamera* pCamera = DynamicCast<BWorldCamera>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pCamera != nullptr) {
        BWorldCamera::MotionPtr motion = pCamera->FindMotion(id);
        if (motion) { return motion; }
    }
    return BWorldCamera::MotionPtr();
}

int method_BWorldCamera_mpIsStarted(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsStarted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldCamera_mpIsPlaying(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlaying()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_BWorldCamera_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlayingNotYetLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldCamera_mpIsCompleted(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompleted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldCamera_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompletedOrLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldCamera_mpGetCurrentFrame(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetFrame()); }
    else { pEngine->PushFloat(0.0f); }
    return 0;
}

int method_BWorldCamera_mpGetLength(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLength()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int method_BWorldCamera_mpGetLoopCount(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLoopCount()); }
    else { pEngine->PushInt(1); }
    return 0;
}

int method_BWorldCamera_mpGetSpeed(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetSpeed()); }
    else { pEngine->PushFloat(1.0f); }
    return 0;
}

int method_BWorldCamera_mpPause(NPpu::CEngine* pEngine) {
    {
        BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
        if (motion) { motion->Pause(true); }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_mpResume(NPpu::CEngine* pEngine) {
    {
        BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
        if (motion) { motion->Pause(false); }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldCamera_mpIsPausing(NPpu::CEngine* pEngine) {
    BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPausing()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_BWorldCamera_mpSetIgnoreAimMode(NPpu::CEngine* pEngine) {
    bool ignore = pEngine->PopInt() != 0;
    {
        BWorldCamera::MotionPtr motion = PopCameraMotion(pEngine);
        if (motion) { motion->SetIgnoreAimMode(ignore); }
    }
    pEngine->PushNull();
    return 0;
}

int DEBUG_IsDropRateUp(NPpu::CEngine* pEngine) {
    pEngine->PushInt(false);
    return 0;
}

int DEBUG_IsCriticalRateUp(NPpu::CEngine* pEngine) {
    pEngine->PushInt(false);
    return 0;
}

int DEBUG_IsNormalDropOn(NPpu::CEngine* pEngine) {
    pEngine->PushInt(true);
    return 0;
}

int DEBUG_IsCriticalOn(NPpu::CEngine* pEngine) {
    pEngine->PushInt(true);
    return 0;
}

int DEBUG_IsSoundTest(NPpu::CEngine* pEngine) {
    pEngine->PushInt(false);
    return 0;
}

