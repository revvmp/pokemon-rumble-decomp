#include <game/IDlgMsgWin.h>
#include <revolution/enc.h>
#include <game/IDlgPiiSelector.h>
#include <game/CNoticeDisplay.h>
#include <game/IGameEnding.h>
#include <game/IGameMap.h>
#include <game/BWorld.h>
#include <game/IGameSession.h>
#include <math.h>
#include <game/BWorldCamera.h>
#include <game/CPpuEngine.h>
extern const EffectInfo gEffectInfos[696];

bool GetWazaEffectParam(int waza, CEffect::Param& rParam) {
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    rParam.mInfo = &gEffectInfos[pData->GetCell(waza, 1)->ToInt()];
    f32 duration = pData->GetCell(waza, 3)->ToFloat();
    int type = pData->GetCell(waza, 4)->ToInt();
    f32 active = pData->GetCell(waza, 5)->ToFloat();
    f32 recovery = pData->GetCell(waza, 6)->ToFloat();
    f32 radius = pData->GetCell(waza, 7)->ToFloat();
    f32 speed = pData->GetCell(waza, 8)->ToFloat();
    switch (type) {
    case 16:
    case 17:
    case 18:
    case 0:
    case 2:
        if (duration > (active + recovery) / speed) {
            recovery = duration * speed - active;
        } else {
            duration = ceil((active + recovery) / speed);
        }
        break;
    case 5:
        if (duration > ceil(active / speed)) {
            duration = ceil(active / speed);
        }
        break;
    case 6:
        if (duration > (active + recovery) / speed) {
            duration = ceil((active + recovery) / speed);
        }
        break;
    }
    switch (rParam.mInfo->mType) {
    case CEffect::TYPE_STOP_EMISSION:
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil(duration);
        rParam.mFrames.mLife = ceil(active / speed);
        rParam.mFrames.mRadius = active;
        return true;
    case CEffect::TYPE_RADIUS:
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil(duration);
        rParam.mFrames.mRadius = radius;
        return true;
    case CEffect::TYPE_LIFE:
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil(recovery / speed);
        rParam.mFrames.mLife = ceil(active / speed);
        rParam.mFrames.mRadius = speed;
        rParam.mFrames.mHeight = radius;
        return true;
    case CEffect::TYPE_LIFE_FOLLOW:
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil(recovery / speed);
        rParam.mFrames.mLife = ceil(active / speed);
        rParam.mFrames.mRadius = speed;
        rParam.mFrames.mHeight = NW4R_MATH_DEG_TO_RAD(radius);
        return true;
    case CEffect::TYPE_SCALE:
        rParam.mEnable = true;
        rParam.mScale = radius;
        return true;
    case CEffect::TYPE_EXTEND_EMISSION:
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil((active + recovery) / speed);
        rParam.mFrames.mLife = ceil(active / speed);
        rParam.mFrames.mRadius = speed;
        rParam.mFrames.mHeight = NW4R_MATH_DEG_TO_RAD(radius);
        return true;
    }
    rParam.mEnable = false;
    return false;
}

