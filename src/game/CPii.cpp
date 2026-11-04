#include <algorithm>
#include <game/CPii.h>
#include <game/ScnMdlSimpleDataInline.h>

#include <game/CHPBar.h>
#include <game/CMiniMapMarker.h>
#include <game/CPpuEngine.h>
#include <game/CModel.h>
#include <game/BWorld.h>
#include <game/CFiber.h>
#include <game/CPiiPersonalData.h>
#include <game/CPiiBox.h>
#include <game/COyaInfo.h>
#include <game/VideoTiming.h>
#include <game/AnmObjChrRootScale.h>
#include <game/CFileData.h>
#include <game/FrameStep.h>
#include <game/Memory.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <revolution/gx/GXTexture.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/res/g3d_resfile.h>

namespace {
enum {
    SPECIES_MACHAMP = 68,
    SPECIES_SPINDA = 327,
    SPECIES_ARCEUS = 493
};

const f32 sizeScales[5] = {1.0f, 1.0f, 0.9f, 0.85f, 0.85f};
}
void SetSceneObjectEnabled(nw4r::g3d::ScnObj* pScnObj, bool enable);

static std::tr1::shared_ptr<CFileData> LoadFileForMotion(const char* pPath, u32 flags)
{
    return CreateFileData(pPath, flags, false);
}

static std::tr1::shared_ptr<CFileData> LoadResourceFileForMotion(const char* pPath, u32 flags)
{
    return LoadFileForMotion(pPath, flags | 0x2000);
}

CPii::CPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type)
    : BModelObj(type | TYPE)
    , mPiiFlags(0)
    , mPersonalData(personalData)
    , mFollowNode(-1)
{
}

CPii::~CPii() {}

void CPii::Initialize(bool background)
{
    LoadModel(background);
}

bool CPii::LoadScript(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment)
{
    mPpuEngine.reset(new CPpuEngine);
    return mPpuEngine->Load(this, pName, mode, rEnvironment);
}

