#ifndef GAME_CENEMYINFO_H
#define GAME_CENEMYINFO_H

#include <game/ui/IEnemyInfo.h>
#include <game/ui/CLayout.h>
#include <memory>

class CPiiPersonalData;
class CPiiTex;

class CEnemyInfo : public IEnemyInfo {
public:
    CEnemyInfo() {}
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void Show(const std::tr1::shared_ptr<CPiiPersonalData>& rData, bool flash, int frames);
    virtual void Hide();
    static std::tr1::shared_ptr<IEnemyInfo> Create(bool background);
private:
    void Initialize(bool background);
    CLayout mLayout;
    nw4r::lyt::Pane* mPanes[6];
    int mFrames;
    bool mFlash;
    std::tr1::shared_ptr<CPiiTex> mPiiTex;
    bool mCollected;
};

#endif
