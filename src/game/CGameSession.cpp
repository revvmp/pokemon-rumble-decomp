#include <game/IGameSession.h>

#include <game/CBattleRoyalBox.h>
#include <game/COyaInfo.h>
#include <game/CPiiBox.h>
#include <game/CPiiCollectionBox.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/CStatistic.h>
#include <game/CControllerComposition.h>
#include <game/ControllerTable.h>
#include <game/IController.h>
#include <game/IDisconMsg.h>
#include <game/CFiber.h>
#include <game/CPpuEngine.h>
#include <game/IGameStage.h>
#include <game/ISaveManager.h>
#include <game/MapIndex.h>
#include <game/Message.h>
#include <game/Pad.h>
#include <game/RenderPrimitive.h>
#include <game/MathUtil.h>
#include <game/SystemMessage.h>
#include <algorithm>
#include <limits>
#include <list>
#include <game/PiiPropData.h>
#include <game/SaveData.h>
#include <game/StageDesc.h>
#include <game/StageID.h>
#include <game/WazaDescData.h>
#include <memory>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/os.h>
#include <set>
#include <string>
#include <vector>
#include <game/CPpuEnv.h>

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
    u32 mMoney;
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
    virtual std::tr1::shared_ptr<CPpuEnv> GetEnvironment();
    virtual int AddPPDToPlayerPiiBox(int player, std::tr1::shared_ptr<CPiiPersonalData> ppd);
    virtual bool CanAddPPDToPlayerPiiBox(int player, std::tr1::shared_ptr<CPiiPersonalData> ppd);
    virtual bool IsReplacePiiAvailable(int player);
    virtual f32 GetPlayTimeSeconds();
    virtual int GetMapChangeType();
    virtual void SetFade(int type, int frames);
    virtual int GetFadeFrames();
    virtual void RequestMapChange(MapIndex map, int type, int frames);
    virtual void RequestSceneChange(int type, int frames);
    virtual void StartMapTransitionOut();
    virtual void StartMapTransitionIn();
    virtual void UpdateMapTransition();
    virtual bool IsMapChangeFinished();
    virtual IDisconMsg* GetDisconMsg();
    virtual bool IsControllerAssigned(const IController* pController);
    virtual void RegisterTeamPiiCollection();

    void ClearInstance();
    void UpdateFade();
    void InitStageDescs();
    void RunStage();
    const CPiiBox* GetTeamPiiBoxView() const { return mTeamPiiBox.get(); }
    void LoadFromSlot();
    void GetSnapshot(SessionSnapshot* pSnapshot);
    void SaveToSlot();

private:
    s64 mPlayTime;
    s64 mStartTime;
    u32 mMoney;
    u32 mLoadedMoney;
    COyaInfo mOyaInfo;
    std::tr1::shared_ptr<CPiiBox> mTeamPiiBox;
    std::tr1::shared_ptr<CPlayer> mPlayers[4];
    std::tr1::shared_ptr<CRemoteMemory> mRemoteMemories[4];
    std::tr1::shared_ptr<CPpuEnv> mEnvironment;
    WazaDescData mWazaDescData;
    PiiPropData mPiiPropData;
    std::tr1::shared_ptr<CPiiCollectionBox> mPiiCollectionBox;
    std::tr1::shared_ptr<CBattleRoyalBox> mBattleRoyalBox;
    std::tr1::shared_ptr<CStatistic> mStatistic;
    StageDesc mStageDescs[3][5][8];
    MapIndex mPendingMap;
    std::tr1::shared_ptr<IGameStage> mGameStage;
    int mMapChangeMode;
    int mMapChangeType;
    nw4r::ut::Color mFadeColor;
    nw4r::ut::Color mFadeTargetColor;
    int mFadeFrames;
    std::tr1::shared_ptr<ISaveManager> mSaveManager;
    IDisconMsg* mpDisconMsg;
};

#pragma force_active on

CGameSession* sGameSession;
int sMaxMoney = 9999999;

CGameSession::CGameSession(IDisconMsg* pDisconMsg) :
    mPlayTime(0), mStartTime(0), mMoney(0), mLoadedMoney(0), mPendingMap(MakeMapIndex(0, 1, 0, 1)), mMapChangeMode(0),
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
    pSnapshot->mMoney = mMoney;
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
    GetSaveData(-1)->mData.mHeader[2] = snapshot.mMoney;
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

