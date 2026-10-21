#include <game/CShadowCamera.h>
#include <game/BWorld.h>
#include <game/CModel.h>
#include <game/MathUtil.h>
#include <game/Memory.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/math/math_geometry.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSError.h>

CShadowCamera::CShadowCamera(u32 type) :
    BWorldCamera(type | TYPE),
    mUseTextureAlpha(true),
    mAlpha(255),
    mAutoSelectLight(true),
    mLightSetIndex(0),
    mLightIndex(0),
    mPlaneHeight(0.0f) {}

CShadowCamera::~CShadowCamera() {}

void CShadowCamera::Init(u16 width, u16 height, int levels, bool useTextureAlpha) {
    mTextureLevels = levels;
    mUseTextureAlpha = useTextureAlpha;

    u32 size = GXGetTexBufferSize(width, height, GX_TF_I8, GX_FALSE, 0);
    std::tr1::shared_ptr<u8> buffer(static_cast<u8*>(GameMemory::Allocate(size)), GameMemory::Free);
    DCFlushRange(buffer.get(), size);

    for (int i = 0; i < mTextureLevels; i++) {
        u16 levelWidth = width >> i;
        u16 levelHeight = height >> i;
        mTextures[i].Init(std::tr1::shared_ptr<u8>(buffer), size, levelWidth, levelHeight, GX_TF_I8, GX_CLAMP, GX_LINEAR);
    }
}

void CShadowCamera::UpdateVisible() {
    SetVisible(CalcLightProjection());
}

