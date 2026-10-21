#include <game/CPpuEngine.h>
#include <game/CPiiPersonalData.h>

int method_getMaxHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetMaxHP() : 0);
    return 0;
}

int method_setMaxHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetMaxHP(value);
        pEngine->PushInt(value);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_getBonusMaxHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetBonusMaxHP() : 0);
    return 0;
}

int method_setBonusMaxHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBonusMaxHP(value);
        pEngine->PushInt(value);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_setWazaID(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    int button = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        if (button == 32) {
            data->SetWazaID(0, value);
        } else {
            data->SetWazaID(1, value);
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_getWazaID(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int button = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        if (button == 32) {
            pEngine->PushInt(data->GetWazaID(0));
        } else {
            pEngine->PushInt(data->GetWazaID(1));
        }
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_getSex(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetSex() : 2);
    return 0;
}

int method_getLevel(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetLevel() : 0);
    return 0;
}

int method_setLevel(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetLevel(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getCappedLevel(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetCappedLevel() : 0);
    return 0;
}

int method_setCappedLevel(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetCappedLevel(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getAP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetAP() : 0);
    return 0;
}

int method_setAP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetAP(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getDP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetDP() : 0);
    return 0;
}

int method_setDP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetDP(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getSPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetSPD() : 0);
    return 0;
}

int method_setSPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetSPD(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBonusAP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetBonusAP() : 0);
    return 0;
}

int method_setBonusAP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBonusAP(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBonusDP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetBonusDP() : 0);
    return 0;
}

int method_setBonusDP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBonusDP(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBonusSPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetBonusSPD() : 0);
    return 0;
}

int method_setBonusSPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBonusSPD(value);
    }
    pEngine->PushNull();
    return 0;
}

int method_getBP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetBP() : 0);
    return 0;
}

int method_setBP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetBP(value);
    }
    pEngine->PushNull();
    return 0;
}

