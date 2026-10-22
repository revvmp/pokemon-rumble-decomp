#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_texMap.h>
#include <nw4r/lyt/lyt_pane.h>
#include <game/detail/CMiniMap.h>

#include <game/BWorldCamera.h>
#include <game/Font.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/StageID.h>
#include <game/StringFormat.h>
#include <nw4r/lyt/lyt_texMap.h>
#include <string>
#include <game/CMiniMapMarker.h>
#include <game/Graphics.h>
#include <revolution/gx/GXTexture.h>
#include <nw4r/math/math_geometry.h>
#include <algorithm>
#include <vector>

namespace {

bool CompareMiniMapMarkers(const CMiniMapMarker* pLeft, const CMiniMapMarker* pRight) {
    if (pLeft == pRight) {
        return false;
    }
    if (pLeft->GetType() != pRight->GetType()) {
        return pLeft->GetType() < pRight->GetType();
    }
    return pLeft < pRight;
}

}

inline void CMiniMap::Initialize(MapDesc* pMapDesc, BWorld* pWorld, f32 height, bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mMapLayout.Load(gSharedLayoutArchives, "minimap_map", fonts, background);
    mDisplayLayout.Load(gSharedLayoutArchives, "minimap", fonts, background);
    {
        int stageNo = pMapDesc->mParams.GetValue(0).ToInt();
        std::string world = gStageWorlds[pMapDesc->mIndex.mStage].mName;
        std::string rank = gStageRanks[pMapDesc->mIndex.mArea].mName;
        std::string number = FormatString("%02d", stageNo);
        std::string archive = "5:/stage/" + world + "/" + number + ".arc";
        std::tr1::shared_ptr<CFileData> file = LoadResourceFile((archive + "/minimap_" + rank + number + ".tpl").c_str(), 0, background);
        if (!(file && file->IsLoaded())) {
            file = LoadFile((archive + "/minimap_" + number + ".tpl").c_str(), 0, background);
        }
        mMapFile = file;
    }

    nw4r::lyt::TexMap captureMap;
    nw4r::ut::Rect rect = mMapLayout.GetLayout()->GetLayoutRect();
    u16 width = rect.right - rect.left;
    u16 mapHeight = rect.top - rect.bottom;
    mCapture.Create(width, mapHeight, GX_TF_RGB5A3, GX_CLAMP, GX_LINEAR);
    captureMap.SetImage(mCapture.GetBuffer());
    captureMap.SetSize(width, mapHeight);
    captureMap.SetTexelFormat(mCapture.GetFormat());
    mMapSize.x = width / 10.0f;
    mMapSize.y = mapHeight / 10.0f;
    mDisplayLayout.FindPane("map")->GetMaterial()->GetTexMapAry()[0] = captureMap;

    nw4r::lyt::TexMap fileMap;
    fileMap.Set(static_cast<TPLPalette*>(mMapFile->GetData()), 0);
    mMapLayout.FindPane("map")->GetMaterial()->GetTexMapAry()[0] = fileMap;
    mWorld = pWorld->shared_from_this();
    mHeight = height;
}

std::tr1::shared_ptr<IMiniMap> IMiniMap::Create(MapDesc* pMapDesc, BWorld* pWorld, f32 height, bool background) {
    std::tr1::shared_ptr<CMiniMap> pMiniMap(new CMiniMap);
    pMiniMap->Initialize(pMapDesc, pWorld, height, background);
    return pMiniMap;
}

CMiniMap::~CMiniMap() {}

void CMiniMap::Show() {
    mDisplay.Show();
}

void CMiniMap::Hide() {
    mDisplay.Hide();
}

void CMiniMap::Calc() {
    mDisplay.Calc();
    mDisplayLayout.Calc();
    mMapLayout.Calc();
}

void CMiniMap::Draw() {
    if (mDisplay.IsVisible()) {
        mDisplayLayout.Draw();
    }
}

void CMiniMap::CalcView() {
    std::vector<CMiniMapMarker*> markers;
    {
        std::tr1::shared_ptr<BWorld> world = mWorld.lock();
        if (world) {
            BWorldCamera* pCamera = world->GetCurrentCamera();
            if (pCamera) {
                {
                    std::vector<BWorldObj*> objects = world->GetObjects(BWorldObj::TYPE_WORLD_OBJ);
                    nw4r::g3d::Camera camera = pCamera->GetG3dCamera();
                    nw4r::math::MTX34 inverse;
                    nw4r::math::MTX34 view;
                    camera.GetCameraMtx(&view);
                    if (PSMTXInverse(view, inverse)) {
                        nw4r::math::VEC3 intersection;
                        nw4r::math::PLANE plane;
                        nw4r::math::VEC3 position;
                        nw4r::math::VEC3 direction(0.0f, 0.0f, -1.0f);
                        PSMTXMultVec(inverse, direction, direction);
                        camera.GetPosition(&position);
                        nw4r::math::LINE3 line(position, direction - position);
                        nw4r::math::VEC3Normalize(&line.d, &line.d);
                        plane.n = nw4r::math::VEC3(0.0f, 1.0f, 0.0f);
                        plane.d = -mHeight;
                        if (nw4r::math::IntersectionLine3Plane(&line, &plane, nullptr, &intersection) == nw4r::math::INTERSECTION_1) {
                            u16 width = mCapture.GetWidth();
                            u16 height = mCapture.GetHeight();
                            nw4r::lyt::Pane* pMap = mMapLayout.FindPane("map");
                            nw4r::lyt::Size mapSize;
                            mapSize.width = width * (mMapSize.x / 15.0f);
                            mapSize.height = height * (mMapSize.y / 15.0f);
                            pMap->SetSize(mapSize);
                            pMap->SetTranslate(nw4r::math::VEC3((width / 2) * (-intersection.x / 15.0f), (height / 2) * (intersection.z / 15.0f), 0.0f));
                            for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
                                BWorldObj* pObject = *it;
                                if (pObject->IsVisible()) {
                                    CMiniMapMarker* pMarker = pObject->GetEffect();
                                    if (pMarker) {
                                        nw4r::math::VEC3 position = pObject->GetPositionRef();
                                        pMarker->GetPane()->SetTranslate(nw4r::math::VEC3((width / 2) * ((position.x - intersection.x) / 15.0f), (height / 2) * (-(position.z - intersection.z) / 15.0f), 0.0f));
                                        markers.push_back(pMarker);
                                    }
                                }
                            }
                        }
                    }
                }
                std::sort(markers.begin(), markers.end(), CompareMiniMapMarkers);
            }
        }
    }
    u16 width = mCapture.GetWidth();
    u16 height = mCapture.GetHeight();
    nw4r::math::VEC2 size(width, height);
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    Graphics::ResetState();
    Graphics::SetPixelFormat(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    mMapLayout.CalculateMtx(true);
    mMapLayout.Render(size);
    for (std::vector<CMiniMapMarker*>::iterator it = markers.begin(); it != markers.end(); ++it) {
        (*it)->GetLayout().CalculateMtx(true);
        (*it)->GetLayout().Render(size);
    }
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);
    mCapture.CopyFromEFB(true);
}
