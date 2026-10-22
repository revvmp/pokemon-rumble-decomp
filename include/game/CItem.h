#ifndef GAME_CITEM_H
#define GAME_CITEM_H

#include <game/BModelObj.h>

namespace NPpu {
class CEnvironment;
}

class CItem : public BModelObj {
public:
    enum { TYPE = 1 << 11 };
    explicit CItem(u32 type);
    virtual ~CItem();
    virtual void Calc();
    virtual CPpuEngine* GetPpuEngine() const { return mPpuEngine.get(); }
    virtual void OnLeaveWorld(BWorld* pWorld);
    virtual void Initialize(bool background);
    bool LoadScript(const char* pName, int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
    void Initialize() { Initialize(false); }
    void LoadModel(const char* pName) { LoadModel(pName, false); }
    void LoadModel(const char* pName, bool background);
private:
    u32 mFlags;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
};

#endif