void CGameSession::LoadFromSlot() {
    SessionSnapshot snapshot;
    snapshot.mPlayTime = GetSaveData(-1)->mData.mTime;
    snapshot.mRegistryCount = GetSaveData(-1)->mData.mRegistryCount;
    snapshot.mMoney = GetSaveData(-1)->mData.mHeader[2];

    snapshot.mPiiCollectionData.reserve(GetSaveData(-1)->mPiiCollectionData.size());
    for (std::vector<SD_PiiCollectionData>::const_iterator it = GetSaveData(-1)->mPiiCollectionData.begin();
         it != GetSaveData(-1)->mPiiCollectionData.end(); ++it) {
        snapshot.mPiiCollectionData.push_back(it->mData);
    }

    snapshot.mPiiPersonalData.reserve(GetSaveData(-1)->mPiiPersonalData.size());
    for (std::vector<SD_PiiPersonalData>::const_iterator it = GetSaveData(-1)->mPiiPersonalData.begin();
         it != GetSaveData(-1)->mPiiPersonalData.end(); ++it) {
        snapshot.mPiiPersonalData.push_back(it->mData);
    }

    snapshot.mBattleRoyalData.reserve(GetSaveData(-1)->mBattleRoyalData.size());
    for (std::vector<SD_BattleRoyalData>::const_iterator it = GetSaveData(-1)->mBattleRoyalData.begin();
         it != GetSaveData(-1)->mBattleRoyalData.end(); ++it) {
        snapshot.mBattleRoyalData.push_back(it->mData);
    }

    snapshot.mStatisticValue1.reserve(GetSaveData(-1)->mStatisticValue1.size());
    for (std::vector<SD_StatisticValue>::const_iterator it = GetSaveData(-1)->mStatisticValue1.begin();
         it != GetSaveData(-1)->mStatisticValue1.end(); ++it) {
        snapshot.mStatisticValue1.push_back(it->mData);
    }

    snapshot.mStatisticValue2.reserve(GetSaveData(-1)->mStatisticValue2.size());
    for (std::vector<SD_StatisticValue>::const_iterator it = GetSaveData(-1)->mStatisticValue2.begin();
         it != GetSaveData(-1)->mStatisticValue2.end(); ++it) {
        snapshot.mStatisticValue2.push_back(it->mData);
    }

    snapshot.mStatisticValue3.reserve(GetSaveData(-1)->mStatisticValue3.size());
    for (std::vector<SD_StatisticValue>::const_iterator it = GetSaveData(-1)->mStatisticValue3.begin();
         it != GetSaveData(-1)->mStatisticValue3.end(); ++it) {
        snapshot.mStatisticValue3.push_back(it->mData);
    }

    mPlayTime = snapshot.mPlayTime;
    mOyaInfo.SetCount(snapshot.mRegistryCount);
    SetMoney(snapshot.mMoney);
    mStartTime = OSGetTime();
    mTeamPiiBox = CPiiBox::Create(snapshot.mPiiPersonalData, 1, 1200, &mOyaInfo);
    mPiiCollectionBox = CPiiCollectionBox::Create(snapshot.mPiiCollectionData);
    mBattleRoyalBox = CBattleRoyalBox::Create(snapshot.mBattleRoyalData);
    mStatistic = CStatistic::Create(snapshot.mStatisticValue1, snapshot.mStatisticValue2, snapshot.mStatisticValue3);
    RegisterTeamPiiCollection();
    if (GetSaveData(-1)->mData.mFlags[0] == 0) {
        GetSaveData(-1)->mData.mFlags[0] = 1;
    }
    if (IsGameFlagSet(3)) {
        if (GetSaveData(-1)->mData.mFlags[2] == 0) {
            GetSaveData(-1)->mData.mFlags[2] = 1;
        }
        reinterpret_cast<StageID&>(mPendingMap) = reinterpret_cast<const StageID&>(MakeMapIndex(GetSaveData(-1)->mData.mFlags[3], GetSaveData(-1)->mData.mFlags[2], 0, 1));
    }
    SetGCRumbleEnabled(GetOption() == 0);
}

