#include <game/BUILayout.h>
#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/BWorldCamera.h>
#include <game/CEffect.h>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CModel.h>
#include <game/CPii.h>
#include <game/CPiiBox.h>
#include <game/CPiiCollectionBox.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/CUIManager.h>
#include <game/CUIPiiSpec.h>
#include <game/COffScreenTexture.h>
#include <game/Font.h>
#include <game/IGameResult.h>
#include <game/IGameSession.h>
#include <game/IPauseMenu.h>
#include <game/IPostEffect.h>
#include <game/IUIInput.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/MathUtil.h>
#include <game/Message.h>
#include <game/PiiSpeciesData.h>
#include <game/QuarterTurn.h>
#include <game/RenderMode.h>
#include <game/Sound.h>
#include <revolution/gx/GXTexture.h>
#include <game/WorkBuffer.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scnmdl.h>

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

extern const EffectInfo gEffectInfos[696];
IUIInput* GetUIInput();

namespace {

bool ComparePiiByCreationTime(const std::tr1::shared_ptr<CPiiPersonalData>& rLeft,
                              const std::tr1::shared_ptr<CPiiPersonalData>& rRight)
{
    if (rLeft->GetCreationTime() != rRight->GetCreationTime()) {
        return rLeft->GetCreationTime() < rRight->GetCreationTime();
    }
    return rLeft < rRight;
}

std::tr1::shared_ptr<CPiiPersonalData> FindStrongestPii(const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList)
{
    std::tr1::shared_ptr<CPiiPersonalData> pStrongest;
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
        if (!pStrongest || pStrongest->GetBP() < (*it)->GetBP()) {
            pStrongest = *it;
        }
    }
    return pStrongest;
}

inline nw4r::g3d::ScnMdl* ToScnMdl(nw4r::g3d::ScnObj* pScnObj) {
    return pScnObj ? nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(pScnObj) : nullptr;
}

bool SetMaterialTexture(nw4r::g3d::ScnMdl* pScnMdl, const char* pMaterialName, const char* pTextureName,
                        const GXTexObj& rTexObj)
{
    nw4r::g3d::ResMat mat = pScnMdl->GetResMdl().GetResMat(pMaterialName);
    if (!mat.IsValid()) {
        return false;
    }
    u32 numTextures = mat.GetNumResTexPlttInfo();
    nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, mat.GetID());
    nw4r::g3d::ResTexObj texObj = access.GetResTexObj(false);
    bool replaced = false;
    for (u32 i = 0; i < numTextures; i++) {
        nw4r::g3d::ResTexPlttInfo info = mat.GetResTexPlttInfo(i);
        std::string name(info.GetTexResName().GetName());
        if (name != pTextureName || info.IsCIFmt()) {
            continue;
        }
        GXTexObj* pTexObj = texObj.GetTexObj(info.ref().mapID);
        *pTexObj = rTexObj;
        GXInitTexObjWrapMode(pTexObj, info.ref().wrap_s, info.ref().wrap_t);
        GXInitTexObjFilter(pTexObj, info.ref().min_filt, info.ref().mag_filt);
        texObj.Validate(info.ref().mapID);
        replaced = true;
    }
    return replaced;
}

void KeepStronger(std::tr1::shared_ptr<CPiiPersonalData>& rStrongest, const std::tr1::shared_ptr<CPiiPersonalData>& rCandidate)
{
    if (rCandidate && (!rStrongest || rStrongest->GetBP() < rCandidate->GetBP())) {
        rStrongest = rCandidate;
    }
}

}

int GetStrongestBP()
{
    std::tr1::shared_ptr<CPiiPersonalData> pStrongest;
    for (int player = 0; player < 4; player++) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        if (pPlayer->IsJoined()) {
            KeepStronger(pStrongest, FindStrongestPii(pPlayer->GetPiiBox().GetPersonalDataList()));
            IController* pController = pPlayer->GetController();
            if (pController && pController->IsConnected() && pController->GetType() == IController::TYPE_WII_REMOTE) {
                KeepStronger(pStrongest, FindStrongestPii(IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->GetPiiBox()->GetPersonalDataList()));
            }
        }
    }
    KeepStronger(pStrongest, FindStrongestPii(IGameSession::GetInstance()->GetTeamPiiBox()->GetPersonalDataList()));
    return pStrongest ? pStrongest->GetBP() : 0;
}

