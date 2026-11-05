#ifndef GAME_CHPBAR_H
#define GAME_CHPBAR_H

#include <game/ui/CLayout.h>
#include <game/ui/IWorldObjStat.h>
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
    void SetEnabled(bool enabled);

private:
    enum Flag {
        FLAG_POSITION_VALID = 1 << 0,
        FLAG_CALCULATED = 1 << 1,
        FLAG_VISIBLE = 1 << 2,
        FLAG_DISABLED = 1 << 3,
        FLAG_PLAYER_DOWN = 1 << 4,
        FLAG_BOSS_HIDDEN = 1 << 5,
        FLAG_ON_SCREEN = 1 << 6,
        FLAG_UPDATED = 1 << 7,
        FLAG_VIEW_STARTED = 1 << 8
    };

    enum AnimationState { ANIMATION_IDLE, ANIMATION_GAUGE_LOOP, ANIMATION_PLAYER_OUT, ANIMATION_PLAYER_LOOP };

    void SetAnimationState(int state);

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
