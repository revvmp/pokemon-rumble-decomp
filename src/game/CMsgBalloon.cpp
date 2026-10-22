#include <revolution/types.h>
#include <MSL_CPP/alloc_ptr.h>
#include <MSL_CPP/allocator.h>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <game/MapDesc.h>
#include <game/CMsgBalloon.h>

#include <game/CBattleRoyalBox.h>
#include <game/COyaInfo.h>
#include <game/CPiiCollectionBox.h>
#include <game/NPpuVM.h>
#include <game/PiiSpeciesData.h>
#include <map>
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
#include <game/MessageFormat.h>
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

static const StageID sInvalidStageID = {-1, -1, -1, -1};

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

inline bool CMsgBalloon::Close() {
    return true;
}

inline void CMsgBalloon::Add(const wchar_t* pText, f32 z, f32 x) {
    std::list<Entry>::iterator it;
    Entry entry;
    mEntries.push_back(Entry());
    it = --mEntries.end();
    it->mPosition.x = x;
    it->mPosition.y = 0.0f;
    it->mPosition.z = z;
    it->mDistanceScale = 1.0f;
    it->mText = pText;
    it->mStageID = sInvalidStageID;
}

inline void CMsgBalloon::Register(BWorldObj* pObject, const wchar_t* pText, f32 radius) {
    std::list<Entry>::iterator it;
    Entry entry;
    mEntries.push_back(Entry());
    it = --mEntries.end();
    it->mObject = pObject->shared_from_this();
    it->mPosition.x = 0.0f;
    it->mPosition.y = 0.0f;
    it->mPosition.z = 0.0f;
    it->mDistanceScale = 1.0f / (radius * radius);
    it->mText = pText;
    it->mStageID = sInvalidStageID;
}

inline bool CMsgBalloon::IsShown(int index) {
    Entry* pEntry = mShownEntries[index];
    return pEntry != nullptr && mCurrentEntry == pEntry;
}

inline void CMsgBalloon::AdjustY(BWorldObj* pObject, const nw4r::math::VEC3& rPosition) {
    std::list<Entry>::iterator it;
    for (it = mEntries.begin(); it != mEntries.end(); ++it) {
        if (it->mObject.lock().get() == pObject) {
            it->mPosition = rPosition;
            break;
        }
    }
}

inline void CMsgBalloon::Draw() {
    mUIManager->Draw();
}

inline void CMsgBalloon::CalcView() {
    std::vector<std::tr1::shared_ptr<CPiiTex> >::iterator it;
    if (mCurrentEntry != nullptr && mCurrentEntry->mStageValue != 0xFFFFFFFF) {
        for (it = mModelTextures.begin(); it != mModelTextures.end(); ++it) {
            (*it)->CalcView();
        }
    }
}

inline void CMsgBalloon::CalcMtx() {
    std::vector<std::tr1::shared_ptr<CPiiTex> >::iterator it;
    if (mCurrentEntry != nullptr && mCurrentEntry->mStageValue != 0xFFFFFFFF) {
        for (it = mModelTextures.begin(); it != mModelTextures.end(); ++it) {
            (*it)->CalcMtx();
        }
    }
}

