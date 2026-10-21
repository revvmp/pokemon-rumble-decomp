#ifndef GAME_CTEXTWORLDOBJSTAT_H
#define GAME_CTEXTWORLDOBJSTAT_H

#include <game/CLayout.h>
#include <game/IWorldObjStat.h>
#include <memory>

class CTextWorldObjStat : public IWorldObjStat {
public:
    enum Kind { KIND_MONEY, KIND_SPSTAT, KIND_CHIT, KIND_TIME, KIND_COUNT };
    enum { TYPE_BASE = 6 };

    CTextWorldObjStat();
    virtual ~CTextWorldObjStat();
    virtual void Update(BWorldObj* pObj);
    virtual void CalcView(const nw4r::math::MTX34& rView, const nw4r::math::MTX44& rProjection);
    virtual bool IsVisible() const;
    virtual void Draw();
    virtual u32 GetType() const;

    void Load(int kind, const wchar_t* pText, bool background);

private:
    enum Flag { FLAG_HAS_POSITION = 1 << 0, FLAG_SHOWN = 1 << 2 };

    u32 mFlags;
    u32 mType;
    nw4r::math::VEC3 mPosition;
    CLayout mLayout;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
};

#endif
