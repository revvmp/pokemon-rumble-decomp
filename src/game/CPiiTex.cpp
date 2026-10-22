#include <game/CPiiTex.h>
#include <game/BWorld.h>
#include <game/BWorldCamera.h>
#include <game/CPii.h>
#include <game/Graphics.h>
#include <game/CPiiPersonalData.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/gx.h>
#include <revolution/gx/GXGeometry.h>
#include <revolution/gx/GXTexture.h>

CPiiTex::CPiiTex(u16 width, u16 height) {
    InitTexture(width, height);
}

CPiiTex::CPiiTex(std::tr1::shared_ptr<CPiiPersonalData> personalData, u16 width, u16 height) {
    InitTexture(width, height);
    InitPiis(personalData);
}

CPiiTex::~CPiiTex() {}

void CPiiTex::InitTexture(u16 width, u16 height) {
    mTexture.reset(new COffScreenTexture);
    mTexture->Create(width, height, GX_TF_RGB5A3, GX_CLAMP, GX_LINEAR);
    BWorld::Desc desc;
    desc.SetDrawType(4);
    desc.SetObjectCapacity(100);
    desc.SetOutlineCapacity(100);
    desc.SetScnObjCapacity(100);
    mWorld.reset(new BWorld(desc));
    std::tr1::shared_ptr<BWorldCamera> camera(new BWorldCamera(2));
    mWorld->AddCamera(camera);
    mWorld->SetCurrentCamera(camera);
}

void CPiiTex::InitPiis(std::tr1::shared_ptr<CPiiPersonalData> personalData) {}

void CPiiTex::SetPiiVisible(u32 index, bool visible) {
    if (index < mPiis.size()) {
        mPiis[index]->SetVisible(visible);
        UpdateCamera();
    }
}

void CPiiTex::UpdateCamera() {
    const PiiSpeciesData* pSpecies;
    if (mPiis.size() == 0) {
        pSpecies = GetPiiSpeciesData(1, 0);
    } else {
        pSpecies = mPiis[0]->GetPersonalData()->GetSpeciesData();
        for (std::vector<std::tr1::shared_ptr<CPii> >::iterator it = mPiis.begin(), end = mPiis.end(); it != end; ++it) {
            if ((*it)->BWorldObj::IsVisible()) {
                pSpecies = (*it)->GetPersonalData()->GetSpeciesData();
                break;
            }
        }
    }
    PiiTexCameraParams params = gPiiTexCameraParams[pSpecies->GetSpecies()];
    if (params.mDistance == 0) {
        params.mDistance = 1000;
        params.mHeight = 500;
        params.mAngle = 20;
    }
    nw4r::g3d::Camera camera = mWorld->GetCurrentCamera()->GetG3dCamera();
    u16 height = mTexture->GetHeight();
    u16 width = mTexture->GetWidth();
    camera.SetViewport(0.0f, 0.0f, width, height);
    f32 sinValue;
    f32 cosValue;
    nw4r::math::SinCosFIdx(&sinValue, &cosValue, 0.7111111f * params.mAngle);
    f32 extent = params.mDistance / 1000.0f * 0.5f;
    nw4r::math::VEC3 position(50.5f * sinValue, params.mHeight / 1000.0f, 50.5f * cosValue);
    camera.SetOrtho(extent, -extent, -extent, extent, 1.0f, 100.0f);
    camera.SetPosition(position);
    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
    posture.cameraTarget = nw4r::math::VEC3(0.0f, position.y, 0.0f);
    posture.cameraTwist = 0.0f;
    camera.SetPosture(posture);
}

nw4r::lyt::TexMap CPiiTex::GetTexMap() const {
    nw4r::lyt::TexMap texMap;
    texMap.SetImage(mTexture->GetBuffer());
    texMap.SetSize(mTexture->GetWidth(), mTexture->GetHeight());
    texMap.SetTexelFormat(mTexture->GetFormat());
    return texMap;
}

void CPiiTex::Calc() {
    mWorld->Calc();
}

void CPiiTex::CalcMtx() {
    mWorld->CalcMtx();
}

void CPiiTex::CalcView() {
    mWorld->CalcView();
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    Graphics::ResetState();
    Graphics::SetPixelFormat(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    mWorld->Draw();
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    mTexture->CopyFromEFB(true);
}
