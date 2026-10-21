#ifndef GAME_CWAZA_H
#define GAME_CWAZA_H

#include <game/BWorldObj.h>
#include <game/BSoundSource.h>
#include <map>

class CPpuEngine;

class CWaza : public BWorldObj, public BSoundSource {
public:
    explicit CWaza(u32 type);
    virtual ~CWaza();
    virtual void Calc();
    virtual CPpuEngine* GetPpuEngine() const;
    virtual void OnLeaveWorld();

private:
    int mWazaID;
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
    std::map<u32, u32> mHitCounts;
};

#endif