inline void CMsgBalloon::Calc() {
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
                nw4r::math::VEC3 position(it->mPosition);
                std::tr1::shared_ptr<BWorldObj> pObject = it->mObject.lock();
                if (pObject) {
                    if (!pObject->IsVisible()) {
                        continue;
                    }
                    position += pObject->GetPositionRef();
                }
                const nw4r::math::VEC3& rPiiPosition = pPii->GetPositionRef();
                f32 sinValue;
                f32 cosValue;
                nw4r::math::SinCosFIdx(&sinValue, &cosValue, 0.71111112f * pPii->GetRotation());
                f32 dz = position.z - (rPiiPosition.z + 0.33f * cosValue);
                f32 dx = position.x - (rPiiPosition.x + 0.33f * sinValue);
                f32 distance = (dz * dz + dx * dx) * it->mDistanceScale;
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
            mMsgLayout->GetLayout()->Detach();
            mLayout->RemoveChild(mWarpLayout);
            mWarpLayout->GetLayout()->Detach();
            if (pBest->mStageValue != 0xFFFFFFFF) {
                if (pBest->mStageID.mWorld == 7) {
                    SetupBattleWarp(pBest->mStageID);
                } else {
                    SetupStageWarp(pBest->mStageID);
                }
                mWarpLayout->GetLayout()->SetText("TITLE", GetMessageString(gStageWorlds[pBest->mStageID.mWorld].mMessageID, nullptr));
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

inline void CMsgBalloon::SetupBattleWarp(StageID stageID) {
    MapIndex& rIndex = reinterpret_cast<MapIndex&>(stageID);
    int clearCount = GetStageClearCount(rIndex);
    if (stageID.mKind == 2) {
        CLayout* pLayout = mWarpLayout->GetLayout();
        pLayout->SetPaneVisible("BATTLE", false);
        pLayout->SetPaneVisible("KNOWN", false);
        pLayout->SetPaneVisible("UNKNOWN", true);
        pLayout->SetPaneVisible("EXTRA", false);
        pLayout->SetPaneVisible("CLEAR", false);
        int progress;
        if (clearCount > 1) {
            progress = 9;
        } else {
            progress = (s8)GetSaveData(-1)->mData.mStatus[18] * 2 + IsGameFlagSet(30) - 1;
        }
        if (progress > 1) {
            pLayout->SetText("UNKNOWN", FormatMessage(GetMessageString(0x600CC, L"さいこうきろく {1}-{2}"), progress / 2, progress % 2 + 1).c_str());
        }
    } else {
        float clearTime = IGameSession::GetInstance()->GetBattleRoyalBox()->GetData(rIndex.mMode, rIndex.mArea)->GetBestTime();
        int centiseconds = ceil(100.0f * clearTime);
        bool cleared = clearCount > 0;
        CLayout* pLayout = mWarpLayout->GetLayout();
        pLayout->SetPaneVisible("BATTLE", cleared);
        pLayout->SetPaneVisible("KNOWN", false);
        pLayout->SetPaneVisible("UNKNOWN", !cleared);
        pLayout->SetPaneVisible("EXTRA", false);
        pLayout->SetPaneVisible("CLEAR", cleared);
        if (cleared) {
            pLayout->SetText("CLEAR", FormatMessage(GetMessageString(0x600C7, nullptr), clearCount).c_str());
            int minutes = centiseconds / 100 / 60;
            int seconds = centiseconds / 100 % 60;
            int hundredths = centiseconds % 100;
            NIntl::basic_formatter<wchar_t> formatter(GetMessageString(0x70010, L"{1}'{2}\"{3}"));
            formatter % minutes;
            formatter.format(L"%02d") % seconds;
            formatter.format(L"%02d") % hundredths;
            pLayout->SetText("TIME", formatter.c_str());
        }
    }
}

inline void CMsgBalloon::SetupStageWarp(StageID stageID) {
    bool isExtra = stageID.mKind == 2;
    if (!isExtra) {
        SetupStageModels(stageID);
    }
    int clearCount = GetStageClearCount(reinterpret_cast<MapIndex&>(stageID));
    bool cleared = clearCount > 0;
    CLayout* pLayout = mWarpLayout->GetLayout();
    pLayout->SetPaneVisible("BATTLE", false);
    pLayout->SetPaneVisible("KNOWN", !isExtra && cleared);
    pLayout->SetPaneVisible("UNKNOWN", !isExtra && !cleared);
    pLayout->SetPaneVisible("EXTRA", isExtra);
    pLayout->SetPaneVisible("CLEAR", clearCount > 0);
    if (cleared) {
        pLayout->SetText("CLEAR", FormatMessage(GetMessageString(0x600C7, nullptr), clearCount).c_str());
    }
}

inline void CMsgBalloon::SetupStageModels(StageID stageID) {
    std::map<int, bool> unlocked;
    MapIndex& rIndex = reinterpret_cast<MapIndex&>(stageID);
    bool anyCleared = false;
    bool allCleared = true;
    for (int i = 1; i < 10; i++) {
        rIndex.mMap = i;
        MapDesc* pDesc = IGameSession::GetInstance()->GetMapDesc(rIndex);
        if (pDesc == nullptr) {
            continue;
        }
        bool cleared = GetMapRecord(rIndex)->IsFlagSet(2);
        anyCleared |= cleared;
        allCleared &= cleared;
        MapPiiTable* pTable = &pDesc->mPiiTable;
        for (int row = 0; row < pTable->GetRowCount(); row++) {
            if (pTable->GetCell(row, 2)->ToInt() == 7) {
                continue;
            }
            u16 first = pTable->GetCell(row, 0)->ToInt();
            u16 second = pTable->GetCell(row, 1)->ToInt();
            if (1 <= first && first < 496) {
                if (unlocked.find(first) == unlocked.end()) {
                    unlocked[first] = cleared;
                } else {
                    unlocked[first] |= cleared;
                }
            }
            if (1 <= second && second < 496) {
                if (unlocked.find(second) == unlocked.end()) {
                    unlocked[second] = cleared;
                } else {
                    unlocked[second] |= cleared;
                }
            }
        }
    }

    bool stageCleared = IsStageCleared(rIndex);
    unsigned long total = unlocked.size();
    if (total > 18) {
        total = 18;
    }
    mModelTextures.resize(total);

    CPiiCollectionBox* pBox = IGameSession::GetInstance()->GetPiiCollectionBox();
    u32 index = 0;
    std::map<int, bool>::iterator it;
    bool anyCollected = false;
    for (it = unlocked.begin(); it != unlocked.end(); ++it) {
        if (it->second) {
            int collectionType = pBox->GetCollectionType(it->first);
            if (collectionType == 2 || collectionType == 3) {
                anyCollected = true;
            }
        }
    }

    int collectedCount = 0;
    for (it = unlocked.begin(); it != unlocked.end(); ++it) {
        u16 monsno = it->first;
        const PiiSpeciesData* pSpeciesData = GetPiiSpeciesData(monsno, 0);
        std::tr1::shared_ptr<CPiiPersonalData> personalData;
        personalData = CreatePiiPersonalData(IGameSession::GetInstance()->GetOyaInfo(), pSpeciesData, 0);
        std::tr1::shared_ptr<CPiiTex> texture;
        texture.reset(new CPiiTex(personalData, 256, 256));
        mModelTextures[index] = texture;
        mModelLayouts[index]->SetVisible(true);
        int collectionType = pBox->GetCollectionType(monsno);
        mModelLayouts[index]->GetLayout()->SetTexture("PII_TEX", texture->GetTexMap(), -1);
        mModelLayouts[index]->GetLayout()->SetTexture("PII_TEX_GRAY", texture->GetTexMap(), -1);
        bool collected = false;
        bool seen = false;
        bool unknown = false;
        if (anyCollected) {
            if (it->second) {
                switch (collectionType) {
                case 3:
                    collected = true;
                    collectedCount++;
                    break;
                case 2:
                    seen = true;
                    break;
                case 0:
                case 1:
                    unknown = true;
                    break;
                }
            } else {
                unknown = true;
            }
        }
        mModelLayouts[index]->GetLayout()->SetPaneVisible("PII_TEX", collected);
        mModelLayouts[index]->GetLayout()->SetPaneVisible("PII_TEX_GRAY", seen);
        nw4r::lyt::Pane* pPane = mModelLayouts[index]->GetLayout()->FindPane("NOT_YET");
        if (pPane != nullptr) {
            mModelLayouts[index]->GetLayout()->SetVisible(pPane, collected);
            const char* pMotion = collected ? "pii_not_yet" : "init";
            mModelLayouts[index]->GetLayout()->CreateMotion(pMotion, pPane, 0.0f, 1.0f);
        }
        mModelLayouts[index]->GetLayout()->SetPaneVisible("PII_UNKNOWN", unknown);
        index++;
        if (index >= 18) {
            break;
        }
    }

    for (; index < 18; index++) {
        mModelLayouts[index]->SetVisible(false);
    }
    total = unlocked.size();
    mWarpLayout->GetLayout()->SetText("OBTAIN", FormatMessage(GetMessageString(0x600C9, nullptr), collectedCount, total).c_str());
}

inline void CMsgBalloon::UpdateBalloon(BWorldCamera* pCamera, CLayout* pLayout, bool* pBehind) {
    nw4r::math::MTX34 viewMtx;
    nw4r::math::MTX44 projectionMtx;
    pCamera->GetG3dCamera().GetCameraMtx(&viewMtx);
    pCamera->GetG3dCamera().GetProjectionMtx(&projectionMtx);
    nw4r::math::VEC3 position(mOffset);
    TransformPoint(&position, &viewMtx, &position);
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

inline void CMsgBalloon::RegisterWarpzone(BWorldObj* pObject, f32 radius, const StageID& rStageID) {
    std::list<Entry>::iterator it;
    Entry entry;
    mEntries.push_back(Entry());
    it = --mEntries.end();
    it->mObject = pObject->shared_from_this();
    it->mPosition.x = 0.0f;
    it->mPosition.y = 0.0f;
    it->mPosition.z = 0.0f;
    it->mDistanceScale = 1.0f / (radius * radius);
    it->mText = L"";
    it->mStageID = rStageID;
}

inline void CMsgBalloon::Show(bool show) {
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

inline void CMsgBalloon::Init(bool flag) {
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

std::tr1::shared_ptr<IMsgBalloon> IMsgBalloon::Create(bool flag) {
    std::tr1::shared_ptr<CMsgBalloon> balloon(new CMsgBalloon);
    balloon->Init(flag);
    return balloon;
}


