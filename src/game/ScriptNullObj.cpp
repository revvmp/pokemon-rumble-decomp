#include <game/CNullObj.h>
#include <game/CPpuEngine.h>

struct NullObjMotionArgs {
    const char* mpName;
    f32 mStartFrame;
    f32 mRate;
};

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

static inline NullObjMotionArgs PopNullObjMotionArgs(NPpu::CEngine* pEngine, f32 startFrame = 0.0f) {
    NullObjMotionArgs args;
    args.mRate = 1.0f;
    args.mStartFrame = startFrame;
    args.mpName = PopString(pEngine);
    return args;
}

static inline NullObjMotionArgs PopNullObjMotionArgsEx(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args;
    args.mRate = pEngine->PopFloat();
    args.mStartFrame = pEngine->PopFloat();
    args.mpName = PopString(pEngine);
    return args;
}

int method_CNullObj_play(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgs(pEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_playEx(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgsEx(pEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_request(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgs(pEngine, -1.0f);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_requestEx(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgsEx(pEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->Request(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_requestNext(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgs(pEngine, -1.0f);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_requestNextEx(NPpu::CEngine* pEngine) {
    NullObjMotionArgs args = PopNullObjMotionArgsEx(pEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->RequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_getMotionLength(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::CValue value;
    pEngine->Pop(value);
    const char* pName = pEngine->ToString(value);
    CNullObj* pObject = DynamicCast<CNullObj>(pEngine->PopWorldObj());
    pEngine->PushInt(pObject != nullptr ? pObject->GetMotionLength(pName) : 0);
    return 0;
}

int method_CNullObj_pause(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        pObject->Pause(true);
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_resume(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        pObject->Pause(false);
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_stop(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        pObject->Stop();
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_getCurrentMPID(NPpu::CEngine* pEngine) {
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int id = 0;
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->GetCurrentMotion();
        if (motion) { id = motion->GetId(); }
    }
    pEngine->PushInt(id);
    return 0;
}

CNullObj::MotionPtr PopNullObjMotion(NPpu::CEngine* pEngine) {
    u32 id = pEngine->PopInt();
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        CNullObj::MotionPtr motion = pObject->FindMotion(id);
        if (motion) { return motion; }
    }
    return CNullObj::MotionPtr();
}

int method_CNullObj_mpIsStarted(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsStarted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_CNullObj_mpIsPlaying(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlaying()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_CNullObj_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlayingNotYetLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_CNullObj_mpIsCompleted(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompleted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_CNullObj_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompletedOrLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_CNullObj_mpGetCurrentFrame(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetFrame()); }
    else { pEngine->PushFloat(0.0f); }
    return 0;
}

int method_CNullObj_mpGetLength(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLength()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int method_CNullObj_mpGetLoopCount(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLoopCount()); }
    else { pEngine->PushInt(1); }
    return 0;
}

int method_CNullObj_mpGetSpeed(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetSpeed()); }
    else { pEngine->PushFloat(1.0f); }
    return 0;
}

int method_CNullObj_mpPause(NPpu::CEngine* pEngine) {
    {
        CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
        if (motion) { motion->Pause(true); }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_mpResume(NPpu::CEngine* pEngine) {
    {
        CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
        if (motion) { motion->Pause(false); }
    }
    pEngine->PushNull();
    return 0;
}

int method_CNullObj_mpIsPausing(NPpu::CEngine* pEngine) {
    CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPausing()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_CNullObj_mpSetIgnoreAimMode(NPpu::CEngine* pEngine) {
    bool ignore = pEngine->PopInt() != 0;
    {
        CNullObj::MotionPtr motion = PopNullObjMotion(pEngine);
        if (motion) { motion->SetIgnoreAimMode(ignore); }
    }
    pEngine->PushNull();
    return 0;
}
