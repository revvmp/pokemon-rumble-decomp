#ifndef GAME_CUIPIISPEC_H
#define GAME_CUIPIISPEC_H

#include <game/BUILayout.h>
#include <game/CPiiPersonalData.h>
#include <memory>

class CUIPiiSpec : public BUILayout {
public:
    virtual void SetPii(std::tr1::shared_ptr<CPiiPersonalData> pii, int player);
};

#endif
