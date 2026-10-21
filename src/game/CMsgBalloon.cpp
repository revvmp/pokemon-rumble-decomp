#include <game/CMsgBalloon.h>

#include <game/CPiiPersonalData.h>
#include <game/CPiiTex.h>
#include <game/CPlayerPii.h>
#include <game/BUILayout.h>
#include <game/BWorld.h>
#include <game/BWorldCamera.h>
#include <game/CPlayer.h>
#include <game/CPpuEngine.h>
#include <game/CUIManager.h>
#include <game/IController.h>
#include <game/IGameMap.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/StringFormat.h>
#include <game/IGameSession.h>
#include <game/MapIndex.h>
#include <game/MathUtil.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <game/StageDesc.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/math/math_triangular.h>

#pragma dont_inline on

CMapRecord* GetMapRecord(MapIndex index) {
    int mode = index.mMode;
    int area = index.mArea;
    int stage = index.mStage;
    int map = index.mMap;
    return &GetSaveData(-1)->mData.mMapRecords[mode][area][stage][map];
}

bool IsStageCleared(MapIndex index) {
    index.mMap = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    return GetMapRecord(index)->IsFlagSet(2);
}

int GetStageClearCount(MapIndex index) {
    int lastMap = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    return Clamp(GetSaveData(-1)->mData.mMapRecords[index.mMode][index.mArea][index.mStage][lastMap].GetClearCount(), 0, 999);
}

std::tr1::shared_ptr<IMsgBalloon> IMsgBalloon::Create(bool flag) {
    std::tr1::shared_ptr<CMsgBalloon> balloon(new CMsgBalloon);
    balloon->Init(flag);
    return balloon;
}

CMsgBalloon::CMsgBalloon() {}

void CMsgBalloon::Init(bool flag) {
    mCurrentEntry = nullptr;
    mCurrentIndex = -1;
    mIsShown = false;
    Show(true);

    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();

    mUIManager.reset(new CUIManager);
    mLayout.reset(new BUILayout);
    mUIManager->AddChild(mLayout);
    const char* const* archives = gSharedLayoutArchives;
    mLayout->Load(archives, "msg_balloon", fonts, flag);

    mMsgLayout.reset(new BUILayout);
    mMsgLayout->Load(archives, "msg_balloon_msg", fonts, flag);

    mWarpLayout.reset(new BUILayout);
    mWarpLayout->Load(archives, "msg_balloon_warp", fonts, flag);

    mWarpLayout->SetText("LABEL_c", GetMessageString(0x600C8, L"なかまに できる ポケモン"));
    mWarpLayout->SetText("UNKNOWN_c", GetMessageString(0x600C5, L"まだ クリアしていません"));
    mWarpLayout->SetText("LABEL_CLEAR_BR", GetMessageString(0x600CA, L"クリアタイム"));
    mWarpLayout->SetText("EXTRA_c", GetMessageString(0x600CB, L"いろんな ポケモンが とうじょうします"));
    mWarpLayout->SetText("CLEAR_00", GetMessageString(0x600C6, L"クリア"));

    for (int i = 0; i < 18; i++) {
        std::tr1::shared_ptr<BUILayout> model(new BUILayout);
        model->Load(archives, "msg_balloon_warp_model", fonts, flag);
        mModelLayouts.push_back(model);
        std::string name = FormatString("MODEL%02d", i);
        mWarpLayout->AttachChild(name.c_str(), model);
    }

    mLayout->AttachChild("above_site", mWarpLayout);
    mLayout->AttachChild("below_site", mMsgLayout);
    mLayout->GetLayout()->SetVisible(false);
}

void CMsgBalloon::Show(bool show) {
    if (mIsShown == show) {
        return;
    }

    mIsShown = show;
    if (show) {
        mShownEntries[0] = nullptr;
        mShownEntries[1] = nullptr;
        mShownEntries[2] = nullptr;
        mShownEntries[3] = nullptr;
        mCurrentEntry = nullptr;
        mCurrentIndex = -1;
        mShowWait = 5;
        mMotion.reset();
    } else {
        mLayout->GetLayout()->SetVisible(false);
        mMotion = mLayout->GetLayout()->CreateMotion("end");
        PlaySound(0x59);
    }
}