int method_effAlive(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    CEffect* pEffect = pEngine->PopEffect();
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

static inline CEffect::Param GetEffectParam(int id) {
    CEffect::Param param;
    param.mInfo = &gEffectInfos[id];
    param.mEnable = false;
    return param;
}

int method_newEffectRandomDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(GetEffectParam(id), pFollow);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newEffectWithAbsDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(GetEffectParam(id), pFollow, rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newEffectWithRelDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect* pEffect = pEngine->GetWorld()->CreateEffectFollowRotation(GetEffectParam(id), pFollow, rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newEffectTowardTarget(NPpu::CEngine* pBaseEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj();
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(GetEffectParam(id), pFollow, pTarget);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}


int NewEffectWithPosDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    f32 x = pEngine->PopFloat();
    int id = pEngine->PopInt();
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(GetEffectParam(id), nw4r::math::VEC3(x, y, z), rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newWazaEffectRandomDir(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    BWorldObj* pFollow = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    CEffect::Param param;
    GetWazaEffectParam(id, param);
    CEffect* pEffect = static_cast<CPpuEngine*>(pEngine)->GetWorld()->CreateEffect(param, pFollow);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newWazaEffectWithAbsDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    GetWazaEffectParam(id, param);
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, pFollow, rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newWazaEffectWithRelDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    GetWazaEffectParam(id, param);
    CEffect* pEffect = pEngine->GetWorld()->CreateEffectFollowRotation(param, pFollow, rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int method_newWazaEffectTowardTarget(NPpu::CEngine* pBaseEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj();
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    GetWazaEffectParam(id, param);
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, pFollow, pTarget);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

int NewWazaEffectWithPosDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    f32 x = pEngine->PopFloat();
    int id = pEngine->PopInt();
    CEffect::Param param;
    GetWazaEffectParam(id, param);
    CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, nw4r::math::VEC3(x, y, z), rotation);
    pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    return 0;
}

static inline bool GetWazaRangeEffectParam(int waza, CEffect::Param& rParam) {
    WazaDescData* pData = IGameSession::GetInstance()->GetWazaDescData();
    rParam.mInfo = &gEffectInfos[pData->GetCell(waza, 1)->ToInt()];
    if (rParam.mInfo->mType == CEffect::TYPE_RADIUS) {
        rParam.mInfo = &gEffectInfos[686];
        f32 duration = pData->GetCell(waza, 3)->ToFloat();
        f32 radius = pData->GetCell(waza, 7)->ToFloat();
        rParam.mEnable = true;
        rParam.mFrames.mEmitSpan = ceil(duration);
        rParam.mFrames.mRadius = radius;
        return true;
    }
    rParam.mEnable = false;
    return false;
}

int method_newWazaRangeEffectRandomDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    bool enabled = GetWazaRangeEffectParam(id, param);
    if (enabled) {
        CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, pFollow);
        pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_newWazaRangeEffectWithAbsDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    bool enabled = GetWazaRangeEffectParam(id, param);
    if (enabled) {
        CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, pFollow, rotation);
        pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_newWazaRangeEffectWithRelDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    bool enabled = GetWazaRangeEffectParam(id, param);
    if (enabled) {
        CEffect* pEffect = pEngine->GetWorld()->CreateEffectFollowRotation(param, pFollow, rotation);
        pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_newWazaRangeEffectTowardTarget(NPpu::CEngine* pBaseEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pBaseEngine)->PopWorldObj();
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int id = pEngine->PopInt();
    BWorldObj* pFollow = pEngine->PopWorldObj();
    CEffect::Param param;
    bool enabled = GetWazaRangeEffectParam(id, param);
    if (enabled) {
        CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, pFollow, pTarget);
        pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int NewWazaRangeEffectWithPosDir(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    f32 rotation = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    f32 x = pEngine->PopFloat();
    int id = pEngine->PopInt();
    CEffect::Param param;
    bool enabled = GetWazaRangeEffectParam(id, param);
    if (enabled) {
        CEffect* pEffect = pEngine->GetWorld()->CreateEffect(param, nw4r::math::VEC3(x, y, z), rotation);
        pEngine->PushInt(pEffect ? pEffect->GetHandle() : 0);
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_effFinalize(NPpu::CEngine* pEngine) {
    CEffect* pEffect = static_cast<CPpuEngine*>(pEngine)->PopEffect();
    if (pEffect) { pEffect->Finalize(FALSE); }
    pEngine->PushNull();
    return 0;
}

int method_effFinalizeAll(NPpu::CEngine* pEngine) {
    CEffect* pEffect = static_cast<CPpuEngine*>(pEngine)->PopEffect();
    if (pEffect) { pEffect->Finalize(TRUE); }
    pEngine->PushNull();
    return 0;
}

int FinalizeEffects(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetWorld()->RemoveAllEffects(FALSE);
    pEngine->PushNull();
    return 0;
}

int FinalizeEffectsAll(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetWorld()->RemoveAllEffects(TRUE);
    pEngine->PushNull();
    return 0;
}
