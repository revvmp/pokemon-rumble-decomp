#include <game/BModelObj.h>
#include <game/CPpuEngine.h>
#include <game/CNullObj.h>
#include <game/BWorld.h>

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

struct MotionArgs {
    const char* mpName;
    f32 mStartFrame;
    f32 mRate;
};

static inline MotionArgs PopMotionArgs(NPpu::CEngine* pEngine, bool extended, f32 startFrame) {
    MotionArgs args;
    if (extended) {
        args.mRate = pEngine->PopFloat();
        args.mStartFrame = pEngine->PopFloat();
    } else {
        args.mRate = 1.0f;
        args.mStartFrame = startFrame;
    }
    args.mpName = PopString(pEngine);
    return args;
}

int method_BModelObj_play(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    MotionArgs args = PopMotionArgs(pBaseEngine, false, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_playEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->Play(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_lengthPlay(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->LengthPlay(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_request(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    MotionArgs args = PopMotionArgs(pBaseEngine, false, -1.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayLoop(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_requestEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayLoop(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_lengthRequest(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->LengthRequest(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_requestNext(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    MotionArgs args = PopMotionArgs(pBaseEngine, false, -1.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayOnce(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_requestNextEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayOnce(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_lengthRequestNext(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->LengthRequestNext(args.mpName, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_getMotionLength(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const char* pName = PopString(pBaseEngine);
    BModelObj* pObject = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    int length;
    if (pObject != nullptr) {
        length = pObject->GetMotionLength(pName);
    } else {
        length = 0;
    }
    pEngine->PushInt(length);
    return 0;
}

int method_BModelObj_pause(NPpu::CEngine* pEngine) {
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        pObject->Pause(true);
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_resume(NPpu::CEngine* pEngine) {
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        pObject->Resume();
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_stop(NPpu::CEngine* pEngine) {
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) { pObject->GetModel()->Stop(); }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_getCurrentMPID(NPpu::CEngine* pEngine) {
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int id = 0;
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->GetModel()->GetCurrentMotion();
        if (motion) { id = motion->GetId(); }
    }
    pEngine->PushInt(id);
    return 0;
}

std::tr1::shared_ptr<CModel::CMotionProgram> PopModelMotion(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    BModelObj* pObject = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->GetModel()->FindMotion(id);
        if (motion) { return motion; }
    }
    return std::tr1::shared_ptr<CModel::CMotionProgram>();
}

int method_BModelObj_mpIsStarted(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsStarted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BModelObj_mpIsPlaying(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlaying()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int method_BModelObj_mpIsPlayingNotYetLooped(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPlayingNotYetLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BModelObj_mpIsCompleted(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompleted()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BModelObj_mpIsCompletedOrIsLooped(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsCompletedOrIsLooped()); }
    else { pEngine->PushInt(true); }
    return 0;
}

int method_BModelObj_mpGetCurrentFrame(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetCurrentFrame()); }
    else { pEngine->PushFloat(0.0f); }
    return 0;
}

int method_BModelObj_mpGetLength(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLength()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int method_BModelObj_mpGetLoopCount(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->GetLoopCount()); }
    else { pEngine->PushInt(1); }
    return 0;
}

int method_BModelObj_mpGetSpeed(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushFloat(motion->GetSpeed()); }
    else { pEngine->PushFloat(1.0f); }
    return 0;
}

int method_BModelObj_mpPause(NPpu::CEngine* pEngine) {
    {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
        if (motion) { motion->Pause(); }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_mpResume(NPpu::CEngine* pEngine) {
    {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
        if (motion) { motion->Resume(); }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_BModelObj_mpIsPausing(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CModel::CMotionProgram> motion = PopModelMotion(pEngine);
    if (motion) { pEngine->PushInt(motion->IsPausing()); }
    else { pEngine->PushInt(false); }
    return 0;
}

int NewNullObj(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CNullObj> object(new CNullObj(CNullObj::TYPE));
    object->SwapRegister(static_cast<CPpuEngine*>(pEngine)->GetRegister());
    BWorld* pWorld = static_cast<CPpuEngine*>(pEngine)->GetWorld();
    bool added = pWorld->Add(object);
    if (added) { pEngine->PushInt(object->GetId()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int method_nullSetTarget(NPpu::CEngine* pEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    CNullObj* pObject = DynamicCast<CNullObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        if (pTarget != nullptr) { pObject->SetTarget(pTarget->shared_from_this()); }
        else { pObject->SetTarget(std::tr1::shared_ptr<BWorldObj>()); }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_nullLoadAnm(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const char* pName = PopString(pBaseEngine);
    CNullObj* pObject = DynamicCast<CNullObj>(pEngine->PopWorldObj());
    if (pObject != nullptr && pName != nullptr) {
        std::string name(pName);
        pObject->LoadAnm(("5:" + name).c_str(), false);
    }
    pEngine->PushInt(0);
    return 0;
}
