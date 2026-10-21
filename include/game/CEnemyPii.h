#ifndef GAME_CENEMYPII_H
#define GAME_CENEMYPII_H

#include <game/CPii.h>

class CEnemyPii : public CPii {
public:
    enum { TYPE = 1 << 8 };

    CEnemyPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type);
    virtual ~CEnemyPii();

    virtual void Initialize(bool background);
};

#endif
