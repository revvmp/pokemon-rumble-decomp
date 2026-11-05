#include <game/pokemon/CPii.h>
#include <game/pokemon/CPiiCollectionBox.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <game/pokemon/CPiiBox.h>
#include <game/session/CPlayer.h>
#include <game/script/CPpuEngine.h>
#include <game/script/CPpuEnv.h>
#include <game/session/IGameSession.h>
#include <game/pokemon/PiiSpeciesData.h>

struct MotionArgs {
    int mMotion;
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
    args.mMotion = pEngine->PopInt();
    return args;
}

int method_CPii_play(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, false, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_playEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_lengthPlay(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->PlayMotionForDuration(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_request(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, false, -1.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->QueueMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_requestEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->QueueMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_lengthRequest(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->QueueMotionForDuration(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_requestNext(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, false, -1.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->AppendMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_requestNextEx(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->AppendMotion(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_lengthRequestNext(NPpu::CEngine* pEngine) {
    MotionArgs args = PopMotionArgs(pEngine, true, 0.0f);
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pObject != nullptr) {
        std::tr1::shared_ptr<CModel::CMotionProgram> motion = pObject->AppendMotionForDuration(args.mMotion, args.mStartFrame, args.mRate);
        if (motion) {
            pEngine->PushInt(motion->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_CPii_getMotionLength(NPpu::CEngine* pEngine) {
    int motion = pEngine->PopInt();
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int length;
    if (pObject != nullptr) {
        length = pObject->GetMotionLengthByIndex(motion);
    } else {
        length = 0;
    }
    pEngine->PushInt(length);
    return 0;
}

int method_isPlaying(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    CPii* pObject = DynamicCast<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    int result = 0;
    if (pObject != nullptr) {
        result = pObject->IsPlayingMotion(id);
    }
    pEngine->PushInt(result);
    return 0;
}

int method_getPiiProp(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        pEngine->PushInt(reinterpret_cast<int>(data->GetSpeciesData()));
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_getMonsno(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? static_cast<u16>(data->GetSpeciesData()->GetSpecies()) : 0);
    return 0;
}

int method_getFormno(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? static_cast<u8>(data->GetSpeciesData()->GetFormNumber()) : 0);
    return 0;
}

int method_getType1(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetSpeciesData()->GetPrimaryType() : 0);
    return 0;
}

int method_getType2(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetSpeciesData()->GetSecondaryType() : 0);
    return 0;
}

int method_setCollectionAppeared(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        int species = data->GetSpeciesData()->GetSpecies();
        int form = data->GetSpeciesData()->GetFormNumber();
        IGameSession::GetInstance()->GetPiiCollectionBox()->RecordCollection(species, form, 1, data->IsRareColor());
    }
    pEngine->PushNull();
    return 0;
}

int method_setCollectionDefeated(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        int species = data->GetSpeciesData()->GetSpecies();
        int form = data->GetSpeciesData()->GetFormNumber();
        IGameSession::GetInstance()->GetPiiCollectionBox()->RecordCollection(species, form, 2, data->IsRareColor());
    }
    pEngine->PushNull();
    return 0;
}

int IsAllPiiBoxCapacityFull(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->IsAllPiiBoxCapacityFull());
    return 0;
}

int AddPPDToPlayerPiiBox(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    int player = pEngine->PopInt();
    data->ExecuteInitScript(0, static_cast<CPpuEnv*>(pEngine->GetEnv())->shared_from_this());
    if (!IGameSession::GetInstance()->IsAllPiiBoxCapacityFull()) {
        pEngine->PushInt(IGameSession::GetInstance()->GetPlayer(player)->GetPiiBox().AddPersonalData(data));
    }
    return 0;
}

int AddPPDToTeamPiiBox(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    data->ExecuteInitScript(0, static_cast<CPpuEnv*>(pEngine->GetEnv())->shared_from_this());
    pEngine->PushInt(IGameSession::GetInstance()->GetTeamPiiBox()->AddPersonalData(data));
    CPiiCollectionBox* pBox = IGameSession::GetInstance()->GetPiiCollectionBox();
    int species = data->GetSpeciesData()->GetSpecies();
    int form = data->GetSpeciesData()->GetFormNumber();
    pBox->RecordCollection(species, form, 3, data->IsRareColor());
    return 0;
}

int GetTeamPiiBoxPPD(NPpu::CEngine* pEngine) {
    int index = pEngine->PopInt();
    CPiiBox* pBox = IGameSession::GetInstance()->GetTeamPiiBox();
    if (index >= 0 && index < static_cast<int>(pBox->GetCount())) {
        std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = pBox->GetPersonalDataList().begin();
        std::advance(it, index);
        pEngine->PushInt((*it)->GetId() | 0x80000000);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int GetTeamPiiBoxSize(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->GetTeamPiiBox()->GetCount());
    return 0;
}
