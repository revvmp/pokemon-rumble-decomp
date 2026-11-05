#include <memory>
#include <string>
#include <game/world/CDividedGround.h>
#include <game/world/CItem.h>
#include <game/pokemon/CPii.h>
#include <game/io/CFileData.h>
#include <game/world/IGameMap.h>
#include <game/world/IGameStage.h>

#include <game/world/BWorld.h>
#include <game/session/CPlayer.h>
#include <game/world/CShadowCamera.h>
#include <game/script/CPpuEngine.h>
#include <game/ui/IBattleRoyalDisplay.h>
#include <game/ui/IComboDisplay.h>
#include <game/ui/IDlg.h>
#include <game/ui/IDlgCollection.h>
#include <game/ui/IDlgInfo.h>
#include <game/ui/IDlgMsgWin.h>
#include <game/ui/IDlgPiiList.h>
#include <game/ui/IDlgPiiSelector.h>
#include <game/ui/IEnemyInfo.h>
#include <game/ui/CEnemyInfo.h>
#include <game/session/IGameSession.h>
#include <game/ui/ILayoutManager.h>
#include <game/ui/IMiniMap.h>
#include <game/ui/IMoneyLifeDisplay.h>
#include <game/ui/CMsgBalloon.h>
#include <game/ui/CNoticeDisplay.h>
#include <game/ui/INumericInput.h>
#include <game/ui/IPauseMenu.h>
#include <game/graphics/IPostEffect.h>
#include <game/ui/ITutorialMsg.h>
#include <game/ui/IWorldObjStatDisplay.h>
#include <game/world/MapDesc.h>
#include <game/script/NPpuVM.h>
#include <game/io/SaveData.h>
#include <game/world/StageID.h>
#include <game/text/StringFormat.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <algorithm>
#include <vector>

