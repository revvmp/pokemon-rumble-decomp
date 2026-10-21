#ifndef GAME_CPIITEX_H
#define GAME_CPIITEX_H

#include <memory>
#include <vector>
#include <revolution/types.h>
#include <nw4r/lyt/lyt_texMap.h>
class CPii;
class CPiiPersonalData;
class COffScreenTexture;
class BWorld;
class CPiiTex {
public:
    CPiiTex(std::tr1::shared_ptr<CPiiPersonalData> personalData, u16 width, u16 height);
    ~CPiiTex();
    void Calc();
    void CalcMtx();
    void CalcView();
    nw4r::lyt::TexMap GetTexMap() const;
private:
    std::vector<std::tr1::shared_ptr<CPii> > mPiis;
    std::tr1::shared_ptr<COffScreenTexture> mTexture;
    std::tr1::shared_ptr<BWorld> mWorld;
};

#endif
