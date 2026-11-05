#ifndef GAME_CPLAYERPII_H
#define GAME_CPLAYERPII_H

#include <game/pokemon/CPii.h>
#include <deque>
#include <nw4r/math/math_types.h>

class CPpuEnv;

class CPlayer;

class CPlayerPii : public CPii {
public:
    enum { TYPE = 1 << 7 };

    CPlayerPii(CPlayer* pPlayer, std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type);
    virtual ~CPlayerPii();

    virtual void Calc();
    virtual void Initialize(bool background);
    virtual const char* GetShadowName() const;
    virtual bool IsVisible() const;

    void Initialize();
    CPlayer* GetPlayer() { return mpPlayer; }
    bool LoadScript(const std::tr1::shared_ptr<CPpuEnv>& rEnvironment);
    bool LoadScript(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment);

    std::tr1::shared_ptr<CPlayerPii> GetSharedPtr() {
        return std::tr1::static_pointer_cast<CPlayerPii>(shared_from_this());
    }

    const std::deque<nw4r::math::VEC3>& GetPositionHistory() { return mPositionHistory; }

private:
    void SetEnvironment(NPpu::CEnvironment*) {}

    CPlayer* mpPlayer;
    std::deque<nw4r::math::VEC3> mPositionHistory;
};

#endif