namespace {

struct ResultPii {
    unsigned long mPlayer;
    nw4r::math::VEC3 mPositions[2];
    std::tr1::shared_ptr<CPiiPersonalData> mPersonalData;
    std::tr1::shared_ptr<CPii> mPiis[2];
    f32 mScale;
};

class CGameResult : public IGameResult {
public:
    explicit CGameResult(const MapDesc* pDesc)
        : mpMapDesc(pDesc), mUIManager(new CUIManager), mLayout(new BUILayout), mPiiSpec(new CUIPiiSpec), mSelected(0)
    {
        const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
        mCapture.Init(GetWorkBuffer(0)->GetBuffer(), GetWorkBuffer(0)->GetSize(), pRenderMode->fbWidth,
                      pRenderMode->efbHeight, GX_TF_RGB565, GX_CLAMP, GX_NEAR);
    }

    void Load(bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    const char* const* pArchives = gSharedLayoutArchives;
    mLayout->Load(pArchives, "gameresult", pFonts, background);
    mLayout->SetText("time", GetMessageString(0x600F3, nullptr));
    mPiiSpec->Load(pArchives, "piispec", pFonts, background);
    mUIManager->AddChild(mLayout);
    mLayout->AttachChild("PIISPEC", mPiiSpec);
    mPauseMenu = IPauseMenu::Create(mpMapDesc, true, true);

    BWorld::Desc worldDesc;
    worldDesc.SetDrawType(0);
    worldDesc.SetEffectCapacity(128);
    worldDesc.SetObjectCapacity(1024);
    worldDesc.SetOutlineCapacity(512);
    worldDesc.SetModelCapacity(512);
    worldDesc.SetScnObjCapacity(1024);
    worldDesc.SetSoundListenerEnable(true);
    for (int i = 0; i < 2; i++) {
    mWorlds[i].reset(new BWorld(worldDesc));
    mCameras[i].reset(new BWorldCamera(2));
    mWorlds[i]->AddCamera(mCameras[i]);
    mWorlds[i]->SetCurrentCamera(mCameras[i]);
    if (i == 0) {
        mCameras[i]->Load("5:/event/result.arc/event.brres", background);
        mCameras[i]->Play("result", 0.0f, 1.0f);
    }
    }

    {
    std::tr1::shared_ptr<CModel> pModel(new CModel);
    std::string path("5:/event/result.arc/result2.brres");
    const char* fileNames[2] = {nullptr, nullptr};
    fileNames[0] = path.c_str();
    CModel::LoadParams params = {fileNames, false, false, 1, 0, nullptr};
    if (pModel->Load(params, background)) {
        nw4r::g3d::ScnMdl* pScnMdl = ToScnMdl(pModel->GetScnObj());
        SetMaterialTexture(pScnMdl, "TUBE", "ref_dummy", mCapture.GetTexObj());
        std::tr1::shared_ptr<BModelObj> pObj(new BModelObj(0x20));
        pObj->ChangeModel(pModel, 1);
        pObj->SetVisible(true);
        mWorlds[0]->Add(pObj);
    }
    }

    {
    std::tr1::shared_ptr<CModel> pModel(new CModel);
    std::string path("5:/event/result.arc/result.brres");
    const char* fileNames[2] = {nullptr, nullptr};
    fileNames[0] = path.c_str();
    CModel::LoadParams params = {fileNames, false, false, 0, 0, nullptr};
    if (pModel->Load(params, background)) {
        std::tr1::shared_ptr<BModelObj> pObj(new BModelObj(0x20));
        pObj->ChangeModel(pModel, 1);
        pObj->SetVisible(true);
        mWorlds[1]->Add(pObj);
    }
    }

    CollectPiis();
    mSelected = -1;
    SetPiiPositions(1, mSelected);
    UpdatePiiWindow();
    mStrongestBP = GetStrongestBP();
    mPiiSpec->SetVisible(mEntries.size() != 0);
    }
    inline virtual ~CGameResult();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void RunAction();
    virtual const MapDesc& GetMapDesc() const { return *mpMapDesc; }
    virtual BWorld* GetWorld() const { return mWorlds[0].get(); }
    virtual IPauseMenu* GetPauseMenu() { return mPauseMenu.get(); }

private:
    void CollectPiis();
    void SetPiiPositions(int row, int center);
    void UpdatePiiWindow();

