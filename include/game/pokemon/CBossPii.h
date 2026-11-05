#ifndef GAME_CBOSSPII_H
#define GAME_CBOSSPII_H

#include <game/pokemon/CEnemyPii.h>

class CBossPii : public CEnemyPii {
public:
    enum { TYPE = 1 << 9 };

    CBossPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type);
    virtual ~CBossPii();

    virtual void Initialize(bool background);
};

#endif
