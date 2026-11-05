#include <game/script/CPpuEngine.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <game/pokemon/CPiiCollectionBox.h>
#include <game/session/CPlayer.h>
#include <game/pokemon/CPlayerPii.h>
#include <game/session/IGameSession.h>
#include <game/pokemon/PiiSpeciesData.h>
#include <game/world/BWorldObj.h>

#pragma dont_inline on

int IsPiiChangeRequested(NPpu::CEngine* pEngine) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(pEngine->PopInt());
    pEngine->PushInt(pPlayer->IsPiiChangeRequested());
    return 0;
}

int PiiChangeRequestCompleted(NPpu::CEngine* pEngine) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(pEngine->PopInt());
    pPlayer->CompletePiiChangeRequest();
    pEngine->PushNull();
    return 0;
}

int GetPlayerPii(NPpu::CEngine* pEngine) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(pEngine->PopInt());
    CPlayerPii* pPii = pPlayer->GetPii();
    if (pPii) {
        pEngine->PushInt(pPii->GetId());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int IsPlayerEnabled(NPpu::CEngine* pEngine) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(pEngine->PopInt());
    pEngine->PushInt(pPlayer->IsJoined());
    return 0;
}

int GetPlayerPPD(NPpu::CEngine* pEngine) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(pEngine->PopInt());
    std::tr1::shared_ptr<CPiiPersonalData> data = pPlayer->GetPersonalData();
    if (data) {
        pEngine->PushInt(data->GetId() | 0x80000000);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_incDefeatNum(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CPlayerPii* pPii = DynamicCast<CPlayerPii>(pEngine->PopWorldObj());
    if (pPii && pPii->GetPlayer()) {
        pPii->GetPlayer()->IncrementDefeatCount();
    }
    pBaseEngine->PushNull();
    return 0;
}

#pragma dont_inline reset

int NewPPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int sex = pEngine->PopInt();
    const PiiSpeciesData* pSpecies = pEngine->PopSpeciesData();
    std::tr1::shared_ptr<CPiiPersonalData> data = CreatePiiPersonalData(IGameSession::GetInstance()->GetOyaInfo(), pSpecies, sex);
    data->RerollRandomSeed();
    pEngine->SetPPD(data);
    pBaseEngine->PushInt(data->GetId() | 0x80000000);
    return 0;
}

int method_isCPiiOrPPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? 1 : 0);
    return 0;
}

int method_getPPD(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data->GetId() | 0x80000000);
    return 0;
}

int method_getHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    pEngine->PushInt(data ? data->GetHP() : 0);
    return 0;
}

int method_setHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int hp = pEngine->PopInt();
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->SetHP(hp);
        pEngine->PushInt(hp);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_fillUpHP(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->FillUpHP();
    }
    pEngine->PushNull();
    return 0;
}

int method_resetDamage(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->ResetDamage();
    }
    pEngine->PushNull();
    return 0;
}

int method_resetHeal(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    if (data) {
        data->ResetHeal();
    }
    pEngine->PushNull();
    return 0;
}
