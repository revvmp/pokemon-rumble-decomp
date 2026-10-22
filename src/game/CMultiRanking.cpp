#include <algorithm>
#include <game/BModelObj.h>
#include <game/CFiber.h>
#include <game/CMultiRanking.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/Font.h>
#include <game/IController.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/Message.h>
#include <game/MessageFormat.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <game/StringFormat.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>

void ChangeBGM(unsigned long id);

nw4r::math::VEC3 GetNodePosition(std::tr1::shared_ptr<BModelObj> pObj, const char* pName) {
    if (pObj && pObj->GetModel()) {
        nw4r::g3d::ScnMdlSimple* pScnMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(pObj->GetModel()->GetScnObj());
        if (pScnMdl != nullptr) {
            nw4r::math::MTX34 mtx;
            nw4r::g3d::ResNode node = pScnMdl->GetResMdl().GetResNode(pName);
            pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, node.GetID());
            return nw4r::math::VEC3(mtx._03, mtx._13, mtx._23);
        }
    }

    return nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
}

inline CMultiRanking::CMultiRanking(const MapDesc* pDesc)
    : mpMapDesc(pDesc), mUI(new CUIManager), mRootLayout(new BUILayout),
      mRankingLayout(new BUILayout), mMaximumScore(0), mPlayerCount(0),
      mMotionCount(0), mFinishedMotionCount(0),
      mReturnFromGame(IGameSession::GetInstance()->GetMapChangeType() == 1), mActive(false) {
    for (int i = 0; i < 4; ++i) {
        mScores[i] = 0;
        mIDVisible[i] = false;
        mMotionFinished[i] = false;
        mOffsets[i] = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    }
}

namespace {
const char* sModelNames[5] = {"ranking_p1", "ranking_p2", "ranking_p3", "ranking_p4", "ranking_back"};
const char* sRootNames[4] = {"ROOT_p1", "ROOT_p2", "ROOT_p3", "ROOT_p4"};
}

inline void CMultiRanking::StartRankingMotion() {
    mRankingLayout->GetLayout()->CreateMotion("begin");
}

inline void CMultiRanking::StartPlayerLayoutMotion(int i) {
    if (mPlayerLayouts[i])
        mPlayerLayouts[i]->GetLayout()->CreateMotion("begin");
}

inline void CMultiRanking::PlayModelMotion(int i) {
    mMotions[i] = mModels[i]->Play(sModelNames[i], 0.0f, 1.0f);
}

inline void CMultiRanking::StartPlayerMotion(int i) {
    if (mPlayerIDs[i] && mScores[i] > 0) {
        PlayModelMotion(i);
        ++mMotionCount;
    } else {
        mMotionFinished[i] = true;
    }
}

inline void CMultiRanking::SkipMotions() {
    for (int i = 0; i < 4; ++i) {
        if (mMotions[i] && !mMotionFinished[i])
            mModels[i]->Play(sModelNames[i], float(mScores[i]) * float(mMotions[i]->GetLength()) / float(mMaximumScore), 0.0f);
    }
}