void CPii::LoadModel(bool background)
{
    const PiiSpeciesData* pSpecies = mPersonalData->GetSpeciesData();
    int species = pSpecies->GetSpecies();
    bool rareColor = mPersonalData->IsRareColor();
    bool plateForm = false;
    if ((u16)species == SPECIES_ARCEUS && (u8)mPersonalData->GetSpeciesData()->GetFormNumber()) {
        plateForm = true;
    }

    std::tr1::shared_ptr<CModel> model(new CModel);
    std::string modelPath("5:/pii_1-151_387-492.arc/");
    modelPath += mPersonalData->GetSpeciesData()->GetModelName(mPersonalData->GetSex());
    modelPath += ".brres";

    const char* fileNames[4] = {nullptr, "5:/pii_1-151_387-492.arc/anim.brres",
                                "5:/pii_1-151_387-492.arc/textures_1-151_387-492.brres", nullptr};
    fileNames[0] = modelPath.c_str();
    CModel::LoadParams params = {fileNames, false, false, 0x284 | ((rareColor || plateForm) ? 3 : 0), 2, nullptr};
    if (!model->Load(params, background)) {
        TerminateFiber(1);
        return;
    }

    ApplySpindaSpots(model);
    if (rareColor) {
        ApplyTextureVariant(model->mModelData, "_C");
    }
    if (plateForm) {
        static const char* sPlateSuffixes[18] = {nullptr, "_BATTLE", "_HIKOU", "_POISON", "_JIMEN", "_IWA",
                                                 "_MUSHI", "_GHOST", "_METAL", nullptr, "_FIRE", "_WATER",
                                                 "_KUSA", "_ELECTRIC", "_SP", "_KOORI", "_DRAGON", "_AKU"};
        const char* pSuffix = sPlateSuffixes[(u8)mPersonalData->GetSpeciesData()->GetFormNumber()];
        if (pSuffix) {
            ApplyTextureVariant(model->mModelData, pSuffix);
        }
    }

    if (!model->SetAimNode("HEAD")) {
        model->SetAimNode("BODY");
    }
    model->DisableAimLimit();
    if ((u16)species == SPECIES_MACHAMP) {
        ModelAim::Parameters aimParams = model->GetAim().GetParameters();
        aimParams.yawInnerMin = 0.6981317f;
        aimParams.yawInnerMax = -aimParams.yawInnerMin;
        model->GetAim().SetParameters(aimParams);
    }
    model->mModelData.GetModel()->SetScnObjOption(nw4r::g3d::ScnMdlSimple::OPTION_IGNORE_ANMCHR_TRANS, 1);
    model->SetMotionCallback(std::tr1::bind(&CPii::OnMotion, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    u32 modelFlags = 2;
    if (mPersonalData->IsBoss() && mPersonalData->GetSpeciesData()->ScalesAsBoss()) {
        modelFlags |= 4;
    }
    ChangeModel(model, modelFlags);

    const char* pShadowName = GetShadowName();
    if (pShadowName) {
        if (!pShadowName) {
            pShadowName = "5:/misc.arc/shadow.brres";
        }
        mShadowModel.Load(pShadowName, 0, nullptr, background);
        mPiiFlags |= 1;
        mPiiFlags &= ~2;
        mShadowModel.GetModel()->SetPriorityDrawXlu(100);
        if (GetWorld() && mShadowModel.IsLoaded()) {
            GetWorld()->GetModelGroup()->PushBack(mShadowModel.GetModel());
        }
    }

    f32 sizeScale = sizeScales[mPersonalData->GetSpeciesData()->GetASize()];
    f32 scaleValue = sizeScale * mPersonalData->GetSpeciesData()->GetModelScale();
    SetScale(nw4r::math::VEC3(scaleValue, scaleValue, scaleValue));
    gFrameTimingReset = true;
}

void CPii::ApplySpindaSpots(std::tr1::shared_ptr<CModel> model)
{
    u16 species = mPersonalData->GetSpeciesData()->GetSpecies();
    if (species == SPECIES_SPINDA) {
        u32 pattern = mPersonalData->GetRandomSeed();
        nw4r::g3d::ScnMdlSimple* pSimple = model->mModelData.GetModel();
        nw4r::g3d::ScnMdl* pScnMdl = pSimple ? nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(pSimple) : nullptr;
        nw4r::g3d::ResMat resMat = pScnMdl->GetResMdl().GetResMat("FACE");
        if (resMat.IsValid()) {
            nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, resMat.GetID());
            if (access.IsValid()) {
                nw4r::g3d::ResTexSrt texSrt = access.GetResTexSrt(false);
                texSrt.ref().flag &= 0xFFFF0000;
                const s8 offsets[4][2] = {{-78, -78}, {77, -78}, {-35, 29}, {30, 29}};
                for (int i = 0; i < 4; i++) {
                    f32 u = 0.01953125f * offsets[i][0];
                    f32 v = -0.015625f * offsets[i][1];
                    int shifted = pattern >> (i * 8);
                    u += 0.08928572f * ((shifted & 0xF) - 8);
                    v += -0.08928572f * (((shifted >> 4) & 0xF) - 8);
                    texSrt.ref().texSrt[i].Tu = u;
                    texSrt.ref().texSrt[i].Tv = v;
                    texSrt.ref().texSrt[i].R = 0.0f;
                    texSrt.ref().texSrt[i].Su = 1.0f;
                    texSrt.ref().texSrt[i].Sv = 1.0f;
                    texSrt.ref().flag |= 7 << (4 * i);
                }
            }
        }
    }
}

void CPii::ApplyTextureVariant(ScnMdlSimpleData& rData, const char* pSuffix)
{
    if (!rData.IsLoaded()) {
        return;
    }
    nw4r::g3d::ScnMdl* pScnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(rData.GetModel());
    if (pScnMdl == nullptr) {
        return;
    }
    nw4r::g3d::ResMdl resMdl = pScnMdl->GetResMdl();
    u32 numMats = resMdl.GetResMatNumEntries();
    for (u32 matIndex = 0; matIndex < numMats; matIndex++) {
        nw4r::g3d::ResMat resMat = resMdl.GetResMat(matIndex);
        u32 numTexs = resMat.GetNumResTexPlttInfo();
        nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, matIndex);
        nw4r::g3d::ResTexObj texObj = access.GetResTexObj(false);
        nw4r::g3d::ResTlutObj tlutObj = access.GetResTlutObj(false);
        for (u32 texIndex = 0; texIndex < numTexs; texIndex++) {
            const nw4r::g3d::ResTexPlttInfo info = resMat.GetResTexPlttInfo(texIndex);
            std::string texName(info.GetTexResName().GetName());
            texName += pSuffix;
            nw4r::g3d::ResFile file;
            nw4r::g3d::ResTex tex;
            for (u32 fileIndex = 0; fileIndex < rData.GetFiles().size(); fileIndex++) {
                file = nw4r::g3d::ResFile(rData.GetFiles()[fileIndex]->GetData());
                tex = file.GetResTex(texName.c_str(), texName.size());
                if (tex.IsValid()) {
                    goto found;
                }
            }
            continue;
        found:
            if ((info.ref().namePltt != 0) != tex.IsCIFmt()) {
                continue;
            }
            nw4r::g3d::ResPltt pltt;
            if (info.ref().namePltt != 0) {
                std::string plttName(info.GetPlttResName().GetName());
                plttName += pSuffix;
                pltt = file.GetResPltt(plttName.c_str(), plttName.size());
                if (!pltt.IsValid()) {
                    continue;
                }
            }
            GXTexObj* pGXTexObj = texObj.GetTexObj(info.ref().mapID);
            void* pTexData;
            u16 width;
            u16 height;
            f32 minLod;
            f32 maxLod;
            GXBool mipmap;
            if (info.ref().namePltt != 0) {
                GXCITexFmt fmtCi;
                tex.GetTexObjCIParam(&pTexData, &width, &height, &fmtCi, &minLod, &maxLod, &mipmap);
                GXInitTexObjCI(pGXTexObj, pTexData, width, height, fmtCi, info.ref().wrap_s, info.ref().wrap_t,
                               mipmap, info.ref().tlutID);
            } else {
                GXTexFmt fmt;
                tex.GetTexObjParam(&pTexData, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
                GXInitTexObj(pGXTexObj, pTexData, width, height, fmt, info.ref().wrap_s, info.ref().wrap_t, mipmap);
            }
            GXInitTexObjLOD(pGXTexObj, info.ref().min_filt, info.ref().mag_filt, minLod, maxLod,
                            info.ref().lod_bias, info.ref().bias_clamp, info.ref().do_edge_lod,
                            info.ref().max_aniso);
            texObj.Validate(info.ref().mapID);
            if (info.ref().namePltt != 0) {
                GXTlutObj* pGXTlutObj = tlutObj.GetTlut(static_cast<GXTlut>(info.ref().mapID));
                GXInitTlutObj(pGXTlutObj, pltt.GetPlttData(), pltt.GetFmt(), pltt.GetNumEntries());
                tlutObj.Validate(static_cast<GXTlut>(info.ref().mapID));
            }
        }
    }
}

const char* CPii::GetShadowName() const
{
    return nullptr;
}

bool CPii::IsVisible() const
{
    return BWorldObj::IsVisible();
}

void CPii::SetPosition(const nw4r::math::VEC3& rPosition)
{
    BModelObj::SetPosition(rPosition);
    mPiiFlags |= 1;
}

void CPii::SetTransform(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity)
{
    BModelObj::SetTransform(rPosition, rVelocity);
    mPiiFlags |= 1;
}

void CPii::SetBaseMtx(const nw4r::math::MTX34& rMtx)
{
    BModelObj::SetBaseMtx(rMtx);
    mPiiFlags |= 1;
}

void CPii::UpdateShadowMtx()
{
    if ((mPiiFlags & 1) && mShadowModel.IsLoaded() && GetModel()) {
        mPiiFlags &= ~1;
        nw4r::math::MTX34 mtx(1.0f, 0.0f, 0.0f, mPosition.x,
                              0.0f, 1.0f, 0.0f, 0.01f,
                              0.0f, 0.0f, 1.0f, mPosition.z);
        mShadowModel.GetModel()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mtx);
    }
}