    const MapDesc* mpMapDesc;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mLayout;
    std::tr1::shared_ptr<CUIPiiSpec> mPiiSpec;
    std::tr1::shared_ptr<IPostEffect> mPostEffect;
    std::tr1::shared_ptr<IPauseMenu> mPauseMenu;
    std::tr1::shared_ptr<BWorld> mWorlds[2];
    std::tr1::shared_ptr<BWorldCamera> mCameras[2];
    std::vector<ResultPii> mEntries;
    int mSelected;
    int mStrongestBP;
    COffScreenTexture mCapture;
};

inline CGameResult::~CGameResult()
{
}

}

std::tr1::shared_ptr<IGameResult> IGameResult::Create(const MapDesc* pDesc, bool background)
{
    std::tr1::shared_ptr<CGameResult> pResult(new CGameResult(pDesc));
    pResult->Load(background);
    return pResult;
}

namespace {

inline void CGameResult::CollectPiis()
{
    std::map<const void*, unsigned long> playerOf;
    std::vector<std::tr1::shared_ptr<CPiiPersonalData> > piis;
    for (int player = 0; player < 4; player++) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        if (pPlayer->IsJoined()) {
            const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList = pPlayer->GetPiiBox().GetPersonalDataList();
            for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
                std::tr1::shared_ptr<CPiiPersonalData> pPii = *it;
                pPii->SetSpecHidden(false);
                piis.push_back(pPii);
                playerOf[pPii.get()] = player;
            }
        }
    }
    std::sort(piis.begin(), piis.end(), ComparePiiByCreationTime);
    mEntries.reserve(piis.size());
    for (std::vector<std::tr1::shared_ptr<CPiiPersonalData> >::iterator it = piis.begin(); it != piis.end(); ++it) {
        ResultPii entry;
        entry.mPlayer = playerOf[(*it).get()];
        entry.mPersonalData = *it;
        entry.mScale = 0.01f;
        const PiiSpeciesData* pSpecies = entry.mPersonalData->GetSpeciesData();
        switch (static_cast<u16>(pSpecies->GetSpecies())) {
        case 50:
            entry.mScale = 0.04f;
            break;
        case 51:
            entry.mScale = 0.05f;
            break;
        case 93:
            entry.mScale = 0.2f;
            break;
        case 472:
            entry.mScale = 0.35f;
            break;
        case 82:
            entry.mScale = 0.25f;
            break;
        case 83:
            entry.mScale = 0.04f;
            break;
        case 104:
            entry.mScale = 0.03f;
            break;
        case 105:
            entry.mScale = 0.05f;
            break;
        case 407:
            entry.mScale = 0.05f;
            break;
        case 22:
            entry.mScale = 0.1f;
            break;
        case 63:
            entry.mScale = 0.05f;
            break;
        case 116:
            entry.mScale = 0.1f;
            break;
        case 117:
            entry.mScale = 0.03f;
            break;
        case 131:
            entry.mScale = 0.04f;
            break;
        case 140:
            entry.mScale = 0.07f;
            break;
        case 144:
            entry.mScale = 0.08f;
            break;
        case 145:
            entry.mScale = 0.1f;
            break;
        case 146:
            entry.mScale = 0.08f;
            break;
        case 147:
            entry.mScale = 0.1f;
            break;
        case 148:
            entry.mScale = 0.1f;
            break;
        case 149:
            entry.mScale = 0.1f;
            break;
        case 412:
            entry.mScale = 0.01f;
            break;
        case 413:
            entry.mScale = 0.02f;
            break;
        case 442:
            entry.mScale = 0.03f;
            break;
        case 455:
            entry.mScale = 0.1f;
            break;
        case 474:
            entry.mScale = 0.1f;
            break;
        case 477:
            entry.mScale = 0.1f;
            break;
        case 473:
        default:
            if (pSpecies->IsFlying()) {
                entry.mScale = 0.15f;
            } else {
                switch (pSpecies->GetBoneType()) {
                case 3:
                    entry.mScale = 0.15f;
                    break;
                case 4:
                    entry.mScale = 0.15f;
                    break;
                }
            }
            break;
        }
        mEntries.push_back(entry);
    }
}