inline bool CMultiRanking::Init(bool background) {
    if (!IsGameFlagSet(2))
        return true;
    for (int i = 0; i < 4; ++i) {
        if (IGameSession::GetInstance()->GetPlayer(i)->mState.mJoined)
            ++mPlayerCount;
    }
    const char* const* pArchives = gSharedLayoutArchives;
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mRankingLayout->Load(pArchives, "ranking", fonts, background);
    mRankingLayout->SetText("time_c_00", GetMessageString(393460, L""));
    mRankingLayout->SetText("time_se_00", GetMessageString(393460, L""));
    mUI->AddChild(mRankingLayout);
    mRootLayout->Load(pArchives, "dlgpiisel_root", fonts, background);
    mUI->AddChild(mRootLayout);
    mPauseMenu = IPauseMenu::Create(mpMapDesc, true, 1);
    BWorld::Desc desc;
    desc.SetDrawType(0);
    desc.SetEffectCapacity(128);
    desc.SetObjectCapacity(256);
    desc.SetOutlineCapacity(128);
    desc.SetModelCapacity(128);
    desc.SetScnObjCapacity(512);
    desc.SetSoundListenerEnable(true);
    mWorld.reset(new BWorld(desc));
    mCamera.reset(new BWorldCamera(2));
    mWorld->AddCamera(mCamera);
    mWorld->SetCurrentCamera(mCamera);
    mCamera->Load("5:/event/ranking.arc/event.brres", background);
    mCamera->Play("ranking_camera", 0.0f, 1.0f);
    for (int i = 0; i < 5; ++i) {
        std::tr1::shared_ptr<CModel> model(new CModel);
        std::string path = FormatString("5:/event/ranking.arc/%s.brres", sModelNames[i]);
        const char* pFiles[2] = {path.c_str(), nullptr};
        CModel::LoadParams params = {pFiles, false, false, 0, 0, nullptr};
        if (model->Load(params, background)) {
            std::tr1::shared_ptr<BModelObj> object(new BModelObj(32));
            object->ChangeModel(model, 1);
            object->SetVisible(true);
            mWorld->Add(object);
            if (i < 4)
                mModels[i] = object;
        }
    }
    mMaximumScore = 0;
    for (int i = 0; i < 4; ++i) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
        if (pPlayer->mState.mJoined) {
            mPlayerLayouts[i].reset(new BUILayout);
            mPlayerLayouts[i]->Load(pArchives, "dlgpiisel_p_root", fonts, background);
            mRootLayout->AttachChild(FormatString("P%d", i).c_str(), mPlayerLayouts[i]);
            mPlayerIDs[i].reset(new CUIPlayerID);
            mPlayerIDs[i]->Setup(i, 2, pArchives, "playerid", fonts, background);
            mPlayerIDs[i]->Update();
            mPlayerIDs[i]->SetVisible(true);
            mPlayerLayouts[i]->AttachChild("PLAYERID", mPlayerIDs[i]);
            std::tr1::shared_ptr<CPii> pii(new CPii(pPlayer->GetPersonalData(), 64));
            pii->Initialize(false);
            pii->SetVisible(true);
            mWorld->Add(pii);
            mPiis[i] = pii;
            mScores[i] = pPlayer->mRankingScore;
            CLayout* pLayout = mRankingLayout->GetLayout();
            pLayout->SetText(FormatString("P%d_TEXT", i).c_str(), FormatMessage(GetMessageString(393442, L""), std::min(mScores[i], 999)).c_str());
            pLayout->SetPaneVisible(FormatString("P%d", i).c_str(), true);
            if (mScores[i] > mMaximumScore)
                mMaximumScore = mScores[i];
        }
    }
    for (int i = 0; i < 4; ++i) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
        if (pPlayer->mState.mJoined) {
            bool showStand = pPlayer->GetPersonalData()->GetHP() <= 0 || mReturnFromGame;
            if (showStand) {
                mPiis[i]->PlayMotion(104, 0.0f, 1.0f);
                const nw4r::math::VEC3 size = mPiis[i]->GetSize();
                mOffsets[i].x = -size.y;
            } else if (mMaximumScore == mScores[i] && mMaximumScore > 0) {
                mPiis[i]->PlayMotion(5, 0.0f, 1.0f);
                mPiis[i]->QueueMotion(5, -1.0f, 1.0f);
                mPiis[i]->AppendMotion(0, -1.0f, 1.0f);
            } else {
                mPiis[i]->PlayMotion(0, 0.0f, 1.0f);
            }
        }
    }
    mFlashFrame = 0;
    mActive = true;
    return true;
}

