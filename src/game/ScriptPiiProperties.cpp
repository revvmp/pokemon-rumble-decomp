#include <game/CPpuEngine.h>
#include <game/CPiiPersonalData.h>

int method_getPrice(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetPrice() : 0);
    return 0;
}

int method_setPrice(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetPrice(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getGroupNo(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetGroupNo() : 0);
    return 0;
}

int method_setGroupNo(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetGroupNo(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getPrefix(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetPrefix() : 0);
    return 0;
}

int method_setPrefix(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetPrefix(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_setBossFlag(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBoss(value != 0);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBossFlag(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsBoss() : false);
    return 0;
}

int method_setBossHideHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBossHideHP(value != 0);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBossHideHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsBossHideHP() : false);
    return 0;
}

int method_isSelling(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsSelling() : false);
    return 0;
}

int method_isNewlyDisclosed(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsNewlyDisclosed() : false);
    return 0;
}

int method_ppdGetRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int index = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        pEngine->Push(data->GetRegister()->GetValue(index));
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_ppdSetRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::Slice value;
    pEngine->Pop(value);
    int index = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->GetRegister()->Set(index, value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getOyaID(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetOriginalTrainerId() : 0);
    return 0;
}

int method_setOyaID(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetOriginalTrainerId(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getRnd(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetRandomSeed() : 0);
    return 0;
}

int method_setRnd(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetRandomSeed(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_isRareColor(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsRareColor() : false);
    return 0;
}

int method_setRareColorRnd(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetRareColorRnd();
    }
    pEngine->PushNull();
    return 0;
}

int method_setNormalColorRnd(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetNormalColorRnd();
    }
    pEngine->PushNull();
    return 0;
}

int method_setIsSpecHidden(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetSpecHidden(value != 0);
    }
    pEngine->PushNull();
    return 0;
}

int method_isSpecHidden(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsSpecHidden() : false);
    return 0;
}

int method_isInWiiRemote(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->IsInWiiRemote() : false);
    return 0;
}