void CMsgBalloon::Draw() {
    mUIManager->Draw();
}

void CMsgBalloon::CalcView() {
    std::vector<std::tr1::shared_ptr<CPiiTex> >::iterator it;
    if (mCurrentEntry != nullptr && mCurrentEntry->mStageValue != 0xFFFFFFFF) {
        for (it = mModelTextures.begin(); it != mModelTextures.end(); ++it) {
            (*it)->CalcView();
        }
    }
}

void CMsgBalloon::CalcMtx() {
    std::vector<std::tr1::shared_ptr<CPiiTex> >::iterator it;
    if (mCurrentEntry != nullptr && mCurrentEntry->mStageValue != 0xFFFFFFFF) {
        for (it = mModelTextures.begin(); it != mModelTextures.end(); ++it) {
            (*it)->CalcMtx();
        }
    }
}

void CMsgBalloon::Calc() {
    BWorld* pWorld = GetCurrentGameMap()->GetWorld();
    if (pWorld == nullptr) {
        return;
    }
    BWorldCamera* pCamera = pWorld->GetCurrentCamera();
    if (pCamera == nullptr) {
        return;
    }
    if (mEntries.size() == 0) {
        return;
    }

    if (mMotion) {
        if (!mMotion->IsEnded(false)) {
            UpdateBalloon(pCamera, mLayout->GetLayout(), nullptr);
        } else {
            mLayout->GetLayout()->SetVisible(false);
            mMotion.reset();
        }
    }
    if (!mIsShown) {
        return;
    }

    Entry* pBest = nullptr;
    nw4r::math::VEC3 bestPosition;
    int bestIndex = -1;
    IDlg* pDialog = GetCurrentGameMap()->GetCurrentDialog();
    if (pDialog == nullptr || pDialog == this) {
        for (int i = 0; i < 4; i++) {
            CPlayerPii* pPii = IGameSession::GetInstance()->GetPlayer(i)->GetPii();
            if (pPii == nullptr || pPii->GetPersonalData()->GetHP() <= 0) {
                mShownEntries[i] = nullptr;
                continue;
            }

            Entry* pNearest = nullptr;
            nw4r::math::VEC3 nearestPosition;
            f32 nearestDistance = 1.0f;
            std::list<Entry>::iterator it;
            for (it = mEntries.begin(); it != mEntries.end(); ++it) {
                nw4r::math::VEC3 position((*it).mPosition);
                std::tr1::shared_ptr<BWorldObj> pObject = (*it).mObject.lock();
                if (pObject) {
                    if (!pObject->IsVisible()) {
                        continue;
                    }
                    position += pObject->GetPositionRef();
                }
                const nw4r::math::VEC3& rPiiPosition = pPii->GetPositionRef();
                f32 sinValue;
                f32 cosValue;
                nw4r::math::SinCosFIdx(&sinValue, &cosValue, 0.71111f * pPii->GetRotation());
                f32 dz = position.z - (rPiiPosition.z + 0.33f * cosValue);
                f32 dx = position.x - (rPiiPosition.x + 0.33f * sinValue);
                f32 distance = (dz * dz + dx * dx) * (*it).mDistanceScale;
                if (distance < nearestDistance) {
                    nearestDistance = distance;
                    pNearest = &*it;
                    nearestPosition = position;
                }
            }

            if (mShownEntries[i] != pNearest) {
                mShownEntries[i] = pNearest;
                if (pNearest != nullptr) {
                    pBest = pNearest;
                    bestPosition = nearestPosition;
                    bestIndex = i;
                }
            } else if (pNearest != nullptr && pBest == nullptr && mCurrentIndex == i) {
                pBest = pNearest;
                bestPosition = nearestPosition;
                bestIndex = i;
            }
        }
    }

    if (pBest == nullptr) {
        if (mHideWait > 0) {
            mHideWait--;
        } else if (mCurrentEntry != nullptr) {
            mMotion = mLayout->GetLayout()->CreateMotion("end");
            PlaySound(0x59);
            mCurrentEntry = nullptr;
        }
        UpdateBalloon(pCamera, mLayout->GetLayout(), nullptr);
    } else {
        if (mCurrentEntry != pBest) {
            if (mCurrentEntry != nullptr) {
                mCurrentEntry->ClearFlags(1);
                mCurrentEntry = nullptr;
            }
            mOffset = bestPosition;
        }
        bool behind;
        UpdateBalloon(pCamera, mLayout->GetLayout(), &behind);
        if (mCurrentEntry == nullptr) {
            mLayout->GetLayout()->SetVisible(true);
            mLayout->GetLayout()->CreateMotion("begin");
            PlaySound(0x58);
            mLayout->GetLayout()->SetPaneVisible("above", behind);
            mLayout->GetLayout()->SetPaneVisible("below", !behind);
            mLayout->RemoveChild(mMsgLayout);
            mMsgLayout->GetLayout()->DetachChildren();
            mLayout->RemoveChild(mWarpLayout);
            mWarpLayout->GetLayout()->DetachChildren();
            if (pBest->mStageValue != 0xFFFFFFFF) {
                if (pBest->mStageID.mWorld == 7) {
                    SetupBattleWarp(pBest->mStageID);
                } else {
                    SetupStageWarp(pBest->mStageID);
                }
                mWarpLayout->GetLayout()->SetText("TITLE", GetMessageString(gStageWorlds[pBest->mStageID.mWorld].mMessageID, L""));
                mLayout->AttachChild(behind ? "above_site" : "below_site", mWarpLayout);
            } else {
                mMsgLayout->GetLayout()->SetText("TEXT", pBest->mText.c_str());
                bool hasCallback = false;
                {
                    std::tr1::shared_ptr<BWorldObj> pObject = pBest->mObject.lock();
                    if (pObject) {
                        CPpuEngine* pEngine = pObject->GetPpuEngine();
                        if (pEngine != nullptr && pEngine->HasCallback(1)) {
                            hasCallback = true;
                        }
                    }
                }
                mMsgLayout->GetLayout()->SetPaneVisible("button", hasCallback);
                mLayout->AttachChild(behind ? "above_site" : "below_site", mMsgLayout);
            }
            mCurrentEntry = pBest;
            mCurrentIndex = bestIndex;
            mMotion.reset();
            mShowWait = 1;
        }

        if (mHideWait != 1) {
            mHideWait = 1;
        }

        if (mShowWait > 0) {
            mShowWait--;
        } else {
            std::tr1::shared_ptr<BWorldObj> pObject = pBest->mObject.lock();
            if (pObject) {
                CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(bestIndex);
                CPlayerPii* pPii = pPlayer->GetPii();
                IController* pController = pPlayer->GetController();
                if (pController != nullptr && pController->IsConnected() && (pController->GetTrig() & 0x20)) {
                    pPii->GetPpuEngine();
                    CPpuEngine::Message message;
                    message.mSender = pPii->GetId();
                    message.mArgument.SetInt(0);
                    message.mMessageID = 1;
                    CPpuEngine* pEngine = pObject->GetPpuEngine();
                    if (pEngine != nullptr) {
                        pEngine->SendMessage(message);
                    }
                }
            }
        }
    }

    std::vector<std::tr1::shared_ptr<CPiiTex> >::iterator it;
    for (it = mModelTextures.begin(); it != mModelTextures.end(); ++it) {
        (*it)->Calc();
    }
    mUIManager->Calc();
}

void CMsgBalloon::UpdateBalloon(BWorldCamera* pCamera, CLayout* pLayout, bool* pBehind) {
    nw4r::math::MTX34 viewMtx;
    nw4r::math::MTX44 projectionMtx;
    pCamera->GetG3dCamera().GetCameraMtx(&viewMtx);
    pCamera->GetG3dCamera().GetProjectionMtx(&projectionMtx);
    nw4r::math::VEC3 position(mOffset);
    nw4r::math::VEC3Transform(&position, &viewMtx, &position);
    nw4r::math::VEC3TransformCoord(&position, &projectionMtx, &position);
    const nw4r::ut::Rect& rRect = pLayout->GetViewRect();
    position.x = position.x * rRect.GetWidth() / 2.0f;
    position.y = -position.y * rRect.GetHeight() / 2.0f;
    position.z = 0.0f;
    pLayout->FindPane("root")->SetTranslate(position);
    pLayout->UpdateAttachedLayouts();
    if (pBehind != nullptr) {
        *pBehind = position.y <= 0.0f;
    }
}
