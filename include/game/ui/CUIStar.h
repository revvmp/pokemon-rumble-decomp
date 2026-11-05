#ifndef GAME_CUISTAR_H
#define GAME_CUISTAR_H

#include <game/ui/BUILayout.h>

class CUIStar : public BUILayout {
public:
    CUIStar();
    virtual ~CUIStar();
    void SetStars(int count, bool temporary);
};

#endif