std::tr1::shared_ptr<IMultiRanking> IMultiRanking::Create(const MapDesc* pDesc, bool background) {
    std::tr1::shared_ptr<CMultiRanking> ranking(new CMultiRanking(pDesc));
    if (ranking->Init(background))
        return ranking;
    return std::tr1::shared_ptr<IMultiRanking>();
}

void CMultiRanking::RunAction() {
    if (!mActive) {
        YieldFiberStarted();
        return;
    }
    IGameSession::GetInstance()->SetFade(0, -1);
    YieldFiberStarted();
    if (mpMapDesc->mIndex.mStage != 7 || mReturnFromGame)
        ChangeBGM(626);
    while (IGameSession::GetInstance()->GetFadeFrames() > 0)
        YieldFiber();
    StartRankingMotion();
    for (int i = 0; i < 4; ++i) {
        StartPlayerMotion(i);
        StartPlayerLayoutMotion(i);
    }
    if (mMaximumScore > 0)
        PlaySound(818, &mSound, nullptr);
    YieldFiber();
    int readyCount = 0;
    bool allReady = false;
    while (!allReady) {
        for (int i = 0; i < 4; ++i) {
            if (mPlayerIDs[i]) {
                CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
                if (pPlayer->mState.mJoined) {
                    IController* pController = pPlayer->mController;
                    if (pController && pController->IsConnected() && (pController->GetTrig() & 0x30)) {
                        if (mFinishedMotionCount != mMotionCount) {
                            SkipMotions();
                        } else {
                            if (!mIDVisible[i]) {
                                PlaySound(8);
                                mPlayerIDs[i]->SetDisplayMode(0);
                                mPlayerIDs[i]->Update();
                                mIDVisible[i] = true;
                                if (++readyCount == mPlayerCount)
                                    allReady = true;
                            }
                        }
                    }
                }
            }
        }
        YieldFiber();
    }
    IGameSession::GetInstance()->SetFade(1, -1);
    while (IGameSession::GetInstance()->GetFadeFrames() > 0)
        YieldFiber();
}

void CMultiRanking::Draw() {
    if (mActive) {
        mWorld->Draw();
        mUI->Draw();
    }
}

void CMultiRanking::CalcView() {
    if (mActive) {
        mWorld->CalcView();
        mUI->CalcView();
        mPauseMenu->CalcView();
    }
}

void CMultiRanking::CalcMtx() {
    if (mActive) {
        mWorld->CalcMtx();
        for (int i = 0; i < 4; ++i) {
            if (mPiis[i])
                mPiis[i]->SetPosition(GetNodePosition(mModels[i], sRootNames[i]) + mOffsets[i]);
        }
        mUI->CalcMtx();
        mPauseMenu->CalcMtx();
    }
}

void CMultiRanking::Calc() {
    if (mActive) {
        mWorld->Calc();
        mFlashFrame = (mFlashFrame + 1) % 90;
        for (int i = 0; i < 4; ++i) {
            if (mPlayerIDs[i]) {
                if (!mIDVisible[i]) {
                    if (mFlashFrame == 0) {
                        mPlayerIDs[i]->SetDisplayMode(0);
                        mPlayerIDs[i]->Update();
                    } else if (mFlashFrame == 45) {
                        mPlayerIDs[i]->SetDisplayMode(2);
                        mPlayerIDs[i]->Update();
                    }
                }
                if (mMotions[i] && !mMotionFinished[i]) {
                    if (!(mMotions[i]->IsPlaying())) {
                        mMotionFinished[i] = true;
                    } else if (!mMotions[i]->IsPausing()) {
                        if (mMaximumScore * mMotions[i]->GetCurrentFrame() >= mScores[i] * mMotions[i]->GetLength()) {
                            mMotions[i]->Pause();
                            mMotionFinished[i] = true;
                        }
                    }
                    if (mMotionFinished[i]) {
                        if (++mFinishedMotionCount == mMotionCount)
                            StopSound(&mSound, nullptr, 0.0f);
                    }
                }
            }
        }
        mUI->Calc();
    }
}