void CPii::SetHPBar(std::tr1::shared_ptr<CHPBar> hpBar)
{
    mHPBar = hpBar;
    AddDrawLayer(mHPBar);
}

void CPii::SetMiniMapMarker(std::tr1::shared_ptr<CMiniMapMarker> marker)
{
    mMiniMapMarker = marker;
    SetEffectEnable(true);
}

bool CPii::SetTextureAnimation(const char* pName)
{
    if (pName == nullptr) {
        mTextureAnimation.reset();
        return false;
    }

    CModel* pModel = GetModel().get();
    std::vector<std::tr1::shared_ptr<CFileData> >::iterator it = pModel->mModelData.GetFiles().begin();
    nw4r::g3d::ResAnmTexSrt anm;
    for (; it != pModel->mModelData.GetFiles().end(); ++it) {
        anm = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmTexSrt(pName);
        if (anm.IsValid()) {
            goto found;
        }
    }
    return false;
found:
    nw4r::g3d::ResMdl resMdl = GetModel()->mModelData.GetModel()->GetResMdl();
    mTextureAnimation.reset(
        nw4r::g3d::AnmObjTexSrtRes::Construct(GetDefaultAllocator(), nullptr, anm, resMdl, false), DestroyG3dObj);
    mTextureFrame = 0.0f;
    mTextureRate = 1.0f;
    mTextureAnimation->SetFrame(mTextureFrame);
    if (mTextureAnimation->Bind(resMdl)) {
        if (GetModel()->mModelData.GetModel()->SetAnmObj(mTextureAnimation.get(),
                                                         nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_NOT_SPECIFIED)) {
            return true;
        }
    }
    mTextureAnimation.reset();
    return false;
}

