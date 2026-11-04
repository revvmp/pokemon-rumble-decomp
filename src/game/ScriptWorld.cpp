#include <game/CPpuEngine.h>
#include <game/CWaza.h>
#include <game/BWorld.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/StageDesc.h>
#include <game/MapDesc.h>
#include <game/Message.h>
#include <game/VideoTiming.h>
#include <game/WazaDescData.h>


int SetStageDescTargetAsCurrent(NPpu::CEngine* pEngine) {
    MapIndex index = GetCurrentGameMap()->GetMapDesc().mIndex;
    static_cast<CPpuEngine*>(pEngine)->SetStageDescTarget(index);
    pEngine->PushNull();
    return 0;
}

#pragma push
#pragma opt_dead_assignments off
int SetStageDescTarget(NPpu::CEngine* pEngine) {
    int world = pEngine->PopInt();
    int rank = pEngine->PopInt();
    int kind = pEngine->PopInt();
    MapIndex target = {kind, rank, world, 0};
    static_cast<CPpuEngine*>(pEngine)->SetStageDescTarget(target);
    pEngine->PushZeroInt();
    return 0;
}
#pragma pop

int SetStageDescParams(NPpu::CEngine* pEngine) {
    int count = pEngine->FetchArgCount();
    StageDesc* pDesc = IGameSession::GetInstance()->GetStageDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    pDesc->mParamTable.Reserve(count);
    for (int i = 0; i < count; ++i) {
        NPpu::CValue value;
        pEngine->GetArg(-1 - (i - count), value);
        pDesc->mParamTable.Set(i, value);
    }
    pEngine->Drop(count);
    pEngine->PushNull();
    return 0;
}

int GetStageDescParam(NPpu::CEngine* pEngine) {
    int index = pEngine->PopInt();
    StageDesc* pDesc = IGameSession::GetInstance()->GetStageDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    pEngine->Push(pDesc->mParamTable.GetValue(index));
    return 0;
}

int SetMapDescTargetAsCurrent(NPpu::CEngine* pEngine) {
    MapIndex index = GetCurrentGameMap()->GetMapDesc().mIndex;
    static_cast<CPpuEngine*>(pEngine)->SetStageDescTarget(index);
    pEngine->PushNull();
    return 0;
}

int SetMapDescTarget(NPpu::CEngine* pEngine) {
    int map = pEngine->PopInt();
    MapIndex target = static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget();
    target.mMap = map;
    static_cast<CPpuEngine*>(pEngine)->SetStageDescTarget(target);
    pEngine->PushNull();
    return 0;
}

int SetMapDescParams(NPpu::CEngine* pEngine) {
    int count = pEngine->FetchArgCount();
    MapDesc* pDesc = IGameSession::GetInstance()->CreateMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    pDesc->mParams.Reserve(count);
    for (int i = 0; i < count; ++i) {
        NPpu::CValue value;
        pEngine->GetArg(-1 - (i - count), value);
        pDesc->mParams.Set(i, value);
    }
    pEngine->Drop(count);
    pEngine->PushNull();
    return 0;
}

