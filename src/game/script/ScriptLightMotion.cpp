#include <game/world/BWorldLightSet.h>
#include <game/script/CPpuEngine.h>





































struct LightMotionArgs {
    const char* mpName;
    f32 mStartFrame;
    f32 mRate;
};

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

static inline LightMotionArgs PopLightMotionArgs(NPpu::CEngine* pEngine, f32 start = 0.0f) {
    LightMotionArgs args;
    args.mRate = 1.0f;
    args.mStartFrame = start;
    args.mpName = PopString(pEngine);
    return args;
}

static inline LightMotionArgs PopLightMotionArgsEx(NPpu::CEngine* pEngine) {
    LightMotionArgs args;
    args.mRate = pEngine->PopFloat();
    args.mStartFrame = pEngine->PopFloat();
    args.mpName = PopString(pEngine);
    return args;
}

int method_BWorldLightSet_play(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    LightMotionArgs args = PopLightMotionArgs(pBaseEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_playEx(NPpu::CEngine* pEngine) {
    LightMotionArgs args = PopLightMotionArgsEx(pEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_request(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    LightMotionArgs args = PopLightMotionArgs(pBaseEngine, -1.0f);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_requestEx(NPpu::CEngine* pEngine) {
    LightMotionArgs args = PopLightMotionArgsEx(pEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_requestNext(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    LightMotionArgs args = PopLightMotionArgs(pBaseEngine, -1.0f);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_requestNextEx(NPpu::CEngine* pEngine) {
    LightMotionArgs args = PopLightMotionArgsEx(pEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_getMotionLength(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const char* pName = PopString(pBaseEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    pEngine->PushInt(pLightSet != nullptr ? pLightSet->GetMotionLength(pName) : 0);
    return 0;
}

int method_BWorldLightSet_pause(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        pLightSet->Pause(true);
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_resume(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        pLightSet->Pause(false);
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_stop(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(pEngine->PopWorldObj());
    if (pLightSet != nullptr) {
        pLightSet->Stop();
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_getCurrentMPID(NPpu::CEngine* pEngine) {
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int id = 0;
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->GetCurrentMotion();
        if (motion) { id = motion->GetId(); }
    }
    pEngine->PushInt(id);
    return 0;
}

static BWorldLightSet::MotionPtr PopLightMotion(NPpu::CEngine* pEngine) {
    u32 id = pEngine->PopInt();
    BWorldLightSet* pLightSet = DynamicCast<BWorldLightSet>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pLightSet != nullptr) {
        BWorldLightSet::MotionPtr motion = pLightSet->FindMotion(id);
        if (motion) { return motion; }
    }
    return BWorldLightSet::MotionPtr();
}

int method_BWorldLightSet_mpIsStarted(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsStarted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldLightSet_mpIsPlaying(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlaying()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_BWorldLightSet_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlayingNotYetLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldLightSet_mpIsCompleted(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompleted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldLightSet_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompletedOrLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BWorldLightSet_mpGetCurrentFrame(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetFrame()); }
    else { pEngine->PushFloat(0.0f); }
    return 0;
}

int method_BWorldLightSet_mpGetLength(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLength()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int method_BWorldLightSet_mpGetLoopCount(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLoopCount()); }
    else { pEngine->PushInt(1); }
    return 0;
}

int method_BWorldLightSet_mpGetSpeed(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetSpeed()); }
    else { pEngine->PushFloat(1.0f); }
    return 0;
}

int method_BWorldLightSet_mpPause(NPpu::CEngine* pEngine) {
    {
        BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
        if (motion) { motion->Pause(true); }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_mpResume(NPpu::CEngine* pEngine) {
    {
        BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
        if (motion) { motion->Pause(false); }
    }
    pEngine->PushNull();
    return 0;
}

int method_BWorldLightSet_mpIsPausing(NPpu::CEngine* pEngine) {
    BWorldLightSet::MotionPtr motion = PopLightMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPausing()); }
    else { pEngine->PushInt(false); }
    return 0;
}
