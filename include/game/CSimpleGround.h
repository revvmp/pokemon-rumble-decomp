#ifndef GAME_CSIMPLEGROUND_H
#define GAME_CSIMPLEGROUND_H

#include <game/BModelObj.h>
#include <memory>

class CPpuEnv;

namespace NPpu {
class CEnvironment;
}

class CPpuEngine;

class CSimpleGround : public BModelObj {
public:
    enum { TYPE = 1 << 14 };

    explicit CSimpleGround(u32 type);
    virtual ~CSimpleGround();
    virtual void Calc();
    virtual CPpuEngine* GetPpuEngine() const { return mPpuEngine.get(); }
    virtual void OnLeaveWorld(BWorld* pWorld);
    virtual void Initialize(bool background);

    void LoadScript(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment);
    void LoadModel(const char* pName, bool background);

private:
    u32 mReserved;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
};

#endif