inline void CGameResult::SetPiiPositions(int row, int center)
{
    for (unsigned long i = 0; i < mEntries.size(); i++) {
        ResultPii& rEntry = mEntries[i];
        int offset = i - center;
        rEntry.mPositions[row].x = offset * (offset < 0 ? 2.0f : 1.0f);
        rEntry.mPositions[row].y = rEntry.mScale;
        rEntry.mPositions[row].z = 0.0f;
    }
}

inline void CGameResult::UpdatePiiWindow()
{
    int i = 0;
    int tail = mEntries.size() - 35;
    for (std::vector<ResultPii>::iterator it = mEntries.begin(); it != mEntries.end(); ++it, ++i) {
        if ((mSelected - 5 <= i && i < mSelected + 30) || tail <= i) {
            if (!it->mPiis[0]) {
                it->mPiis[0].reset(new CPii(it->mPersonalData, 0x40));
                it->mPiis[0]->Initialize(false);
                it->mPiis[0]->SetVisible(true);
                it->mPiis[0]->SetBoundingEnable(false);
                it->mPiis[0]->PlayMotion(0, 0.0f, 1.0f);
                mWorlds[0]->Add(it->mPiis[0]);
                it->mPiis[1].reset(new CPii(it->mPersonalData, 0x40));
                it->mPiis[1]->Initialize(false);
                it->mPiis[1]->SetVisible(true);
                it->mPiis[1]->SetBoundingEnable(false);
                it->mPiis[1]->PlayMotion(0, 0.0f, 1.0f);
                mWorlds[1]->Add(it->mPiis[1]);
            }
            nw4r::math::VEC3 position = it->mPositions[1];
            it->mPiis[0]->SetPosition(position);
            it->mPiis[1]->SetPosition(position);
        } else if (it->mPiis[0]) {
            mWorlds[0]->RemoveObject(it->mPiis[0]);
            mWorlds[1]->RemoveObject(it->mPiis[1]);
            it->mPiis[0].reset();
            it->mPiis[1].reset();
        }
    }
}

enum {
    BUTTON_NEXT = 0x22,
    BUTTON_PREVIOUS = 0x8,
    BUTTON_FINISH = 0x10
};

