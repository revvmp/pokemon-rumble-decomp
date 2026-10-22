#ifndef GAME_CHPBAR_H
#define GAME_CHPBAR_H

#include <game/CLayout.h>
#include <game/IWorldObjStat.h>
#include <list>
#include <memory>

class CPiiPersonalData;

class CHPBar : public IWorldObjStat {
public:
    class CDamage;
    enum Kind { KIND_PLAYER, KIND_ENEMY, KIND_BOSS, KIND_UNINITIALIZED };

    explicit CHPBar(u32 type);
    virtual ~CHPBar();
    virtual void Update(BWorldObj* pObj);
    virtual void CalcView(const nw4r::math::MTX34& rView,
                          const nw4r::math::MTX44& rProjection);
    virtual bool IsVisible() const;
    virtual void Draw();
    virtual int GetType() const;

    void Initialize(int playerNo, CPiiPersonalData* pPersonalData, bool background);

private:
    u32 mFlags;
    u32 mType;
    Kind mKind;
    int mPlayerNo;
    int mAnimationState;
    f32 mHPRate;
    f32 mDelayedHPRate;
    int mDelayFrames;
    nw4r::math::VEC3 mPosition;
    CLayout mLayout;
    nw4r::lyt::Pane* mpBar;
    f32 mBarWidth;
    nw4r::lyt::Pane* mpYellow;
    nw4r::lyt::Pane* mpGreen;
    nw4r::lyt::Pane* mpLevel;
    std::list<std::tr1::shared_ptr<CDamage> > mDamage;
    bool mHasDamageSound;
};

#endif
