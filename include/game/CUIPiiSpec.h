#ifndef GAME_CUIPIISPEC_H
#define GAME_CUIPIISPEC_H

#include <game/BUILayout.h>
#include <game/CPiiPersonalData.h>
#include <memory>

class CUIPiiSpec : public BUILayout {
public:
    CUIPiiSpec();
    virtual void SetPii(std::tr1::shared_ptr<CPiiPersonalData> pii, int player);

private:
    u8 mReserved134[0x54];
};

#endif