#pragma dont_inline on
inline void CGameResult::RunAction()
{
    IGameSession::GetInstance()->SetFade(0, -1);
    YieldFiberStarted();
    while (IGameSession::GetInstance()->GetFadeFrames() > 0) {
        YieldFiber();
    }
    CPiiCollectionBox* pCollection = IGameSession::GetInstance()->GetPiiCollectionBox();
    CLayout* pLayout = mLayout->GetLayout();
    pLayout->CreateMotion("title_begin", "TITLE", 0.0f, 1.0f);
    pLayout->QueueMotion("title_loop", "TITLE", 0.0f, 1.0f);
    pLayout->CreateMotion("begin", "NEXT", 0.0f, 1.0f);
    pLayout->QueueMotion("loop", "NEXT", 0.0f, 1.0f);
    bool strongestShown = false;
    bool newShown = false;
    bool piiSpecShown = false;
    std::tr1::weak_ptr<CEffect> pCurrentEffect;
    std::tr1::weak_ptr<CEffect> pPreviousEffect;
    for (;;) {
        bool atEnd = mSelected == static_cast<int>(mEntries.size()) - 1;
        bool canRepeat = atEnd ? false : true;
        IUIInput* pInput = GetUIInput();
        int action;
        for (;;) {
            u32 repeat = !canRepeat ? 0 : pInput->GetRepeat();
            if ((pInput->GetTrig() | repeat) & BUTTON_NEXT) {
                action = 0;
                break;
            }
            repeat = pInput->GetRepeat();
            if ((pInput->GetTrig() | repeat) & BUTTON_PREVIOUS) {
                action = 1;
                break;
            }
            if (pInput->GetTrig() & BUTTON_FINISH) {
                action = 2;
                break;
            }
            YieldFiber();
        }
        int previous = mSelected;
        int steps = 10;
        switch (action) {
        case 0:
            if (atEnd) {
                goto finish;
            }
            mSelected = previous + 1;
            break;
        case 2:
            if (atEnd) {
                goto finish;
            }
            mSelected = mEntries.size() - 1;
            steps = std::min(30, (mSelected - previous) * 10);
            break;
        case 1:
            if (previous <= 0) {
                YieldFiber();
                continue;
            }
            mSelected = previous - 1;
            break;
        }
        ResultPii& rEntry = mEntries[mSelected];
        std::tr1::shared_ptr<CPiiPersonalData> pPii = rEntry.mPersonalData;
        UpdatePiiWindow();
        pPreviousEffect = pCurrentEffect;
        pCurrentEffect.reset();
        if (pPii->GetBP() == mStrongestBP) {
            PlaySound(0x33C);
            CEffect::Param param;
            param.mInfo = &gEffectInfos[57];
            param.mEnable = false;
            CEffect* pEffect = mWorlds[0]->CreateEffect(param, rEntry.mPiis[0].get());
            if (pEffect) {
                pCurrentEffect = pEffect->shared_from_this();
            }
            pLayout->CreateMotion("strongest_begin", "STRONGEST", 0.0f, 1.0f);
            pLayout->QueueMotion("strongest_loop", "STRONGEST", 0.0f, 1.0f);
            strongestShown = true;
        } else {
            if (pPii->GetPrefix()) {
                PlaySound(0x33B);
            } else {
                PlaySound(0x54);
            }
            if (strongestShown) {
                pLayout->CreateMotion("strongest_end", "STRONGEST", 0.0f, 1.0f);
                strongestShown = false;
            }
        }
        nw4r::math::VEC3 target = mCameras[0]->GetPositionRef();
        rEntry.mPiis[0]->FaceHeadToward(target);
        rEntry.mPiis[1]->FaceHeadToward(target);
        mPiiSpec->SetPii(rEntry.mPersonalData, rEntry.mPlayer);
        if (!piiSpecShown) {
            pLayout->CreateMotion("begin", "PIISPEC", 0.0f, 1.0f);
            pLayout->QueueMotion("loop", "PIISPEC", 0.0f, 1.0f);
            piiSpecShown = true;
        }
        int species = pPii->GetSpeciesData()->GetSpecies();
        int form = pPii->GetSpeciesData()->GetFormNumber();
        if (pCollection->GetCollectionType(species) != 3) {
            pCollection->RecordCollection(species, form, 3, pPii->IsRareColor());
            pLayout->CreateMotion("new_begin", "NEW", 0.0f, 1.0f);
            pLayout->QueueMotion("new_loop", "NEW", 0.0f, 1.0f);
            newShown = true;
        } else if (newShown) {
            pLayout->CreateMotion("new_end", "NEW", 0.0f, 1.0f);
            newShown = false;
        }
        SetPiiPositions(0, previous);
        SetPiiPositions(1, mSelected);
        for (int step = 1; step <= steps; step++) {
            f32 t = SinQuarterTurn(static_cast<f32>(step) / static_cast<f32>(steps));
            for (std::vector<ResultPii>::iterator it = mEntries.begin(); it != mEntries.end(); ++it) {
                if (it->mPiis[0]) {
                    nw4r::math::VEC3 position = Lerp(it->mPositions[0], it->mPositions[1], t);
                    it->mPiis[0]->SetPosition(position);
                    it->mPiis[1]->SetPosition(position);
                }
            }
            YieldFiber();
        }
        UpdatePiiWindow();
        std::tr1::shared_ptr<CEffect> pFinished = pPreviousEffect.lock();
        if (pFinished) {
            pFinished->Finalize(0);
        }
    }
finish:
    if (piiSpecShown) {
        pLayout->CreateMotion("end", "PIISPEC", 0.0f, 1.0f);
    }
    if (newShown) {
        pLayout->CreateMotion("new_end", "NEW", 0.0f, 1.0f);
    }
    if (strongestShown) {
        pLayout->CreateMotion("strongest_end", "STRONGEST", 0.0f, 1.0f);
        std::tr1::shared_ptr<CEffect> pFinished = pCurrentEffect.lock();
        if (pFinished) {
            pFinished->Finalize(0);
        }
    }
    pLayout->CreateMotion("end", "NEXT", 0.0f, 1.0f);
    IGameSession::GetInstance()->SetFade(1, -1);
    while (IGameSession::GetInstance()->GetFadeFrames() > 0) {
        YieldFiber();
    }
}


