#ifndef GAME_CWAZA_H
#define GAME_CWAZA_H

#include <game/BWorldObj.h>
#include <game/BSoundSource.h>
#include <map>

class CPpuEnv;

class CPpuEngine;
namespace NPpu { class CEnvironment; }

class CWaza : public BWorldObj, public BSoundSource {
public:
    enum { TYPE = 0x10000 };
    int GetWazaID() const { return mWazaID; }
    explicit CWaza(u32 type);
    using BWorldObj::SetPosition;
    virtual ~CWaza();
    virtual void Calc();
    void Setup(int id, std::tr1::shared_ptr<BWorldObj> parent);
    bool GetAttachmentPosition(nw4r::math::VEC3& rPosition);
    bool LoadScript(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment);
    void ClearBlownEnemysCount();
    u32 GetBlownEnemysCount(u32 id);
    void IncBlownEnemysCount(u32 id);
    virtual CPpuEngine* GetPpuEngine() const;
    virtual void OnLeaveWorld(BWorld* pWorld);

private:
    int mWazaID;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
    std::map<u32, u32> mBlownEnemysCount;
};

#endif
