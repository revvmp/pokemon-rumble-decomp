#ifndef GAME_CPIITEX_H
#define GAME_CPIITEX_H

#include <memory>
#include <vector>
#include <revolution/types.h>
#include <nw4r/lyt/lyt_texMap.h>
#include <game/CaptureTexture.h>
class CPii;
class CPiiPersonalData;
class BWorld;
struct PiiTexCameraParams {
    int mDistance;
    int mHeight;
    int mAngle;
};

extern const PiiTexCameraParams gPiiTexCameraParams[];

class COffScreenTexture : public CaptureTexture {
public:
    COffScreenTexture() {}
    ~COffScreenTexture() {}
};

class CPiiTex {
public:
    CPiiTex(u16 width, u16 height);
    CPiiTex(std::tr1::shared_ptr<CPiiPersonalData> personalData, u16 width, u16 height);
    ~CPiiTex();
    void Calc();
    void CalcMtx();
    void CalcView();
    nw4r::lyt::TexMap GetTexMap() const;
private:
    void InitTexture(u16 width, u16 height);
    void InitPiis(std::tr1::shared_ptr<CPiiPersonalData> personalData);
    void UpdateCamera();
    void SetPiiVisible(u32 index, bool visible);

    std::vector<std::tr1::shared_ptr<CPii> > mPiis;
    std::tr1::shared_ptr<COffScreenTexture> mTexture;
    std::tr1::shared_ptr<BWorld> mWorld;
};

#endif
