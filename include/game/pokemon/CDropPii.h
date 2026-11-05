#ifndef GAME_CDROPPII_H
#define GAME_CDROPPII_H

#include <game/pokemon/CPii.h>

class CDropPii : public CPii {
public:
    enum { TYPE = 1 << 10 };

    CDropPii(std::tr1::shared_ptr<CPiiPersonalData> personalData, u32 type);
    virtual ~CDropPii();

    virtual void Initialize(bool background);
};

#endif