void CGameSession::RunStage() {
    mTeamPiiBox->ShowAllSpecs();
    for (int i = 0; i < 4; i++) {
        mPlayers[i]->SaveState();
    }
    mPiiCollectionBox->SaveSnapshot();
    mStatistic->SaveSnapshot();
    mLoadedMoney = mMoney;
    mPlayers[0]->ResetRankingScore();
    mPlayers[1]->ResetRankingScore();
    mPlayers[2]->ResetRankingScore();
    mPlayers[3]->ResetRankingScore();
    mOyaInfo.ApplyEnvironment(true, mEnvironment);
    IGameStage::Param param;
    param.mMapIndex = mPendingMap;
    param.mEnvironment = mEnvironment;
    mGameStage = IGameStage::Create(param);
    mGameStage->Run();
    mGameStage.reset();
    switch (mMapChangeMode) {
    case 0:
    case 1:
        MovePlayerPiisToTeamPiiBox();
        break;
    case 2:
        for (int i = 0; i < 4; i++) {
            mPlayers[i]->RestoreState();
            mPlayers[i]->GetPiiBox().RestoreAll();
        }
        mPiiCollectionBox->RestoreSnapshot();
        mStatistic->RestoreSnapshot();
        mMoney = mLoadedMoney;
        for (int i = 0; i < 4; i++) {
            mRemoteMemories[i]->GetPiiBox()->RestoreAll();
        }
        mTeamPiiBox->RestoreAll();
    case 3:
        SetFade(1, 15);
        while (GetFadeFrames() > 0) {
            YieldFiber();
        }
        SetFade(0, 0);
        StartMapTransitionOut();
        while (!IsMapChangeFinished()) {
            YieldFiber();
        }
        SetFade(1, 0);
        break;
    }
}

void CGameSession::Run() {
    mEnvironment.reset();
restart:
    {
        mWazaDescData.Clear();
        mPiiPropData.Reset();
        InitStageDescs();
        mEnvironment = CreateScriptEnvironment();
        RunScript("StageDescData.pkc", 1, mEnvironment);
        RunScript("WazaDescData.pkc", 1, mEnvironment);
        RunScript("PiiPropData.pkc", 1, mEnvironment);
        RunScript("WazaTableData.pkc", 1, mEnvironment);
        RunScript("CGameSession.pkc", 1, mEnvironment);
        RunScript("CStatistic.pkc", 1, mEnvironment);
        mTeamPiiBox->ApplyEnvironment(true, mEnvironment);
        mEnvironment->CollectGarbage();
        {
            s64 leadId = (static_cast<s64>(GetSaveData(-1)->mData.mLeadPiiIdHigh) << 32) | GetSaveData(-1)->mData.mLeadPiiIdLow;
            std::tr1::shared_ptr<CPiiPersonalData> lead;
            int bestBP = 0;
            for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = GetTeamPiiBoxView()->GetPersonalDataList().begin();
                 it != GetTeamPiiBoxView()->GetPersonalDataList().end(); ++it) {
                if ((*it)->GetCreationTime() == leadId) {
                    lead = *it;
                    break;
                }
                if ((*it)->GetBP() > bestBP) {
                    lead = *it;
                    bestBP = (*it)->GetBP();
                }
            }
            mPlayers[0]->SetPendingPersonalData(std::tr1::shared_ptr<CPiiPersonalData>(lead));
            mPlayers[0]->Join();
        }
        while (true) {
            RunStage();
            mEnvironment->CollectGarbage();
            switch (mMapChangeMode) {
            case 1:
                goto done;
            case 3:
                goto restart;
            }
        }
    }
done:
    mEnvironment.reset();
}

WazaDescData* CGameSession::GetWazaDescData() {
    return &mWazaDescData;
}

PiiPropData* CGameSession::GetPiiPropData() {
    return &mPiiPropData;
}

CPiiCollectionBox* CGameSession::GetPiiCollectionBox() {
    return mPiiCollectionBox.get();
}

CBattleRoyalBox* CGameSession::GetBattleRoyalBox() {
    return mBattleRoyalBox.get();
}

CStatistic* CGameSession::GetStatistic() {
    return mStatistic.get();
}

