#ifndef GAME_CPIITEX_H
#define GAME_CPIITEX_H

#include <game/PiiTexCameraParams.h>
#include <memory>
#include <vector>
#include <revolution/types.h>
#include <nw4r/lyt/lyt_texMap.h>
#include <game/COffScreenTexture.h>
#include <game/CPii.h>
class CPii;
class CPiiPersonalData;
class BWorld;
class CPiiTex {
public:
    CPiiTex(u16 width, u16 height);
    CPiiTex(std::tr1::shared_ptr<CPiiPersonalData> personalData, u16 width, u16 height);
    ~CPiiTex();
    void Calc();
    void CalcMtx();
    void CalcView();
    void SetPiiVisible(u32 index, bool visible);
    int AddPii(std::tr1::shared_ptr<CPiiPersonalData> personalData);
    unsigned GetPiiCount() const { return mPiis.size(); }
    bool IsPiiVisible(unsigned index) const { return static_cast<BWorldObj*>(mPiis[index].get())->IsVisible(); }
    nw4r::lyt::TexMap GetTexMap() const;
private:
    void InitTexture(u16 width, u16 height);
    void UpdateCamera();

    std::vector<std::tr1::shared_ptr<CPii> > mPiis;
    mutable std::tr1::shared_ptr<COffScreenTexture> mTexture;
    std::tr1::shared_ptr<BWorld> mWorld;
};

#endif
