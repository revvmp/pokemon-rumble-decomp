#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_texMap.h>
#include <nw4r/lyt/lyt_pane.h>
#include <game/detail/CMiniMap.h>

#include <game/BWorldCamera.h>
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
                    nw4r::math::MTX34 view;
                    nw4r::math::MTX34 inverse;
                    camera.GetCameraMtx(&view);
                    if (PSMTXInverse(view, inverse)) {
                        nw4r::math::VEC3 direction(0.0f, 0.0f, -1.0f);
                        PSMTXMultVec(inverse, direction, direction);
                        nw4r::math::VEC3 position;
                        camera.GetPosition(&position);
                        direction -= position;
                        nw4r::math::LINE3 line(position, direction);
                        nw4r::math::VEC3Normalize(&line.d, &line.d);
                        nw4r::math::PLANE plane(0.0f, 1.0f, 0.0f, -mHeight, true);
                        nw4r::math::VEC3 intersection;
                        if (nw4r::math::IntersectionLine3Plane(&line, &plane, nullptr, &intersection) == nw4r::math::INTERSECTION_1) {
                            u16 width = mCapture.GetWidth();
                            u16 height = mCapture.GetHeight();
                            nw4r::lyt::Pane* pMap = mMapLayout.FindPane("map");
                            pMap->SetSize(nw4r::lyt::Size(width * (mMapSize.x / 15.0f), height * (mMapSize.y / 15.0f)));
                            pMap->SetTranslate(nw4r::math::VEC3((width / 2) * (-intersection.x / 15.0f), (height / 2) * (intersection.z / 15.0f), 0.0f));
                            for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
                                BWorldObj* pObject = *it;
                                if (pObject->IsVisible()) {
                                    CMiniMapMarker* pMarker = pObject->GetEffect();
                                    if (pMarker) {
                                        nw4r::math::VEC3 delta = pObject->GetPositionRef() - intersection;
                                        pMarker->GetPane()->SetTranslate(nw4r::math::VEC3((width / 2) * (delta.x / 15.0f), (height / 2) * (-delta.z / 15.0f), 0.0f));
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
