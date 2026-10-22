#ifndef GAME_CUIPIISPEC_H
#define GAME_CUIPIISPEC_H

#include <game/BUILayout.h>
#include <game/CPiiPersonalData.h>
#include <memory>

class CPiiTex;
class CUIStar;
class CUIType;

class CUIPiiSpec : public BUILayout {
public:
    CUIPiiSpec();
    virtual ~CUIPiiSpec();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    virtual void SetPii(std::tr1::shared_ptr<CPiiPersonalData> pii, int player);

private:
    std::tr1::shared_ptr<CUIType> mSpeciesTypes[2];
    std::tr1::shared_ptr<CUIType> mWazaTypes[2];
    std::tr1::shared_ptr<CUIStar> mWazaStars[2];
    std::tr1::shared_ptr<CUIStar> mApStar;
    std::tr1::shared_ptr<CUIStar> mDpStar;
    std::tr1::shared_ptr<CPiiPersonalData> mPii;
    std::tr1::shared_ptr<CPiiTex> mPiiTex;
    int mPiiTexDelay;
};

#endif
