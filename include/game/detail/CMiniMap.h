#ifndef GAME_DETAIL_CMINIMAP_H
#define GAME_DETAIL_CMINIMAP_H

#include <game/BWorld.h>
#include <game/CDisplayPhaseCtrl.h>
#include <game/CFileData.h>
#include <game/CLayout.h>
#include <game/COffScreenTexture.h>
#include <game/IMiniMap.h>
#include <memory>

class CMiniMap : public IMiniMap {
public:
    CMiniMap() : mDisplay(&mDisplayLayout) {}
    virtual void Calc();
    virtual void CalcView();
    virtual void Draw();
    virtual void Show();
    virtual void Hide();

    void Initialize(MapDesc* pMapDesc, BWorld* pWorld, f32 height, bool background);

private:
    void CalcMarkers(BWorldCamera* pCamera, std::vector<BWorldObj*>& objects, std::vector<CMiniMapMarker*>& markers);

    CDisplayPhaseCtrl mDisplay;
    std::tr1::shared_ptr<CFileData> mMapFile;
    CLayout mMapLayout;
    nw4r::math::VEC2 mMapSize;
    COffScreenTexture mCapture;
    CLayout mDisplayLayout;
    std::tr1::weak_ptr<BWorld> mWorld;
    f32 mHeight;
};

#endif
