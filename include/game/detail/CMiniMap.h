#ifndef GAME_DETAIL_CMINIMAP_H
#define GAME_DETAIL_CMINIMAP_H

#include <game/BWorld.h>
#include <game/CDisplayPhaseCtrl.h>
#include <game/CFileData.h>
#include <game/CLayout.h>
#include <game/CaptureTexture.h>
#include <game/IMiniMap.h>
#include <memory>

class CMiniMap : public IMiniMap {
public:
    CMiniMap() : mDisplay(&mDisplayLayout) {}
    virtual ~CMiniMap();
    virtual void Calc();
    virtual void CalcView();
    virtual void Draw();
    virtual void Show();
    virtual void Hide();

    bool Initialize(MapDesc* pMapDesc, BWorld* pWorld, f32 height, bool background);

private:
    CDisplayPhaseCtrl mDisplay;
    std::tr1::shared_ptr<CFileData> mMapFile;
    CLayout mMapLayout;
    nw4r::math::VEC2 mMapSize;
    CaptureTexture mCapture;
    CLayout mDisplayLayout;
    std::tr1::weak_ptr<BWorld> mWorld;
    f32 mHeight;
};

#endif