int CGameSession::GetCollectionType(u16 monsno) {
    int type = mPiiCollectionBox->GetCollectionType(monsno);
    if (type == 3) {
        return 3;
    }
    for (int i = 0; i < 4; i++) {
        std::tr1::shared_ptr<CPlayer> player = mPlayers[i];
        if (!player->HasJoinedFlag()) {
            continue;
        }
        const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList = player->GetPiiBox().GetPersonalDataList();
        for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
            if (monsno == static_cast<u16>((*it)->GetSpeciesData()->GetSpecies())) {
                return 3;
            }
        }
    }
    return type;
}

std::wstring CGameSession::GetWazaName(int waza) {
    if (mWazaDescData.GetColumnCount() > 0 && waza < mWazaDescData.GetRowCount()) {
        NPpu::CValue value = *mWazaDescData.GetCell(waza, 0);
        return GetMessageString(value.ToInt(), nullptr);
    }
    return std::wstring(L"");
}

StageDesc* CGameSession::GetStageDesc(MapIndex index) {
    return &mStageDescs[index.mMode][index.mArea][index.mStage];
}

MapDesc* CGameSession::GetMapDesc(MapIndex index) {
    StageDesc* pStage = GetStageDesc(index);
    if (index.mMap < pStage->mMapDescs.size()) {
        return pStage->mMapDescs[index.mMap].get();
    }
    return nullptr;
}

MapDesc* CGameSession::CreateMapDesc(const MapIndex& rIndex) {
    MapDesc* pExisting = GetMapDesc(rIndex);
    if (pExisting) {
        return pExisting;
    }
    std::tr1::shared_ptr<MapDesc> desc(new MapDesc);
    desc->mIndex = rIndex;
    StageDesc* pStage = GetStageDesc(rIndex);
    if (pStage->mMapDescs.size() <= rIndex.mMap) {
        pStage->mMapDescs.resize(rIndex.mMap + 1);
    }
    pStage->mMapDescs[rIndex.mMap] = desc;
    return desc.get();
}

int CGameSession::MovePlayerPiisToTeamPiiBox() {
    int moved = 0;
    for (int i = 0; i < 4; i++) {
        std::tr1::shared_ptr<CPlayer> player = mPlayers[i];
        CPiiBox& rBox = player->GetPiiBox();
        const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList = rBox.GetPersonalDataList();
        for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
            if (sGameSession->GetTeamPiiBox()->AddPersonalData(*it)) {
                moved++;
            }
        }
        rBox.Clear();
        player->GetPiiBox().RestoreAll();
    }
    for (int i = 0; i < 4; i++) {
        mRemoteMemories[i]->GetPiiBox()->RestoreAll();
    }
    mTeamPiiBox->RestoreAll();
    RegisterTeamPiiCollection();
    return moved;
}

void CGameSession::RegisterTeamPiiCollection() {
    std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList = mTeamPiiBox->GetPersonalDataList();
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = rList.begin(); it != rList.end(); ++it) {
        int monsno = (*it)->GetSpeciesData()->GetSpecies();
        int form = (*it)->GetSpeciesData()->GetFormNumber();
        mPiiCollectionBox->RecordCollection(monsno, form, 3, (*it)->IsRareColor());
    }
}

int CGameSession::GetMoney() {
    return mMoney;
}

void CGameSession::SetMoney(int money) {
    mMoney = std::min(money, sMaxMoney);
}

void CGameSession::AddMoney(int money) {
    int limit = std::numeric_limits<int>::max();
    int current = mMoney;
    int total;
    if (money > 0) {
        total = current + money;
        if (current >= total) {
            total = limit;
        }
    } else if (money < 0) {
        total = current + money;
        if (current > total) {
            total = std::max(current + money, limit);
        } else {
            total = 9999999;
        }
    } else {
        total = current;
    }
    SetMoney(total);
    if (money > 0) {
        mStatistic->Add(-98, money);
    }
}

bool CGameSession::IsAllPiiBoxCapacityFull() {
    u32 count = mTeamPiiBox->GetCount();
    for (int i = 0; i < 4; i++) {
        CPlayer* pPlayer = GetPlayer(i);
        if (pPlayer->HasJoinedFlag()) {
            count += pPlayer->GetPiiBox().GetCount();
        }
    }
    return count >= mTeamPiiBox->GetCapacity();
}

