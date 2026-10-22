#include <game/IGameSession.h>

#include <game/CBattleRoyalBox.h>
#include <game/COyaInfo.h>
#include <game/CPiiBox.h>
#include <game/CPiiCollectionBox.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/CStatistic.h>
#include <game/ControllerTable.h>
#include <game/MapIndex.h>
#include <game/PiiPropData.h>
#include <game/SaveData.h>
#include <game/StageDesc.h>
#include <game/WazaDescData.h>
#include <memory>
#include <nw4r/ut/ut_Color.h>
#include <revolution/os.h>
#include <string>
#include <vector>

class CMapTransition;

namespace NPpu {
class CEnvironment;
}

#pragma force_active on
inline MapIndex MakeMapIndex(int mode, int area, int stage, int map) {
    MapIndex index = {};
    index.mMode = mode;
    index.mArea = area;
    index.mStage = stage;
    index.mMap = map;
    return index;
}
#pragma force_active reset

namespace {

struct SessionSnapshot {
    s64 mPlayTime;
    u32 mHeaderValue;
    u32 mRegistryCount;
    std::vector<SD_PiiPersonalData::Data> mPiiPersonalData;
    std::vector<SD_PiiCollectionData::Data> mPiiCollectionData;
    std::vector<SD_BattleRoyalData::Data> mBattleRoyalData;
    std::vector<SD_StatisticValue::Data> mStatisticValue1;
    std::vector<SD_StatisticValue::Data> mStatisticValue2;
    std::vector<SD_StatisticValue::Data> mStatisticValue3;
};

class CGameSession : public IGameSession {
public:
    explicit CGameSession(IDisconMsg* pDisconMsg);
    virtual ~CGameSession();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void Run();
    virtual WazaDescData* GetWazaDescData();
    virtual PiiPropData* GetPiiPropData();
    virtual CPiiCollectionBox* GetPiiCollectionBox();
    virtual int GetCollectionType(u16 monsno);
    virtual CBattleRoyalBox* GetBattleRoyalBox();
    virtual CStatistic* GetStatistic();
    virtual std::wstring GetWazaName(int waza);
    virtual StageDesc* GetStageDesc(MapIndex index);
    virtual MapDesc* GetMapDesc(MapIndex index);
    virtual MapDesc* CreateMapDesc(const MapIndex& rIndex);
    virtual int MovePlayerPiisToTeamPiiBox();
    virtual int GetMoney();
    virtual void SetMoney(int money);
    virtual void AddMoney(int money);
    virtual IGameStage* GetGameStage();
    virtual bool IsAllPiiBoxCapacityFull();
    virtual CPiiBox* GetTeamPiiBox();
    virtual CPlayer* GetPlayer(int player);
    virtual CRemoteMemory* GetRemoteMemory(int player);
    virtual COyaInfo* GetOyaInfo();
    virtual std::tr1::shared_ptr<NPpu::CEnvironment> GetEnvironment();
    virtual int AddPPDToPlayerPiiBox(int player, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    virtual bool CanAddPPDToPlayerPiiBox(int player, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    virtual bool IsReplacePiiAvailable(int player, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    virtual f32 GetPlayTimeSeconds();
    virtual int GetMapChangeType();
    virtual void SetFade(int type, int frames);
    virtual int GetFadeFrames();
    virtual bool RequestMapChange(MapIndex map, int type, int frames);
    virtual bool RequestSceneChange(int type, int frames);
    virtual void StartMapTransitionOut();
    virtual void StartMapTransitionIn();
    virtual void UpdateMapTransition();
    virtual bool IsMapChangeFinished();
    virtual IDisconMsg* GetDisconMsg();
    virtual bool IsControllerAssigned(const IController* pController);
    virtual void UnanalyzedSlot42();

    void ClearInstance();
    void InitStageDescs();
    void LoadFromSlot();
    void GetSnapshot(SessionSnapshot* pSnapshot);
    void SaveToSlot();

private:
    s64 mPlayTime;
    s64 mStartTime;
    u32 mHeaderValue;
    u32 mLoadedHeaderValue;
    COyaInfo mOyaInfo;
    std::tr1::shared_ptr<CPiiBox> mTeamPiiBox;
    std::tr1::shared_ptr<CPlayer> mPlayers[4];
    std::tr1::shared_ptr<CRemoteMemory> mRemoteMemories[4];
    std::tr1::shared_ptr<NPpu::CEnvironment> mEnvironment;
    WazaDescData mWazaDescData;
    PiiPropData mPiiPropData;
    std::tr1::shared_ptr<CPiiCollectionBox> mPiiCollectionBox;
    std::tr1::shared_ptr<CBattleRoyalBox> mBattleRoyalBox;
    std::tr1::shared_ptr<CStatistic> mStatistic;
    StageDesc mStageDescs[120];
    MapIndex mPendingMap;
    std::tr1::shared_ptr<IGameStage> mGameStage;
    int mMapChangeMode;
    int mMapChangeType;
    nw4r::ut::Color mFadeColor;
    nw4r::ut::Color mFadeTargetColor;
    int mFadeFrames;
    std::tr1::shared_ptr<CMapTransition> mMapTransition;
    IDisconMsg* mpDisconMsg;
};

#pragma force_active on

CGameSession* sGameSession;

CGameSession::CGameSession(IDisconMsg* pDisconMsg) :
    mPlayTime(0), mStartTime(0), mHeaderValue(0), mLoadedHeaderValue(0), mPendingMap(MakeMapIndex(0, 1, 0, 1)), mMapChangeMode(0),
    mMapChangeType(0), mFadeColor(0, 0, 0, 255), mFadeTargetColor(0, 0, 0, 255), mFadeFrames(0), mpDisconMsg(pDisconMsg) {
    sGameSession = this;
    for (int i = 0; i < 4; i++) {
        mPlayers[i].reset(new CPlayer(i));
    }
    mPlayers[0]->SetController(GetController(IController::TYPE_WII_REMOTE, 0));
    for (int i = 0; i < 4; i++) {
        mRemoteMemories[i].reset(new CRemoteMemory(i, &mOyaInfo));
    }
    mRemoteMemories[0]->Update();
    InitStageDescs();
    LoadFromSlot();
}

void CGameSession::ClearInstance() {
    sGameSession = nullptr;
}

CGameSession::~CGameSession() {
    ClearInstance();
}

#pragma dont_inline on
void CGameSession::InitStageDescs() {
    for (int mode = 0; mode < 3; mode++) {
        for (int area = 0; area < 5; area++) {
            for (int stage = 0; stage < 8; stage++) {
                MapIndex index = {};
                index.mMode = mode;
                index.mArea = area;
                index.mStage = stage;
                GetStageDesc(index)->Init(index);
            }
        }
    }
}
#pragma dont_inline reset

f32 CGameSession::GetPlayTimeSeconds() {
    OSTime total = mPlayTime + (OSGetTime() - mStartTime);
    return OSTicksToMicroseconds(total) / 1000000.0;
}

void CGameSession::GetSnapshot(SessionSnapshot* pSnapshot) {
    OSTime now = OSGetTime();
    pSnapshot->mPlayTime = mPlayTime + (now - mStartTime);
    pSnapshot->mHeaderValue = mHeaderValue;
    pSnapshot->mRegistryCount = mOyaInfo.GetCount();
    mTeamPiiBox->ExportPersonalData(pSnapshot->mPiiPersonalData);
    mPiiCollectionBox->ExportCollectionData(pSnapshot->mPiiCollectionData);
    mBattleRoyalBox->ExportBattleRoyalData(pSnapshot->mBattleRoyalData);
    mStatistic->ExportStatisticValues(pSnapshot->mStatisticValue1, pSnapshot->mStatisticValue2, pSnapshot->mStatisticValue3);
}


void CGameSession::SaveToSlot() {
    SessionSnapshot snapshot;
    GetSnapshot(&snapshot);
    GetSaveData(-1)->mData.mTime = snapshot.mPlayTime;
    GetSaveData(-1)->mData.mRegistryCount = snapshot.mRegistryCount;
    GetSaveData(-1)->mData.mHeader[2] = snapshot.mHeaderValue;
    GetSaveData(-1)->mData.mElapsedSeconds = OSTicksToSeconds(OSGetTime());

    std::vector<SD_PiiCollectionData> collection;
    collection.reserve(snapshot.mPiiCollectionData.size());
    for (std::vector<SD_PiiCollectionData::Data>::iterator it = snapshot.mPiiCollectionData.begin();
         it != snapshot.mPiiCollectionData.end(); ++it) {
        SD_PiiCollectionData element;
        element.mData = *it;
        collection.push_back(element);
    }
    GetSaveData(-1)->mPiiCollectionData.swap(collection);

    std::tr1::shared_ptr<CPiiPersonalData> lead;
    for (int i = 0; i < 4; i++) {
        if ((lead = mPlayers[i].get()->GetPersonalData())) {
            break;
        }
        if ((lead = mPlayers[i].get()->GetPendingPersonalData())) {
            break;
        }
    }
    u64 id = 0;
    int monsno = 0;
    if (lead) {
        id = lead->GetCreationTime();
        monsno = lead->GetSpeciesData()->GetSpecies();
    }
    GetSaveData(-1)->mData.mLeadPiiIdHigh = id >> 32;
    GetSaveData(-1)->mData.mLeadPiiIdLow = id;
    SlotData* pSlot = GetSaveData(-1);
    pSlot->mData.mLeadPiiMonsno = monsno;

    std::vector<SD_PiiPersonalData> personal;
    personal.reserve(snapshot.mPiiPersonalData.size());
    for (std::vector<SD_PiiPersonalData::Data>::iterator it = snapshot.mPiiPersonalData.begin();
         it != snapshot.mPiiPersonalData.end(); ++it) {
        SD_PiiPersonalData element;
        element.mData = *it;
        personal.push_back(element);
    }
    GetSaveData(-1)->mPiiPersonalData.swap(personal);

    std::vector<SD_BattleRoyalData> battleRoyal;
    battleRoyal.reserve(snapshot.mBattleRoyalData.size());
    for (std::vector<SD_BattleRoyalData::Data>::iterator it = snapshot.mBattleRoyalData.begin();
         it != snapshot.mBattleRoyalData.end(); ++it) {
        SD_BattleRoyalData element;
        element.mData = *it;
        battleRoyal.push_back(element);
    }
    GetSaveData(-1)->mBattleRoyalData.swap(battleRoyal);

    std::vector<SD_StatisticValue> statistic1;
    for (std::vector<SD_StatisticValue::Data>::iterator it = snapshot.mStatisticValue1.begin();
         it != snapshot.mStatisticValue1.end(); ++it) {
        SD_StatisticValue element;
        element.mData = *it;
        statistic1.push_back(element);
    }
    GetSaveData(-1)->mStatisticValue1.swap(statistic1);

    std::vector<SD_StatisticValue> statistic2;
    for (std::vector<SD_StatisticValue::Data>::iterator it = snapshot.mStatisticValue2.begin();
         it != snapshot.mStatisticValue2.end(); ++it) {
        SD_StatisticValue element;
        element.mData = *it;
        statistic2.push_back(element);
    }
    GetSaveData(-1)->mStatisticValue2.swap(statistic2);

    std::vector<SD_StatisticValue> statistic3;
    for (std::vector<SD_StatisticValue::Data>::iterator it = snapshot.mStatisticValue3.begin();
         it != snapshot.mStatisticValue3.end(); ++it) {
        SD_StatisticValue element;
        element.mData = *it;
        statistic3.push_back(element);
    }
    GetSaveData(-1)->mStatisticValue3.swap(statistic3);
}
#pragma force_active reset

}

IGameSession* IGameSession::GetInstance() {
    return sGameSession;
}

std::tr1::shared_ptr<IGameSession> IGameSession::Create(IDisconMsg* pDisconMsg) {
    std::tr1::shared_ptr<CGameSession> session(new CGameSession(pDisconMsg));
    return session;
}