bool CPii::OnMotion(CModel*, const std::tr1::shared_ptr<CModel::CMotionProgram>& rMotion)
{
    if (rMotion->IsPlaying() && rMotion->GetUserData() &&
        static_cast<const PiiMotionInfo*>(rMotion->GetUserData())->mHasTextureAnimation) {
        SetTextureAnimation(static_cast<const PiiMotionInfo*>(rMotion->GetUserData())->mName);
        mTextureFrame = rMotion->GetCurrentFrame();
        mTextureRate = rMotion->GetSpeed();
        return true;
    }
    return false;
}

inline const PiiMotionInfo& CPii::GetMotionInfo(int index)
{
    if (index >= 3) {
        return gSharedMotions[index];
    }
    return gPiiMotionSets[mPersonalData->GetSpeciesData()->GetBoneType()].mMotions[index];
}

inline void CPii::LoadMotionResource(const PiiMotionInfo& rInfo)
{
    if (rInfo.mResourceName) {
        std::tr1::shared_ptr<CFileData> file = LoadResourceFileForMotion(rInfo.mResourceName, 0);
        if (file && file->IsLoaded()) {
            GetModel()->AddResourceFile(file);
        }
    }
}

int CPii::GetMotionLengthByIndex(int index)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    return GetModel()->GetMotionLength(rInfo.mName);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::ApplyMotionInfo(
    const std::tr1::shared_ptr<CModel::CMotionProgram>& rProgram, const PiiMotionInfo& rInfo)
{
    if (rProgram) {
        rProgram->SetUserData(&rInfo);
        rProgram->SetLoopTextureAnimation(rInfo.mLoopsTextureAnimation);
        if (rInfo.mHasTextureAnimation) {
            NotifyMotionCallback(rProgram, true, GetModel().get());
        }
    }
    return rProgram;
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::PlayMotion(int index, f32 frame, f32 rate)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    if (index == 0) {
        rate *= mPersonalData->GetSpeciesData()->GetMotionRateScale();
    }
    return ApplyMotionInfo(GetModel()->Play(rInfo.mName, frame, rate), rInfo);
}

