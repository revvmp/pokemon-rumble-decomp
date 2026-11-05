#ifndef GAME_CPIIINFOWORLDOBJSTAT_H
#define GAME_CPIIINFOWORLDOBJSTAT_H

#include <game/ui/CLayout.h>
#include <game/ui/IWorldObjStat.h>
#include <memory>

class CPiiPersonalData;

class CPiiInfoWorldObjStat : public IWorldObjStat {
public:
    CPiiInfoWorldObjStat();
    virtual ~CPiiInfoWorldObjStat();
    virtual void Update(BWorldObj* pObj);
    virtual void CalcView(const nw4r::math::MTX34& rView, const nw4r::math::MTX44& rProjection);
    virtual bool IsVisible() const;
    virtual void Draw();
    virtual int GetType() const;

    void Load(std::tr1::shared_ptr<CPiiPersonalData> data, bool background);
    void SetPersonalData(std::tr1::shared_ptr<CPiiPersonalData> data) { Load(data, false); }

private:
    enum Flag { FLAG_HAS_POSITION = 1 << 0, FLAG_SHOWN = 1 << 2 };
    enum { TYPE_PII_INFO = 107 };

    u32 mFlags;
    nw4r::math::VEC3 mPosition;
    CLayout mLayout;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
};

#endif