int GetMapDescParam(NPpu::CEngine* pEngine) {
    int index = pEngine->PopInt();
    MapDesc* pDesc = IGameSession::GetInstance()->GetMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    if (pDesc != nullptr) {
        pEngine->Push(pDesc->mParams.GetValue(index));
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int InitMapDescEnemies(NPpu::CEngine* pEngine) {
    int columns = pEngine->PopInt();
    int rows = pEngine->PopInt();
    MapDesc* pDesc = IGameSession::GetInstance()->CreateMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    pDesc->mPiiTable.Initialize(rows, columns);
    pEngine->PushNull();
    return 0;
}

int SetMapDescEnemyRow(NPpu::CEngine* pEngine) {
    int count = pEngine->FetchArgCount();
    MapDesc* pDesc = IGameSession::GetInstance()->CreateMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    NPpu::CValue value;
    pEngine->GetArg(count - 1, value);
    int lastColumnArg;
    int row = value.ToInt();
    lastColumnArg = count - 2;
    for (int i = 0; i < count - 1; ++i) {
        pEngine->GetArg(lastColumnArg - i, value);
        pDesc->mPiiTable.SetValue(row, i, value, pEngine);
    }
    pEngine->Drop(count);
    pEngine->PushNull();
    return 0;
}

int GetMapDescEnemy(NPpu::CEngine* pEngine) {
    int column = pEngine->PopInt();
    int row = pEngine->PopInt();
    MapDesc* pDesc = IGameSession::GetInstance()->GetMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    if (pDesc != nullptr) {
        pEngine->Push(*pDesc->mPiiTable.GetCell(row, column));
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int GetMapDescEnemyRowSize(NPpu::CEngine* pEngine) {
    MapDesc* pDesc = IGameSession::GetInstance()->GetMapDesc(static_cast<CPpuEngine*>(pEngine)->GetStageDescTarget());
    if (pDesc != nullptr) {
        pEngine->PushInt(pDesc->mPiiTable.GetRowCount());
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int GetStageTypeName(NPpu::CEngine* pEngine) {
    pEngine->PushWString(GetMessageString(gStageWorlds[pEngine->PopInt()].mMessageID, nullptr));
    return 0;
}

int LeaveInitPart(NPpu::CEngine* pEngine) {
    static_cast<CPpuEngine*>(pEngine)->LeaveInitPart();
    pEngine->PushNull();
    return 1;
}

int InsertCallback(NPpu::CEngine* pEngine) {
    int position = pEngine->PopInt();
    int functionID = pEngine->PopInt();
    int messageID = pEngine->PopInt();
    pEngine->PushInt(static_cast<CPpuEngine*>(pEngine)->InsertCallback(messageID, functionID, position));
    return 0;
}

int EraseCallback(NPpu::CEngine* pEngine) {
    int functionID = pEngine->PopInt();
    pEngine->PopInt();
    int messageID = pEngine->PopInt();
    pEngine->PushInt(static_cast<CPpuEngine*>(pEngine)->EraseCallback(messageID, functionID));
    return 0;
}

int SendTo(NPpu::CEngine* pEngine) {
    CPpuEngine::Message message;
    NPpu::CValue sender;
    pEngine->GetThis(sender);
    message.mSender = sender.ToInt();
    pEngine->Pop(message.mArgument);
    message.mMessageID = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        CPpuEngine* pTarget = pObject->GetPpuEngine();
        if (pTarget != nullptr && pTarget->SendMessage(message)) {
            pEngine->PushInt(1);
            return 0;
        }
    }
    pEngine->PushNull();
    return 0;
}

int EnableCalcWorld(NPpu::CEngine* pEngine) {
    int enable = pEngine->PopInt();
    GetCurrentGameMap()->GetWorld()->EnableCalc(enable != 0);
    pEngine->PushNull();
    return 0;
}

int IsCalcWorldEnabled(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetCurrentGameMap()->GetWorld()->IsCalcEnabled());
    pEngine->PushNull();
    return 0;
}

int Fade(NPpu::CEngine* pEngine) {
    int frames = pEngine->PopInt();
    int type = pEngine->PopInt();
    IGameSession::GetInstance()->SetFade(type, frames);
    pEngine->PushNull();
    return 0;
}

int GetRemainingFadeFrames(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->GetFadeFrames());
    return 0;
}

int IgnoreTimeConsumption(NPpu::CEngine* pEngine) {
    gFrameTimingReset = true;
    pEngine->PushNull();
    return 0;
}

void SetupScriptWaza(CWaza* pWaza, int id, std::tr1::shared_ptr<BWorldObj> parent);

int NewWaza(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldObj* pParent = pEngine->PopWorldObj();
    int id = pEngine->PopInt();
    std::tr1::shared_ptr<CWaza> waza(new CWaza(CWaza::TYPE));
    waza->SwapRegister(pEngine->GetRegister());
    if (pParent) {
        SetupScriptWaza(waza.get(), id, pParent->shared_from_this());
    } else {
        SetupScriptWaza(waza.get(), id, std::tr1::shared_ptr<BWorldObj>());
    }
    if (pEngine->GetWorld()->Add(waza)) {
        waza->LoadScript(0, std::tr1::shared_ptr<CPpuEnv>());
        pEngine->PushInt(waza->GetId());
    } else {
        pEngine->PushNull();
    }
    return 0;
}

void SetupScriptWaza(CWaza* pWaza, int id, std::tr1::shared_ptr<BWorldObj> parent) {
    pWaza->Setup(id, parent);
}

int method_CWaza_getWazaID(NPpu::CEngine* pBaseEngine) {
    CWaza* pWaza = DynamicCast<CWaza>(static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj());
    if (pWaza) pBaseEngine->PushInt(pWaza->GetWazaID());
    else pBaseEngine->PushNull();
    return 0;
}

int InitWazaDescs(NPpu::CEngine* pEngine) {
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    int columns = pEngine->PopInt();
    int rows = pEngine->PopInt();
    pData->Initialize(rows, columns);
    pEngine->PushNull();
    return 0;
}

int SetWazaDesc(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    NPpu::CValue value;
    pEngine->Pop(value);
    int column = pEngine->PopInt();
    int row = pEngine->PopInt();
    pData->SetValue(row, column, value, pEngine);
    pEngine->PushNull();
    return 0;
}

int SetWazaDescRow(NPpu::CEngine* pEngine) {
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    int count = pEngine->FetchArgCount() - 1;
    NPpu::CValue argument;
    pEngine->GetArg(count, argument);
    int row = argument.ToInt();
    for (int column = count - 1; column >= 0; --column) {
        NPpu::CValue value;
        pEngine->Pop(value);
        pData->SetValue(row, column, value, pEngine);
    }
    pEngine->Pop(argument);
    pEngine->PushNull();
    return 0;
}

int GetWazaDesc(NPpu::CEngine* pEngine) {
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    int column = pEngine->PopInt();
    int row = pEngine->PopInt();
    NPpu::CValue value = *pData->GetCell(row, column);
    pEngine->Push(value);
    return 0;
}

int method_CWaza_clearBlownEnemysCount(NPpu::CEngine* pBaseEngine) {
    CWaza* pWaza = DynamicCast<CWaza>(static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj());
    if (pWaza) pWaza->ClearBlownEnemysCount();
    pBaseEngine->PushNull();
    return 0;
}

int method_CWaza_getBlownEnemysCount(NPpu::CEngine* pBaseEngine) {
    int id = pBaseEngine->PopInt();
    CWaza* pWaza = DynamicCast<CWaza>(static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj());
    if (pWaza) pBaseEngine->PushInt(pWaza->GetBlownEnemysCount(id));
    else pBaseEngine->PushNull();
    return 0;
}

int method_CWaza_incBlownEnemysCount(NPpu::CEngine* pBaseEngine) {
    int id = pBaseEngine->PopInt();
    CWaza* pWaza = DynamicCast<CWaza>(static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj());
    if (pWaza) pWaza->IncBlownEnemysCount(id);
    pBaseEngine->PushNull();
    return 0;
}