inline void CPii::BlendFromWalk(const PiiMotionInfo& rIntro)
{
    LoadMotionResource(rIntro);
    if (rIntro.mName == nullptr) {
        return;
    }
    std::tr1::shared_ptr<CModel::CMotionProgram> current = GetModel()->GetCurrentMotion();
    if (!current) {
        return;
    }
    if (!current->IsPlaying()) {
        return;
    }
    const PiiMotionInfo* pCurrentInfo = static_cast<const PiiMotionInfo*>(current->GetUserData());
    if (pCurrentInfo == nullptr || pCurrentInfo->mId != 1) {
        return;
    }
    f32 currentFrame = current->GetCurrentFrame();
    if (GetModel()->GetMotionLength(rIntro.mName) <= currentFrame) {
        return;
    }
    ApplyMotionInfo(GetModel()->Play(rIntro.mName, currentFrame, current->GetSpeed()), rIntro);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::QueueMotion(int index, f32 frame, f32 rate)
{
    const PiiMotionInfo& rIntro = GetMotionInfo(2);
    LoadMotionResource(rIntro);
    do {
        if (rIntro.mName == nullptr) {
            break;
        }
        std::tr1::shared_ptr<CModel::CMotionProgram> current = GetModel()->GetCurrentMotion();
        if (!current) {
            break;
        }
        if (!current->IsPlaying()) {
            break;
        }
        const PiiMotionInfo* pCurrentInfo = static_cast<const PiiMotionInfo*>(current->GetUserData());
        if (pCurrentInfo == nullptr || pCurrentInfo->mId != 1) {
            break;
        }
        f32 currentFrame = current->GetCurrentFrame();
        if (GetModel()->GetMotionLength(rIntro.mName) <= currentFrame) {
            break;
        }
        ApplyMotionInfo(GetModel()->Play(rIntro.mName, currentFrame, current->GetSpeed()), rIntro);
    } while (false);

    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    if (index == 0) {
        rate *= mPersonalData->GetSpeciesData()->GetMotionRateScale();
    }
    return ApplyMotionInfo(GetModel()->PlayLoop(rInfo.mName, frame, rate), rInfo);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::AppendMotion(int index, f32 frame, f32 rate)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    if (index == 0) {
        rate *= mPersonalData->GetSpeciesData()->GetMotionRateScale();
    }
    return ApplyMotionInfo(GetModel()->PlayOnce(rInfo.mName, frame, rate), rInfo);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::PlayMotionForDuration(int index, f32 frame, f32 duration)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    f32 rate = CalcPlayRate(rInfo.mName, frame, duration);
    return ApplyMotionInfo(GetModel()->Play(rInfo.mName, frame, rate), rInfo);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::QueueMotionForDuration(int index, f32 frame, f32 duration)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    f32 rate = CalcPlayRate(rInfo.mName, frame, duration);
    return ApplyMotionInfo(GetModel()->PlayLoop(rInfo.mName, frame, rate), rInfo);
}

std::tr1::shared_ptr<CModel::CMotionProgram> CPii::AppendMotionForDuration(int index, f32 frame, f32 duration)
{
    const PiiMotionInfo& rInfo = GetMotionInfo(index);
    LoadMotionResource(rInfo);
    f32 rate = CalcPlayRate(rInfo.mName, frame, duration);
    return ApplyMotionInfo(GetModel()->PlayOnce(rInfo.mName, frame, rate), rInfo);
}

bool CPii::IsPlayingMotion(int id)
{
    std::tr1::shared_ptr<CModel::CMotionProgram> current = GetModel()->GetCurrentMotion();
    if (current && current->IsPlaying()) {
        const PiiMotionInfo* pInfo = static_cast<const PiiMotionInfo*>(current->GetUserData());
        if (pInfo && pInfo->mId == id) {
            return true;
        }
    }
    return false;
}

void CPii::FaceHeadForward()
{
    mFollowTarget.reset();
    GetModel()->DisableAimLimit();
}

void CPii::FaceHeadToward(const nw4r::math::VEC3& rTarget)
{
    mFollowTarget.reset();
    GetModel()->SetAimTarget(rTarget, false);
}

void CPii::FaceHeadToward(BWorldObj* pTarget)
{
    if (pTarget) {
        mFollowTarget = pTarget->shared_from_this();
        mFollowNode = -1;
    } else {
        FaceHeadForward();
    }
}

void CPii::FaceHeadToward(BModelObj* pTarget, const char* pNodeName)
{
    if (pTarget) {
        mFollowTarget = pTarget->shared_from_this();
        mFollowNode = -1;
        if (pTarget->GetModel()) {
            nw4r::g3d::ResMdl resMdl = pTarget->GetModel()->mModelData.GetModel()->GetResMdl();
            if (resMdl.IsValid()) {
                nw4r::g3d::ResNode node = resMdl.GetResNode(pNodeName);
                if (node.IsValid()) {
                    mFollowNode = node.GetID();
                }
            }
        }
    } else {
        FaceHeadForward();
    }
}

nw4r::math::VEC3 CPii::GetPosition()
{
    nw4r::math::VEC3 size = GetSize();
    return nw4r::math::VEC3(mPosition.x, mPosition.y + 0.5f * size.y, mPosition.z);
}

void CPii::Calc()
{
    BModelObj::Calc();
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
    std::tr1::shared_ptr<BWorldObj> target = Lock(mFollowTarget);
    if (target) {
        nw4r::math::VEC3 position = target->GetPosition();
        if (mFollowNode != -1) {
            std::tr1::shared_ptr<BModelObj> modelObj = DynamicCast<BModelObj>(target);
            if (modelObj) {
                nw4r::math::MTX34 mtx;
                modelObj->GetModel()->mModelData.GetModel()->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, mFollowNode);
                position = nw4r::math::VEC3(mtx._03, mtx._13, mtx._23);
            }
        }
        GetModel()->SetAimTarget(position, false);
    } else {
        mFollowTarget.reset();
    }
}