namespace {

enum { TYPE_HIDDEN_MODEL_OBJ = 1 << 16 };

class CGameMap : public IGameMap {
public:
    explicit CGameMap(const Param& rParam);
    virtual ~CGameMap();
    void Initialize();
    void Finalize() {}
    void LoadStageResources();

    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void RunAction();
    virtual const MapDesc& GetMapDesc() const { return *mParam.mMapDesc; }
    virtual BWorld* GetWorld() const { return mWorld.get(); }
    virtual IWorldObjStatDisplay* GetWorldObjStatDisplay() { return mWorldObjStatDisplay.get(); }
    virtual IMiniMap* GetMiniMap() { return mMiniMap.get(); }
    virtual IDlgPiiSelector* GetDlgPiiSelector() { return mDlgPiiSelector.get(); }
    virtual IDlgPiiList* GetDlgPiiList() { return mDlgPiiList.get(); }
    virtual IDlgCollection* GetDlgCollection() { return mDlgCollection.get(); }
    virtual IDlgInfo* GetDlgInfo() { return mDlgInfo.get(); }
    virtual IDlgMsgWin* GetDlgMsgWin() { return mDlgMsgWin.get(); }
    virtual INoticeDisplay* GetNoticeDisplay() { return mNoticeDisplay.get(); }
    virtual IMoneyLifeDisplay* GetMoneyLifeDisplay() { return mMoneyLifeDisplay.get(); }
    virtual IBattleRoyalDisplay* GetBattleRoyalDisplay() { return mBattleRoyalDisplay.get(); }
    virtual IComboDisplay* GetComboDisplay() { return mComboDisplay.get(); }
    virtual ILayoutManager* GetLayoutManager() { return mLayoutManager.get(); }
    virtual IPauseMenu* GetPauseMenu() { return mPauseMenu.get(); }
    virtual ITutorialMsg* GetTutorialMsg() { return mTutorialMsg.get(); }
    virtual IMsgBalloon* GetMsgBalloon() { return mMsgBalloon.get(); }
    virtual IEnemyInfo* GetEnemyInfo() { return mEnemyInfo.get(); }
    virtual INumericInput* GetNumericInput() { return mNumericInput.get(); }
    virtual IPostEffect* GetPostEffect() { return mPostEffect.get(); }
    virtual IDlg* GetCurrentDialog() { return mCurrentDialog.lock().get(); }
    virtual bool RegisterDialog(std::tr1::shared_ptr<IDlg> dialog);
    virtual void UnregisterDialog(const std::tr1::shared_ptr<IDlg>& rDialog);
    virtual void SetPausing(bool pausing) { mPausing = pausing; }
    virtual bool IsPausing();
    virtual void Hitstop(int frames) { mHitstopFrames = std::max(frames, mHitstopFrames); }
    virtual bool IsHitstopping() { return mWorld->IsFlag(BWorld::FLAG_SKIP_STATE_4); }
    virtual void EnterEvent() { mWorld->RequestPause(); }
    virtual void ExitEvent() { mWorld->RequestResume(); }
    virtual bool IsInEvent() { return mWorld->IsFlag(BWorld::FLAG_PAUSED); }
    virtual void StartTimer(int frames, int step)
    {
        mTimerFrame = frames;
        mTimerStep = step;
    }
    virtual void AddTimer(int frames)
    {
        mTimerFrame += frames;
        if (frames > 0) {
            mBattleRoyalDisplay->Finish();
        }
    }
    virtual void StopTimer() { mTimerStep = 0; }
    virtual bool IsTimerWorking() { return mTimerStep != 0; }
    virtual int GetTimerFrame() { return mTimerFrame; }
    virtual void DisappearModelObjs();
    virtual void AppearModelObjs();

private:
    Param mParam;
    bool mPausing;
    std::tr1::weak_ptr<IDlg> mCurrentDialog;
    int mTimerFrame;
    int mTimerStep;
    int mHitstopFrames;
    bool mModelObjsHidden;
    std::vector<u32> mHiddenObjIDs;
    std::tr1::shared_ptr<BWorld> mWorld;
    std::tr1::shared_ptr<IPostEffect> mPostEffect;
    std::tr1::shared_ptr<IWorldObjStatDisplay> mWorldObjStatDisplay;
    std::tr1::shared_ptr<IMiniMap> mMiniMap;
    std::tr1::shared_ptr<IDlgPiiSelector> mDlgPiiSelector;
    std::tr1::shared_ptr<IDlgPiiList> mDlgPiiList;
    std::tr1::shared_ptr<IDlgCollection> mDlgCollection;
    std::tr1::shared_ptr<IDlgInfo> mDlgInfo;
    std::tr1::shared_ptr<IMoneyLifeDisplay> mMoneyLifeDisplay;
    std::tr1::shared_ptr<IBattleRoyalDisplay> mBattleRoyalDisplay;
    std::tr1::shared_ptr<IComboDisplay> mComboDisplay;
    std::tr1::shared_ptr<INoticeDisplay> mNoticeDisplay;
    std::tr1::shared_ptr<ILayoutManager> mLayoutManager;
    std::tr1::shared_ptr<IDlgMsgWin> mDlgMsgWin;
    std::tr1::shared_ptr<IPauseMenu> mPauseMenu;
    std::tr1::shared_ptr<ITutorialMsg> mTutorialMsg;
    std::tr1::shared_ptr<IMsgBalloon> mMsgBalloon;
    std::tr1::shared_ptr<IEnemyInfo> mEnemyInfo;
    std::tr1::shared_ptr<INumericInput> mNumericInput;
};

CGameMap::CGameMap(const Param& rParam)
    : mParam(rParam), mPausing(true), mTimerFrame(0), mTimerStep(0),
      mHitstopFrames(0), mModelObjsHidden(false)
{
}

CGameMap::~CGameMap()
{
    Finalize();
}

void CGameMap::Calc()
{
    CPpuEngine::SetDefaultEnvironment(mParam.mEnvironment);
    MapIndex target = mParam.mMapDesc->mIndex;
    CPpuEngine::SetDefaultStageDescTarget(target);
    mWorld->SetOutlineAlpha(255);
    mWorld->SetOutlineWidth(1);
    if (mHitstopFrames > 0) {
        if (!mWorld->IsFlag(BWorld::FLAG_SKIP_STATE_4)) {
            for (int i = 0; i < 4; i++) {
                IGameSession::GetInstance()->GetPlayer(i)->BeginHoldCollection();
            }
        }
        mWorld->SetFlag(BWorld::FLAG_SKIP_STATE_4, true);
        mHitstopFrames--;
    } else {
        mWorld->SetFlag(BWorld::FLAG_SKIP_STATE_4, false);
    }
    if (!IsPausing()) {
        mTimerFrame += mTimerStep;
    }
    mWorld->Calc();
    if (!mWorld->IsFlag(BWorld::FLAG_SKIP_STATE_4)) {
        for (int i = 0; i < 4; i++) {
            IGameSession::GetInstance()->GetPlayer(i)->EndHoldCollection();
        }
    }
    mPostEffect->Calc();
    mWorldObjStatDisplay->Calc();
    mMiniMap->Calc();
    mEnemyInfo->Calc();
    if (mDlgPiiList) {
        mDlgPiiList->Calc();
    }
    if (mDlgCollection) {
        mDlgCollection->Calc();
    }
    if (mDlgInfo) {
        mDlgInfo->Calc();
    }
    mMoneyLifeDisplay->Calc();
    mMsgBalloon->Calc();
    mBattleRoyalDisplay->Calc();
    mComboDisplay->Calc();
    mNoticeDisplay->Calc();
    mLayoutManager->Calc();
    mDlgMsgWin->Calc();
    mTutorialMsg->Calc();
    mDlgPiiSelector->Calc();
    mNumericInput->Calc();
}

bool CGameMap::IsPausing()
{
    if (IsStage7EvenMap(GetMapDesc().mIndex)) {
        for (int i = 0; i < 4; i++) {
            if (mDlgPiiSelector->IsShown(i)) {
                return true;
            }
        }
    }
    return mPausing;
}

void CGameMap::CalcMtx()
{
    mWorld->CalcMtx();
    mPostEffect->CalcMtx();
    mWorldObjStatDisplay->CalcMtx();
    mMiniMap->CalcMtx();
    mEnemyInfo->CalcMtx();
    if (mDlgPiiList) {
        mDlgPiiList->CalcMtx();
    }
    if (mDlgCollection) {
        mDlgCollection->CalcMtx();
    }
    if (mDlgInfo) {
        mDlgInfo->CalcMtx();
    }
    mMoneyLifeDisplay->CalcMtx();
    mMsgBalloon->CalcMtx();
    mBattleRoyalDisplay->CalcMtx();
    mComboDisplay->CalcMtx();
    mNoticeDisplay->CalcMtx();
    mLayoutManager->CalcMtx();
    mDlgMsgWin->CalcMtx();
    mTutorialMsg->CalcMtx();
    mDlgPiiSelector->CalcMtx();
    mNumericInput->CalcMtx();
    mPauseMenu->CalcMtx();
}

void CGameMap::CalcView()
{
    mWorld->CalcView();
    mPostEffect->CalcView();
    if (!mModelObjsHidden) {
        mWorldObjStatDisplay->CalcView();
    }
    mMiniMap->CalcView();
    mEnemyInfo->CalcView();
    if (mDlgPiiList) {
        mDlgPiiList->CalcView();
    }
    if (mDlgCollection) {
        mDlgCollection->CalcView();
    }
    if (mDlgInfo) {
        mDlgInfo->CalcView();
    }
    mMoneyLifeDisplay->CalcView();
    mMsgBalloon->CalcView();
    mBattleRoyalDisplay->CalcView();
    mComboDisplay->CalcView();
    mNoticeDisplay->CalcView();
    mLayoutManager->CalcView();
    mDlgMsgWin->CalcView();
    mTutorialMsg->CalcView();
    mDlgPiiSelector->CalcView();
    mNumericInput->CalcView();
    mPauseMenu->CalcView();
}

void CGameMap::Draw()
{
    mWorld->Draw();
    mPostEffect->Draw();
    if (!mModelObjsHidden) {
        mWorldObjStatDisplay->Draw();
    }
    mMiniMap->Draw();
    mMsgBalloon->Draw();
    mEnemyInfo->Draw();
    if (mDlgPiiList) {
        mDlgPiiList->Draw();
    }
    if (mDlgCollection) {
        mDlgCollection->Draw();
    }
    if (mDlgInfo) {
        mDlgInfo->Draw();
    }
    mMoneyLifeDisplay->Draw();
    mBattleRoyalDisplay->Draw();
    mComboDisplay->Draw();
    mNoticeDisplay->Draw();
    mLayoutManager->Draw();
    mDlgMsgWin->Draw();
    mTutorialMsg->Draw();
    mDlgPiiSelector->Draw();
    mNumericInput->Draw();
}

int GetStageNumber(MapDesc* pDesc)
{
    MapIndex index = pDesc->mIndex;
    MapDesc* pTarget = pDesc;
    MapIndex target;
    if (index.mMode == 2 && index.mStage == 7) {
        index.mMode = !IsBattleRoyalFirstPart(index);
        index.mArea = static_cast<u8>(GetSaveData(-1)->mData.mExtraBRCurrentRound);
        index.mMap = (index.mMap - 1) % 2 + 1;
        target = index;
        pTarget = IGameSession::GetInstance()->GetMapDesc(target);
    }
    return pTarget->mParams.GetValue(0).ToInt();
}

void CGameMap::Initialize()
{
    if (GetSaveData(-1)) {
        MapIndex index = mParam.mMapDesc->mIndex;
        int area = index.mArea;
        GetSaveData(-1)->mData.mArea = area;
        int mode = index.mMode;
        GetSaveData(-1)->mData.mMode = mode;
        switch (mode) {
        case 0:
            GetSaveData(-1)->mData.mNormalArea = area;
            break;
        case 1:
            GetSaveData(-1)->mData.mExtraArea = area;
            break;
        }
    }
    BWorld::Desc worldDesc;
    worldDesc.SetDrawType(1);
    worldDesc.SetObjectCapacity(1024);
    worldDesc.SetOutlineCapacity(worldDesc.GetObjectCapacity());
    worldDesc.SetModelCapacity(worldDesc.GetObjectCapacity() + 128);
    worldDesc.SetScnObjCapacity(worldDesc.GetOutlineCapacity() + worldDesc.GetModelCapacity() + 512);
    worldDesc.SetEffectCapacity(worldDesc.GetObjectCapacity());
    worldDesc.SetCaptureEnable(true);
    worldDesc.SetSoundListenerEnable(true);
    mWorld.reset(new BWorld(worldDesc));
    std::tr1::shared_ptr<CShadowCamera> shadowCamera;
    shadowCamera.reset(new CShadowCamera(4));
    shadowCamera->Setup(512, 512, 1, false);
    mWorld->AddCamera(shadowCamera, 1);
    for (int i = 0; i < 4; i++) {
        IGameSession::GetInstance()->GetPlayer(i)->EndHoldCollection();
    }
    LoadStageResources();
    mPostEffect = IPostEffect::Create(true, true);
    mMiniMap = IMiniMap::Create(mParam.mMapDesc, mWorld.get(), 0.0f, true);
    mEnemyInfo = CEnemyInfo::Create(true);
    mDlgMsgWin = IDlgMsgWin::Create(true, true);
    mMoneyLifeDisplay = IMoneyLifeDisplay::Create(mParam.mMapDesc, true);
    mMsgBalloon = IMsgBalloon::Create(true);
    mBattleRoyalDisplay = IBattleRoyalDisplay::Create(true);
    mComboDisplay = CreateComboDisplay(true);
    mNoticeDisplay = CreateNoticeDisplay(1);
    mLayoutManager = ILayoutManager::Create();
    mWorldObjStatDisplay = IWorldObjStatDisplay::Create(mWorld.get());
    mTutorialMsg = ITutorialMsg::Create(true);
    mNumericInput = INumericInput::Create(true);
    mPauseMenu = IPauseMenu::Create(mParam.mMapDesc, false, 1);
    if (mParam.mMapDesc->mIndex.mStage == 0) {
        mDlgPiiList = IDlgPiiList::Create(true);
        mDlgCollection = IDlgCollection::Create(true);
        mDlgInfo = IDlgInfo::Create(true);
    }
    mDlgPiiSelector = IDlgPiiSelector::Create(true);
    std::tr1::shared_ptr<CDividedGround> ground(new CDividedGround(0x8000));
    ground->Load(&reinterpret_cast<const StageID&>(mParam.mMapDesc->mIndex), GetStageNumber(mParam.mMapDesc), true);
    mWorld->AddGround(ground);
}

void CGameMap::LoadStageResources()
{
    MapDesc* pDesc = mParam.mMapDesc;
    int stageNo = pDesc->mParams.GetValue(0).ToInt();
    std::string world = gStageWorlds[pDesc->mIndex.mStage].mName;
    std::string rank = gStageRanks[pDesc->mIndex.mArea].mName;
    std::string number = FormatString("%02d", stageNo);
    std::string lights[3];
    if (pDesc->mIndex.mMode == 2) {
        rank = "EX";
    }
    lights[0] = "light_" + rank + number;
    lights[1] = "light_" + rank;
    lights[2] = "light_" + number;
    const std::tr1::shared_ptr<CFileData>& stage = LoadFile(("5:/stage/" + world + "/" + number + ".arc/stage.brres").c_str(), 0, true);
    std::tr1::shared_ptr<CFileData> common;
    std::tr1::shared_ptr<CFileData> file = stage;
    for (int i = 0; i < 2; i++) {
        if (file && file->IsLoaded()) {
            for (int j = 0; j < 3; j++) {
                if (lights[j].size() != 0) {
                    nw4r::g3d::ResFile resFile(file->GetData());
                    nw4r::g3d::ResAnmScn scene = resFile.GetResAnmScn(lights[j].c_str());
                    if (scene.IsValid()) {
                        mWorld->LoadLightScene(file->GetPath().c_str(), lights[j].c_str(), true);
                        mWorld->GetShadowCamera()->SelectLight();
                        return;
                    }
                }
            }
        }
        common = LoadResourceFile(("5:/stage/" + world + "/common.arc/stage.brres").c_str(), 0, true);
        file = common;
    }
    mWorld->GetShadowCamera()->SelectLight();
}

void CGameMap::RunAction()
{
    CPpuEngine::SetDefaultEnvironment(mParam.mEnvironment);
    MapIndex target = mParam.mMapDesc->mIndex;
    CPpuEngine::SetDefaultStageDescTarget(target);
    CDividedGround* pGround = DynamicCast<CDividedGround>(mWorld->GetGround());
    pGround->LoadPkc(1, std::tr1::shared_ptr<CPpuEnv>(), nullptr);
}

bool CGameMap::RegisterDialog(std::tr1::shared_ptr<IDlg> dialog)
{
    if (!dialog) {
        return false;
    }
    std::tr1::shared_ptr<IDlg> pCurrent = mCurrentDialog.lock();
    if (pCurrent.get() == dialog.get()) {
        return true;
    }
    if (pCurrent && !pCurrent->Close()) {
        return false;
    }
    pCurrent.reset();
    mCurrentDialog = dialog;
    return true;
}

void CGameMap::UnregisterDialog(const std::tr1::shared_ptr<IDlg>& rDialog)
{
    std::tr1::shared_ptr<IDlg> pCurrent = mCurrentDialog.lock();
    if (pCurrent && pCurrent.get() == rDialog.get()) {
        mCurrentDialog.reset();
    }
}

void CGameMap::DisappearModelObjs()
{
    mModelObjsHidden = true;
    std::vector<BWorldObj*> objs = mWorld->GetObjects(CPii::TYPE | CItem::TYPE | TYPE_HIDDEN_MODEL_OBJ);
    for (std::vector<BWorldObj*>::iterator it = objs.begin(); it != objs.end(); ++it) {
        if ((*it)->IsVisible()) {
            mHiddenObjIDs.push_back((*it)->GetId());
            (*it)->SetVisible(false);
        }
    }
}

void CGameMap::AppearModelObjs()
{
    for (std::vector<u32>::iterator it = mHiddenObjIDs.begin(); it != mHiddenObjIDs.end(); ++it) {
        BWorldObj* pObj = mWorld->GetObject(*it);
        if (pObj) {
            pObj->SetVisible(true);
        }
    }
    mHiddenObjIDs.clear();
    mModelObjsHidden = false;
}

}

std::tr1::shared_ptr<IGameMap> IGameMap::Create(const Param& rParam)
{
    std::tr1::shared_ptr<CGameMap> map(new CGameMap(rParam));
    map->Initialize();
    return map;
}

IGameMap* GetCurrentGameMap()
{
    IGameStage* pStage = GetCurrentGameStage();
    return pStage != nullptr ? pStage->GetGameMap() : nullptr;
}

IGameMap* GetActiveGameMap()
{
    IGameStage* pStage = GetCurrentGameStage();
    return pStage != nullptr ? pStage->GetGameMap() : nullptr;
}