bool CShadowCamera::CalcLightProjection() {
    BWorldCamera* pCamera = GetWorld()->GetCurrentCamera();
    if (pCamera == nullptr) {
        return false;
    }

    nw4r::g3d::LightSet lightSet = GetWorld()->GetScnRoot()->GetLightSet(mLightSetIndex);
    if (!lightSet.IsValid()) {
        return false;
    }

    nw4r::g3d::LightObj* pLight = lightSet.GetLightObj(mLightIndex);
    if (pLight == nullptr) {
        return false;
    }

    nw4r::math::VEC3 lightPos(-1.0f, 3.0f, -1.0f);
    pLight->GetLightPos(&lightPos);

    nw4r::math::VEC3 lightDir;
    if (!SafeNormalize(&lightDir, &lightPos)) {
        return false;
    }

    nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
    f32 nearClip = camera.ref().projNear;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    switch (camera.ref().flags & (nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM | nw4r::g3d::CameraData::FLAG_PROJ_PERSP |
                                  nw4r::g3d::CameraData::FLAG_PROJ_ORTHO)) {
    case nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM:
        top = camera.ref().projTop;
        bottom = camera.ref().projBottom;
        left = camera.ref().projLeft;
        right = camera.ref().projRight;
        break;
    case nw4r::g3d::CameraData::FLAG_PROJ_PERSP:
        top = nearClip * nw4r::math::TanDeg(0.5f * camera.ref().projFovy);
        right = top * camera.ref().projAspect;
        bottom = -top;
        left = -right;
        break;
    default:
        return false;
    }

    nw4r::math::VEC3 corners[4];
    nw4r::math::MTX34 cameraMtx;
    pCamera->GetG3dCamera().GetCameraMtx(&cameraMtx);

    nw4r::math::MTX34 invCameraMtx;
    if (!Inverse(&invCameraMtx, &cameraMtx)) {
        return false;
    }

    corners[0] = nw4r::math::VEC3(left, top, -nearClip);
    corners[1] = nw4r::math::VEC3(right, top, -nearClip);
    corners[2] = nw4r::math::VEC3(right, bottom, -nearClip);
    corners[3] = nw4r::math::VEC3(left, bottom, -nearClip);
    PSMTXMultVecArray(invCameraMtx, reinterpret_cast<Vec*>(corners), reinterpret_cast<Vec*>(corners), 4);

    nw4r::math::VEC3 eye;
    pCamera->GetG3dCamera().GetPosition(&eye);

    nw4r::math::PLANE ground;
    ground.n = nw4r::math::VEC3(0.0f, 1.0f, 0.0f);
    ground.d = -mPlaneHeight;
    f32 distances[4];
    for (int i = 0; i < 4; i++) {
        nw4r::math::LINE3 ray(eye, corners[i] - eye);
        nw4r::math::VEC3Normalize(&ray.d, &ray.d);
        if (nw4r::math::IntersectionLine3Plane(&ray, &ground, &distances[i], &corners[i]) != nw4r::math::INTERSECTION_1) {
            distances[i] = -1.0f;
        }
    }

    if (distances[0] < 0.0f && distances[1] < 0.0f) {
        if (0.0f < distances[2] && 0.0f < distances[3]) {
            nw4r::math::VEC3 origin(eye.x, 0.0f, eye.y);
            corners[3].y = 0.0f;
            corners[2].y = 0.0f;

            nw4r::math::LINE3 leftRay(origin, corners[3] - origin);
            nw4r::math::VEC3Normalize(&leftRay.d, &leftRay.d);

            nw4r::math::LINE3 rightRay(origin, corners[2] - origin);
            nw4r::math::VEC3Normalize(&rightRay.d, &rightRay.d);

            corners[0] = corners[3] + leftRay.d * 5.0f;
            corners[1] = corners[2] + rightRay.d * 5.0f;
        } else {
            return false;
        }
    }

    nw4r::g3d::Camera shadowCamera = GetG3dCamera();
    f32 viewNear = pCamera->GetG3dCamera().ref().projNear;
    f32 viewFar = pCamera->GetG3dCamera().ref().projFar;
    shadowCamera.SetOrtho(1.0f, -1.0f, -1.0f, 1.0f, viewNear, viewFar);
    shadowCamera.SetViewport(0.0f, 0.0f, mTextures[0].GetWidth(), mTextures[0].GetHeight());

    nw4r::math::VEC3 center = (corners[0] + corners[1] + corners[2] + corners[3]) / 4.0f;
    nw4r::math::VEC3 shadowPos = center + lightDir * ((viewFar - viewNear) * 0.5f);
    shadowCamera.SetPosition(shadowPos);

    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
    posture.cameraTarget = center;
    posture.cameraTwist = 0.0f;
    shadowCamera.SetPosture(posture);

    nw4r::math::VEC3 viewCorners[4];
    nw4r::math::MTX34 shadowMtx;
    shadowCamera.GetCameraMtx(&shadowMtx);
    PSMTXMultVecArray(shadowMtx, reinterpret_cast<Vec*>(corners), reinterpret_cast<Vec*>(viewCorners), 4);

    f32 longestSq = 0.0f;
    nw4r::math::VEC3 longestEdge;
    for (int i = 0; i < 4; i++) {
        nw4r::math::VEC3 edge = viewCorners[i] - viewCorners[(i + 1) % 4];
        f32 lenSq = edge.LenSq();
        if (longestSq <= lenSq) {
            longestSq = lenSq;
            longestEdge = edge;
        }
    }

    posture.cameraTwist = nw4r::math::Atan2Deg(longestEdge.y, longestEdge.x);
    shadowCamera.SetPosture(posture);

    shadowCamera.GetCameraMtx(&shadowMtx);
    PSMTXMultVecArray(shadowMtx, reinterpret_cast<Vec*>(corners), reinterpret_cast<Vec*>(viewCorners), 4);

    f32 maxY = viewCorners[0].y;
    f32 minY = viewCorners[0].y;
    f32 maxX = viewCorners[0].x;
    f32 minX = viewCorners[0].x;
    for (int i = 1; i < 4; i++) {
        maxY = Max(maxY, viewCorners[i].y);
        minY = Min(minY, viewCorners[i].y);
        maxX = Max(maxX, viewCorners[i].x);
        minX = Min(minX, viewCorners[i].x);
    }

    shadowCamera.SetOrtho(maxY, minY, minX, maxX, viewNear, viewFar);
    return true;
}