IGameStage* CGameSession::GetGameStage() {
    return mGameStage.get();
}

CPiiBox* CGameSession::GetTeamPiiBox() {
    return mTeamPiiBox.get();
}

CPlayer* CGameSession::GetPlayer(int player) {
    return mPlayers[player].get();
}

CRemoteMemory* CGameSession::GetRemoteMemory(int player) {
    return mRemoteMemories[player].get();
}

COyaInfo* CGameSession::GetOyaInfo() {
    return &mOyaInfo;
}

std::tr1::shared_ptr<CPpuEnv> CGameSession::GetEnvironment() {
    return mEnvironment;
}

int CGameSession::AddPPDToPlayerPiiBox(int player, std::tr1::shared_ptr<CPiiPersonalData> ppd) {
    int result = ppd->GetDisplayKind();
    if (ppd->GetHP() > 0 && 0 <= player && player < 4) {
        if (ppd == GetPlayer(player)->GetPersonalData()) {
            result = 4;
        } else if (!CanAddPPDToPlayerPiiBox(player, ppd)) {
            result = 5;
        }
    }
    return result;
}

bool CGameSession::CanAddPPDToPlayerPiiBox(int player, std::tr1::shared_ptr<CPiiPersonalData> ppd) {
    if (ppd->IsFainted()) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        CPlayer* pPlayer = GetPlayer(i);
        if (pPlayer->IsJoined()) {
            if (pPlayer->GetPersonalData() == ppd) {
                return false;
            }
            if (i != player && pPlayer->GetPendingPersonalData() == ppd) {
                return false;
            }
        }
    }
    return true;
}

bool CGameSession::IsReplacePiiAvailable(int player) {
    CPlayer* pPlayer = GetPlayer(player);
    if (!pPlayer->IsJoined()) {
        return false;
    }
    const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList = pPlayer->GetPiiBox().GetPersonalDataList();
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
        if (CanAddPPDToPlayerPiiBox(player, std::tr1::shared_ptr<CPiiPersonalData>(*it))) {
            return true;
        }
    }
    IController* pController = pPlayer->GetController();
    if (pController && pController->IsConnected()) {
        CRemoteMemory* pRemoteMemory = mRemoteMemories[pController->GetChannel()].get();
        if (pRemoteMemory->HasPiiBox()) {
            const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rRemoteList = pRemoteMemory->GetPiiBox()->GetPersonalDataList();
            for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rRemoteList.begin(); it != rRemoteList.end(); ++it) {
                if (CanAddPPDToPlayerPiiBox(player, std::tr1::shared_ptr<CPiiPersonalData>(*it))) {
                    return true;
                }
            }
        }
    }
    const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rTeamList = mTeamPiiBox->GetPersonalDataList();
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rTeamList.begin(); it != rTeamList.end(); ++it) {
        if (CanAddPPDToPlayerPiiBox(player, std::tr1::shared_ptr<CPiiPersonalData>(*it))) {
            return true;
        }
    }
    return false;
}

void CGameSession::UpdateFade() {
    if (mFadeFrames <= 0) {
        return;
    }
    mFadeFrames--;
    if (mFadeFrames == 0) {
        mFadeColor = mFadeTargetColor;
        return;
    }
    f32 ratio = 1.0f / (mFadeFrames + 1);
    f32 r = mFadeColor.r + ratio * (mFadeTargetColor.r - mFadeColor.r);
    f32 g = mFadeColor.g + ratio * (mFadeTargetColor.g - mFadeColor.g);
    f32 b = mFadeColor.b + ratio * (mFadeTargetColor.b - mFadeColor.b);
    f32 a = mFadeColor.a + ratio * (mFadeTargetColor.a - mFadeColor.a);
    mFadeColor.r = Min(Max(0.0f, r), 255.0f);
    mFadeColor.g = Min(Max(0.0f, g), 255.0f);
    mFadeColor.b = Min(Max(0.0f, b), 255.0f);
    mFadeColor.a = Min(Max(0.0f, a), 255.0f);
    if (mFadeColor.ToU32() == mFadeTargetColor.ToU32()) {
        mFadeFrames = 0;
    }
}