void CPii::CalcMtx()
{
    BModelObj::CalcMtx();
    if (mTextureAnimation) {
        mTextureFrame += mTextureRate * gFrameStep;
    }
    bool visible = IsVisible();
    if (mShadowModel.IsLoaded()) {
        nw4r::g3d::ScnMdlSimple* pShadow = mShadowModel.GetModel();
        if (visible) {
            if (mPiiFlags & 2) {
                mPiiFlags &= ~2;
                SetSceneObjectEnabled(pShadow, true);
            }
        } else if (!(mPiiFlags & 2)) {
            mPiiFlags |= 2;
            SetSceneObjectEnabled(pShadow, false);
        }
    }
    if (visible) {
        UpdateShadowMtx();
    }
}

void CPii::UpdateVisible()
{
    if (mTextureAnimation) {
        mTextureAnimation->SetFrame(mTextureFrame);
    }
}

void CPii::OnEnterWorld()
{
    BModelObj::OnEnterWorld();
    if (GetWorld() && mShadowModel.IsLoaded()) {
        GetWorld()->GetModelGroup()->PushBack(mShadowModel.GetModel());
    }
}

void CPii::OnLeaveWorld(BWorld* pWorld)
{
    if (mShadowModel.IsLoaded()) {
        pWorld->GetModelGroup()->Remove(mShadowModel.GetModel());
    }
    BModelObj::OnLeaveWorld(pWorld);
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}

CPiiBox::CPiiBox(int type, int capacity) : mType(type), mCapacity(capacity) {}

CPiiBox::~CPiiBox() {}

bool CPiiBox::AddPersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    if (ppd) {
        ppd->SetInTeam(false);
        ppd->SetInWiiRemote(false);
        switch (mType) {
        case 1:
            ppd->SetInTeam(true);
            break;
        case 2:
            ppd->SetInWiiRemote(true);
            break;
        }
    }
    RemoveItem(ppd);
    if (mPersonalDataList.size() + 1 > mCapacity) {
        return false;
    }
    mPersonalDataList.push_front(ppd);
    return true;
}

bool CPiiBox::RemovePersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    return RemoveItem(ppd);
}

bool CPiiBox::RemoveItem(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        if (it->get() == ppd.get()) {
            mPersonalDataList.erase(it);
            return true;
        }
    }
    return false;
}

void CPiiBox::Clear()
{
    mPersonalDataList.clear();
}

void CPiiBox::RestoreAll()
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->FillUpHP();
    }
}

void CPiiBox::ShowAllSpecs()
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->SetSpecHidden(false);
    }
}

void CPiiBox::ExportPersonalData(std::vector<SD_PiiPersonalData::Data>& rOut)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    std::vector<SD_PiiPersonalData::Data> records;
    records.reserve(mPersonalDataList.size());
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        SD_PiiPersonalData::Data data;
        (*it)->Export(data);
        records.push_back(data);
    }
    std::swap(rOut, records);
}

std::tr1::shared_ptr<CPiiBox> CPiiBox::Create(const std::vector<SD_PiiPersonalData::Data>& rData, int type, int capacity, const COyaInfo* pOyaInfo)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    std::tr1::shared_ptr<CPiiBox> box(new CPiiBox(type, capacity));
    typedef std::reverse_iterator<std::vector<SD_PiiPersonalData::Data>::const_iterator> DataIterator;
    for (DataIterator it = rData.rbegin(); it != rData.rend(); ++it) {
        std::tr1::shared_ptr<CPiiPersonalData> ppd = CreatePiiPersonalData(pOyaInfo, *it);
        if (!ppd) {
            return std::tr1::shared_ptr<CPiiBox>();
        }
        box->AddPersonalData(ppd);
    }
    int serial = 1;
    typedef std::reverse_iterator<Iterator> ListIterator;
    for (ListIterator it(box->GetPersonalDataList().end()); it != ListIterator(box->GetPersonalDataList().begin()); ++it) {
        if ((*it)->GetCreationTime() == 0) {
            (*it)->SetCreationTime(serial);
            ++serial;
        }
    }
    return box;
}

void CPiiBox::ApplyEnvironment(bool enable, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->ExecuteInitScript(enable, rEnvironment);
    }
}


std::tr1::bad_function_call* CreateMissingMotionCallbackError() {
    return new std::tr1::bad_function_call;
}