static void SetupShadowMaterial(nw4r::g3d::ResMat mat, nw4r::g3d::ScnMdl::CopiedMatAccess& rAccess,
                                const CaptureTexture& rTexture, GXColor color, int cameraIndex, bool useTextureAlpha) {
    u8 addedStages = useTextureAlpha ? 2 : 1;

    nw4r::g3d::ResGenMode genMode = rAccess.GetResGenMode(false);
    nw4r::g3d::ResMatTevColor tevColor = rAccess.GetResMatTevColor(false);
    nw4r::g3d::ResMatTexCoordGen texCoordGen = rAccess.GetResMatTexCoordGen(false);
    nw4r::g3d::ResTexObj texObj = rAccess.GetResTexObj(false);
    nw4r::g3d::ResTexSrt texSrt = rAccess.GetResTexSrt(false);
    nw4r::g3d::ResTev tev = rAccess.GetResTev(false);

    u32 texMap = mat.GetNumResTexPlttInfo();
    u32 texCoord = genMode.GXGetNumTexGens();
    u32 stage = genMode.GXGetNumTevStages();

    if (texMap > 7) {
        OSReport("mat '%s' has more than %d textures.(%d)\n", mat.GetName(), 7, texMap);
        return;
    }
    if (texCoord > 7) {
        OSReport("mat '%s' has more than %d tex coord gens.(%d)\n", mat.GetName(), 7, texCoord);
        return;
    }
    if (stage > 16 - addedStages) {
        OSReport("mat '%s' has more than %d tev stages.(%d)\n", mat.GetName(), 16 - addedStages, stage);
        return;
    }

    GXTevStageID second = static_cast<GXTevStageID>(stage + 1);
    GXPTTexMtx postMtx = static_cast<GXPTTexMtx>(GX_PTTEXMTX0 + texCoord * 3);
    GXTexMapID map = static_cast<GXTexMapID>(texMap);
    *texObj.GetTexObj(map) = rTexture.GetTexObj();
    texObj.Validate(map);

    u32 numStages = stage + addedStages;
    genMode.GXSetNumTevStages(numStages);
    genMode.GXSetNumTexGens(texCoord + 1);

    tevColor.GXSetTevKColor(GX_KCOLOR3, color);
    tevColor.DCStore(false);

    texCoordGen.GXSetTexCoordGen2(static_cast<GXTexCoordID>(texCoord), GX_TG_MTX3x4, GX_TG_POS, GX_FALSE, postMtx);
    texCoordGen.DCStore(false);

    texSrt.SetEffectMtx(texCoord, nullptr);
    texSrt.SetMapMode(texCoord, 2, cameraIndex, -1);
    texSrt.ref().flag = (texSrt.ref().flag & ~(0xF << (texCoord * 4))) | (0xF << (texCoord * 4));

    if (tev.ref().dummy[7] != 0) {
        return;
    }

    tev.SetNumTevStages(numStages);

    GXTevStageID first = static_cast<GXTevStageID>(stage);
    GXTexCoordID coord = static_cast<GXTexCoordID>(texCoord);
    if (useTextureAlpha) {
        tev.GXSetTevKColorSel(first, GX_TEV_KCSEL_K3_A);
        tev.GXSetTevKColorSel(second, GX_TEV_KCSEL_K3);
        tev.GXSetTevOrder(first, coord, map, GX_COLOR_NULL);
        tev.GXSetTevColorIn(first, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXA, GX_CC_ZERO);
        tev.GXSetTevColorOp(first, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
        tev.GXSetTevAlphaIn(first, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        tev.GXSetTevAlphaOp(first, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        tev.GXSetTevOrder(second, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        tev.GXSetTevColorIn(second, GX_CC_CPREV, GX_CC_KONST, GX_CC_C0, GX_CC_ZERO);
        tev.GXSetTevColorOp(second, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        tev.GXSetTevAlphaIn(second, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        tev.GXSetTevAlphaOp(second, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    } else {
        tev.GXSetTevKColorSel(first, GX_TEV_KCSEL_K3);
        tev.GXSetTevOrder(first, coord, map, GX_COLOR_NULL);
        tev.GXSetTevColorIn(first, GX_CC_CPREV, GX_CC_KONST, GX_CC_TEXA, GX_CC_ZERO);
        tev.GXSetTevColorOp(first, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        tev.GXSetTevAlphaIn(first, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        tev.GXSetTevAlphaOp(first, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    }

    tev.ref().dummy[6] = stage;
    tev.ref().dummy[7] = 195;
    tev.DCStore(false);
}

void CShadowCamera::SetupShadowModel(std::tr1::shared_ptr<CModel> model) {
    nw4r::g3d::ScnMdl* pScnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(model->GetScnObj());
    nw4r::g3d::ResMdl resMdl = pScnMdl->GetResMdl();
    u32 numMats = resMdl.GetResMatNumEntries();

    for (u32 i = 0; i < numMats; i++) {
        nw4r::g3d::ResMat mat = resMdl.GetResMat(i);
        nw4r::g3d::ResUserData userData = mat.GetResUserData();
        if (!userData.IsValid()) {
            continue;
        }

        nw4r::g3d::ResUserDataEntry entry = userData.GetEntry("SHADOW_RGBA");
        if (!entry.IsValid() || entry.GetType() != nw4r::g3d::ResUserDataEntry::TYPE_INT || entry.GetNumData() != 1) {
            continue;
        }

        GXColor color = entry.GetColor();
        if (!mUseTextureAlpha) {
            mAlpha = std::min(mAlpha, color.a);
        }

        nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, i);
        SetupShadowMaterial(mat, access, mTextures[mTextureLevels - 1], color, GetIndex(), mUseTextureAlpha);
    }
}