#pragma dont_inline reset

inline void CGameResult::Draw()
{
    mWorlds[1]->Draw();
    mCapture.CopyFromEFB(true);
    mWorlds[0]->Draw();
    mUIManager->Draw();
    if (mPostEffect) {
        mPostEffect->Draw();
    }
}

inline void CGameResult::CalcView()
{
    mWorlds[0]->CalcView();
    mWorlds[1]->CalcView();
    if (mPostEffect) {
        mPostEffect->CalcView();
    }
    mPauseMenu->CalcView();
}

inline void CGameResult::CalcMtx()
{
    mWorlds[0]->CalcMtx();
    mWorlds[1]->CalcMtx();
    if (mPostEffect) {
        mPostEffect->CalcMtx();
    }
    mPauseMenu->CalcMtx();
}

inline void CGameResult::Calc()
{
    mWorlds[0]->SetOutlineAlpha(255);
    mWorlds[1]->SetOutlineAlpha(255);
    mWorlds[0]->Calc();
    mWorlds[1]->Calc();
    nw4r::math::VEC3 position = mCameras[0]->GetPositionRef();
    position.y = -position.y;
    mCameras[1]->SetPosition(position);
    nw4r::g3d::Camera camera = mCameras[0]->GetG3dCamera();
    nw4r::g3d::Camera mirrorCamera = mCameras[1]->GetG3dCamera();
    nw4r::math::MTX34 cameraMtx;
    camera.GetCameraMtx(&cameraMtx);
    nw4r::math::MTX34 invCameraMtx;
    if (PSMTXInverse(cameraMtx, invCameraMtx)) {
        nw4r::g3d::Camera::PostureInfo info;
        info.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
        nw4r::math::VEC3 origin(0.0f, 0.0f, 0.0f);
        nw4r::math::VEC3 eye;
        PSMTXMultVec(invCameraMtx, reinterpret_cast<Vec*>(&origin), reinterpret_cast<Vec*>(&eye));
        nw4r::math::VEC3 down(0.0f, -1.0f, 0.0f);
        nw4r::math::VEC3 downWorld;
        PSMTXMultVec(invCameraMtx, reinterpret_cast<Vec*>(&down), reinterpret_cast<Vec*>(&downWorld));
        nw4r::math::VEC3 up = downWorld - eye;
        info.cameraUp.x = up.x;
        info.cameraUp.y = -up.y;
        info.cameraUp.z = up.z;
        nw4r::math::VEC3 forward(0.0f, 0.0f, -1.0f);
        nw4r::math::VEC3 target;
        PSMTXMultVec(invCameraMtx, reinterpret_cast<Vec*>(&forward), reinterpret_cast<Vec*>(&target));
        info.cameraTarget.x = target.x;
        info.cameraTarget.y = -target.y;
        info.cameraTarget.z = target.z;
        mirrorCamera.SetPosture(info);
        const nw4r::g3d::CameraData& rData = camera.ref();
        if (rData.flags & nw4r::g3d::CameraData::FLAG_PROJ_ORTHO) {
            mirrorCamera.SetOrtho(rData.projTop, rData.projBottom, rData.projLeft, rData.projRight, rData.projNear, rData.projFar);
        } else if (rData.flags & nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM) {
            mirrorCamera.SetFrustum(rData.projTop, rData.projBottom, rData.projLeft, rData.projRight, rData.projNear, rData.projFar);
        } else {
            mirrorCamera.SetPerspective(rData.projFovy, rData.projAspect, rData.projNear, rData.projFar);
        }
    }
    mUIManager->Calc();
    if (mPostEffect) {
        mPostEffect->Calc();
    }
}

}
