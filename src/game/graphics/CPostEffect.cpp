#include <nw4r/ut/ut_algorithm.h>
#include <game/graphics/CPostEffect.h>
#include <game/animation/AnmObjChrRootScale.h>
#include <game/io/CFileData.h>
#include <game/graphics/CModel.h>
#include <game/graphics/Graphics.h>
#include <game/core/Memory.h>
#include <game/graphics/RenderMode.h>
#include <game/graphics/WorkBuffer.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/g3d/g3d_state.h>
#include <string.h>

#include <game/world/WorldOwnerGuard.h>
namespace std { namespace tr1 {
template <> template <> inline shared_ptr<nw4r::g3d::ScnRoot>::shared_ptr(nw4r::g3d::ScnRoot* p, void (*d)(nw4r::g3d::G3dObj*)) : ptr_(p) {
 detail::world_owner_guard<nw4r::g3d::ScnRoot, void (*&)(nw4r::g3d::G3dObj*)> holder(p,d);
 s_ = new detail::shared_ptr_deleter<nw4r::g3d::ScnRoot, void (*)(nw4r::g3d::G3dObj*)>(p,d);
 holder.release();
 enable_from_this(p,p);
}
} }

std::tr1::shared_ptr<IPostEffect> IPostEffect::Create(bool flag, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadFile("5:/post_effect.arc", 0, background);
    std::tr1::shared_ptr<CPostEffect> effect(new CPostEffect(flag, file));
    return effect;
}

CPostEffect::CPostEffect(bool flag, std::tr1::shared_ptr<CFileData> file) : mFile(file), mFlag(flag) {
    mScnRoot.reset(nw4r::g3d::ScnRoot::Construct(GetDefaultAllocator(), nullptr, 1, 4, 128, 128), DestroyG3dObj);
    mScnRoot->SetCurrentCamera(0);
    nw4r::g3d::Camera camera = mScnRoot->GetCamera(0);
    camera.Init();
    nw4r::math::VEC3 position(0.0f, 0.0f, 5.0f);
    camera.SetPosition(position);
    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
    posture.cameraTarget = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    posture.cameraTwist = 0.0f;
    camera.SetPosture(posture);
    camera.SetOrtho(0.5f, -0.5f, -0.5f, 0.5f, 0.1f, 10.0f);
    const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
    mTexture.Init(GetWorkBuffer(0)->GetBuffer(), GetWorkBuffer(0)->GetSize(), pRenderMode->fbWidth, pRenderMode->efbHeight,
                  GX_TF_RGB565, GX_CLAMP, GX_LINEAR);
}

inline void CPostEffect::Draw() {
    if (!mModels.empty()) {
        Graphics::ResetState();
        GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
        GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetAlphaUpdate(GX_FALSE);
        mScnRoot->GetCamera(0);
        for (std::list<std::tr1::shared_ptr<CModel> >::iterator it = mModels.begin(); it != mModels.end(); ++it) {
            mTexture.CopyFromEFB(false);
            mScnRoot->Clear();
            mScnRoot->PushBack((*it)->GetScnObj());
            mScnRoot->CalcWorld();
            mScnRoot->CalcVtx();
            mScnRoot->CalcMaterial();
            mScnRoot->CalcView();
            mScnRoot->GatherDrawScnObj();
            mScnRoot->ZSort();
            mScnRoot->DrawOpa();
            mScnRoot->DrawXlu();
        }
    }
}

inline void CPostEffect::Calc() {
    if (!mModels.empty()) {
        for (std::list<std::tr1::shared_ptr<CModel> >::iterator it = mModels.begin(); it != mModels.end(); ++it) {
            (*it)->UpdateFrame(1.0f);
        }
    }
}

inline void CPostEffect::Hide() {
    mModels.clear();
}

inline void CPostEffect::BindCaptureTexture(std::tr1::shared_ptr<CModel> model) {
    nw4r::g3d::ScnMdl* pScnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(model->GetScnObj());
    nw4r::g3d::ResMdl resMdl = pScnMdl->GetResMdl();
    u32 numMats = resMdl.GetResMatNumEntries();
    for (u32 i = 0; i < numMats; i++) {
        nw4r::g3d::ResMat mat = resMdl.GetResMat(i);
        u32 numTexs = mat.GetNumResTexPlttInfo();
        nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, i);
        nw4r::g3d::ResTexObj texObj = access.GetResTexObj(false);
        for (u32 j = 0; j < numTexs; j++) {
            nw4r::g3d::ResTexPlttInfo info = mat.GetResTexPlttInfo(j);
            if (strcmp(info.GetTexResName().GetName(), "dummy_capture") == 0) {
                *texObj.GetTexObj(info.ref().mapID) = mTexture.GetTexObj();
                texObj.Validate(info.ref().mapID);
            }
        }
    }
}

inline void CPostEffect::Show(int effect) {
    Hide();
    if (effect == EFFECT_NONE) {
        return;
    }
    const EffectInfo& rInfo = gPostEffectInfos[effect];
    std::tr1::shared_ptr<CModel> model(new CModel);
    std::string path = "5:/post_effect.arc/";
    path.append(rInfo.mModelName);
    const char* files[2] = {path.c_str(), nullptr};
    CModel::LoadParams params = {files, false, false, 1, 1, nullptr};
    if (!model->Load(params, false)) {
        return;
    }
    BindCaptureTexture(model);
    if (rInfo.mAnimationName != nullptr) {
        model->Play(rInfo.mAnimationName, 0.0f, 1.0f);
    } else {
        model->Play(0, 0.0f, 1.0f);
    }
    nw4r::math::MTX34 mtx;
    switch (rInfo.mScaleMode) {
    case 0: {
        f32 aspect = GetDisplayAspectRatio();
        mtx = nw4r::math::MTX34(1.0f / rInfo.mWidth, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / rInfo.mWidth * aspect, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    case 1: {
        f32 scale = 1.0f / rInfo.mHeight;
        f32 aspect = GetDisplayAspectRatio();
        mtx = nw4r::math::MTX34(1.0f / (rInfo.mHeight * aspect), 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    case 2:
        mtx = nw4r::math::MTX34(1.0f / rInfo.mWidth, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / rInfo.mHeight, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    model->GetScnObj()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mtx);
    mModels.push_back(model);
}