void CGameSession::RequestMapChange(MapIndex map, int type, int frames) {
    MapIndex index = map;
    index.mMap = 1;
    mPendingMap = index;
    mMapChangeType = type;
    if (mGameStage) {
        mGameStage->RequestState(frames);
    }
    mMapChangeMode = 0;
}

void CGameSession::RequestSceneChange(int type, int frames) {
    mMapChangeType = type;
    if (mGameStage) {
        mGameStage->RequestState(frames);
    }
    mMapChangeMode = 1;
}

void CGameSession::CalcMtx() {
    if (mGameStage) {
        mGameStage->CalcMtx();
    }
}

void CGameSession::CalcView() {
    if (mGameStage) {
        mGameStage->CalcView();
    }
}

void CGameSession::Draw() {
    if (mFadeColor.a < 255 && mGameStage) {
        mGameStage->Draw();
    }
    if (mFadeColor.a != 0) {
        const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
        SetupOverlayRendering();
        SetupScreenProjection();
        DrawScreenQuad(0.0f, 0.0f, pRenderMode->fbWidth, pRenderMode->efbHeight, 1.0f, mFadeColor);
    }
}

void CGameSession::StartMapTransitionOut() {
    if (!mSaveManager) {
        mSaveManager = ISaveManager::Create();
    }
    SaveToSlot();
    mSaveManager->RequestSave();
}

void CGameSession::StartMapTransitionIn() {
    if (!mSaveManager) {
        mSaveManager = ISaveManager::Create();
    }
    SaveToSlot();
    mSaveManager->RequestSaveWithoutBackup();
}

void CGameSession::UpdateMapTransition() {
    mSaveManager->RequestSaveCommitOnly();
}

bool CGameSession::IsMapChangeFinished() {
    if (!mSaveManager) {
        return true;
    }
    return mSaveManager->IsIdle();
}

int CGameSession::GetMapChangeType() {
    return mMapChangeType;
}

IDisconMsg* CGameSession::GetDisconMsg() {
    return mpDisconMsg;
}

bool CGameSession::IsControllerAssigned(const IController* pController) {
    for (int i = 0; i < 4; i++) {
        if (mPlayers[i]->GetController() == pController) {
            return true;
        }
    }
    return false;
}

void CGameSession::Calc() {
    for (int i = 0; i < 4; i++) {
        mPlayers[i]->Calc();
    }
    if (mGameStage) {
        mGameStage->Calc();
    }
    UpdateFade();
    std::set<IController*> controllers;
    for (int i = 0; i < 4; i++) {
        CPlayer* pPlayer = mPlayers[i].get();
        if (pPlayer->IsJoined() && pPlayer->GetController()) {
            controllers.insert(pPlayer->GetController());
        }
    }
    std::vector<IController*> controllerList;
    controllerList.reserve(controllers.size());
    controllerList.assign(controllers.begin(), controllers.end());
    GetControllerTable()->SetControllers(controllerList);
}

void CGameSession::SetFade(int type, int frames) {
    if (frames < 0) {
        mFadeFrames = 15;
    } else {
        mFadeFrames = frames;
    }
    switch (type) {
    default:
        mFadeTargetColor = nw4r::ut::Color(0x00000000);
        break;
    case 1:
        mFadeTargetColor = nw4r::ut::Color(0x000000FF);
        break;
    case 2:
        mFadeTargetColor = nw4r::ut::Color(0xFFFFFFFF);
        break;
    }
    if (mFadeFrames == 0) {
        mFadeColor = mFadeTargetColor;
    } else if (mFadeColor.a == 0) {
        mFadeColor.r = mFadeTargetColor.r;
        mFadeColor.g = mFadeTargetColor.g;
        mFadeColor.b = mFadeTargetColor.b;
    }
}

int CGameSession::GetFadeFrames() {
    return mFadeFrames;
}

#pragma force_active reset

}

IGameSession* IGameSession::GetInstance() {
    return sGameSession;
}

IGameSession* GetGameSession() {
    return sGameSession;
}

std::tr1::shared_ptr<IGameSession> IGameSession::Create(IDisconMsg* pDisconMsg) {
    std::tr1::shared_ptr<IGameSession> session(new CGameSession(pDisconMsg));
    return session;
}
