#include <game/CMsgBalloon.h>

#include <game/BUILayout.h>
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
    return &GetSaveData(-1)->mMapRecords[mode][area][stage][map];
}

bool IsStageCleared(MapIndex index) {
    index.mMap = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    return GetMapRecord(index)->IsFlagSet(2);
}

int GetStageClearCount(MapIndex index) {
    int lastMap = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    return Clamp(GetSaveData(-1)->mMapRecords[index.mMode][index.mArea][index.mStage][lastMap].GetClearCount(), 0, 999);
}

std::tr1::shared_ptr<IMsgBalloon> IMsgBalloon::Create(bool flag) {
    std::tr1::shared_ptr<CMsgBalloon> balloon(new CMsgBalloon);
    balloon->Init(flag);
    return balloon;
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
