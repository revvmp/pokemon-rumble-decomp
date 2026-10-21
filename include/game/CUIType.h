#ifndef GAME_CUITYPE_H
#define GAME_CUITYPE_H

#include <game/BUILayout.h>

class CUIType : public BUILayout {
public:
    CUIType();
    virtual ~CUIType();
    void Initialize(bool background);
    void SetType(int type);
};

#endif
