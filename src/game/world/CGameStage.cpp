#include <game/world/IGameStage.h>

#include <game/core/CFiber.h>
#include <game/world/IGameMap.h>
#include <game/session/IGameResult.h>
#include <game/session/IGameEnding.h>
#include <game/ui/IMultiRanking.h>
#include <game/session/IGameSession.h>
#include <game/world/MapDesc.h>
#include <game/core/MathUtil.h>
#include <game/audio/Sound.h>
#include <memory>
#include <nw4r/snd/snd_SoundHandle.h>

namespace {

class CGameStage : public IGameStage {
public:
    explicit CGameStage(const Param& rParam);
    virtual ~CGameStage();
    virtual void Run();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual IGameMap* GetGameMap();
    virtual int GetLife();
    virtual void SetLife(int life);
    virtual void ChangeMap(MapIndex index);
    virtual void RequestState(int kind);
    virtual void SetEnvSE(int id);
    virtual void StopEnvSE();

private:
    void Release() {}
    int RunMap();

    Param mParam;
    MapIndex mMapIndex;
    int mLife;
    std::tr1::shared_ptr<IGameMap> mMap;
    int mState;
    nw4r::snd::SoundHandle mSound;
};

CGameStage::CGameStage(const Param& rParam) : mParam(rParam), mMapIndex(rParam.mMapIndex), mLife(3), mState(0) {}

CGameStage::~CGameStage() {
    Release();
}

void CGameStage::Run() {
    for (;;) {
        int result = RunMap();
        switch (result) {
        case 3:
        case 4:
            StopEnvSE();
            return;
        case 5:
        case 6:
        case 7:
            StopEnvSE();

            mMap = IMultiRanking::Create(IGameSession::GetInstance()->GetMapDesc(mMapIndex), true);
            mMap->RunAction();
            mMap.reset();

            switch (result) {
            case 6:
                return;
            case 5:
                mMap = IGameResult::Create(IGameSession::GetInstance()->GetMapDesc(mMapIndex), true);
                mMap->RunAction();
                mMap.reset();
                return;
            case 7:
                mMap = IGameEnding::Create(IGameSession::GetInstance()->GetMapDesc(mMapIndex), true);
                mMap->RunAction();
                mMap.reset();
                IGameSession::GetInstance()->RequestSceneChange(0, 4);
                return;
            }
        }
    }
}

int CGameStage::RunMap() {
    IGameMap::Param param;
    param.mMapDesc = IGameSession::GetInstance()->GetMapDesc(mMapIndex);
    param.mEnvironment = mParam.mEnvironment;
    mMap = IGameMap::Create(param);
    mMap->RunAction();

    mState = 0;
    int state;
    for (;;) {
        state = mState;
        mState = 0;
        if (state == 0 || (state == 8 && param.mMapDesc->mIndex == mMapIndex)) {
            YieldFiber();
            continue;
        }
        break;
    }

    if ((state >= 5 && state <= 7) || state == 1 || state == 3) {
        IGameSession::GetInstance()->SetFade(1, -1);
        while (IGameSession::GetInstance()->GetFadeFrames() > 0) {
            YieldFiber();
        }
    }
    IGameSession::GetInstance()->SetFade(1, 0);
    mMap.reset();
    return state;
}

IGameMap* CGameStage::GetGameMap() {
    return mMap.get();
}

int CGameStage::GetLife() {
    return mLife;
}

void CGameStage::SetLife(int life) {
    mLife = Clamp(life, 0, 3);
}

void CGameStage::ChangeMap(MapIndex index) {
    mState = 1;
    mMapIndex = index;
}

void CGameStage::RequestState(int kind) {
    switch (kind) {
    default:
        mState = 3;
        break;
    case 1:
        mState = 5;
        break;
    case 2:
        mState = 6;
        break;
    case 3:
        mState = 7;
        break;
    case 4:
        mState = 4;
        break;
    }
}

void CGameStage::SetEnvSE(int id) {
    StopEnvSE();
    PlaySound(id, &mSound, nullptr);
}

void CGameStage::StopEnvSE() {
    StopSound(&mSound, nullptr, 0.0f);
}

void CGameStage::Calc() {
    if (mMap) {
        mMap->Calc();
    }
}

void CGameStage::CalcMtx() {
    if (mMap) {
        mMap->CalcMtx();
    }
}

void CGameStage::CalcView() {
    if (mMap) {
        mMap->CalcView();
    }
}

void CGameStage::Draw() {
    if (mMap) {
        mMap->Draw();
    }
}

}

std::tr1::shared_ptr<IGameStage> IGameStage::Create(const Param& rParam) {
    std::tr1::shared_ptr<CGameStage> stage(new CGameStage(rParam));
    return stage;
}

IGameStage* GetGameStage() {
    IGameSession* pSession = GetGameSession();
    return pSession ? pSession->GetGameStage() : nullptr;
}

IGameStage* GetCurrentGameStage() {
    IGameSession* pSession = GetGameSession();
    return pSession ? pSession->GetGameStage() : nullptr;
}
