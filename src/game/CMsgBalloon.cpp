#include <game/CMsgBalloon.h>

#include <game/BUILayout.h>
#include <game/CUIManager.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/StringFormat.h>
#include <game/IGameSession.h>
#include <game/MapIndex.h>
#include <game/MathUtil.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <game/StageDesc.h>

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
