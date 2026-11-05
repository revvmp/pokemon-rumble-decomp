#ifndef GAME_CUITYPE_H
#define GAME_CUITYPE_H

#include <game/ui/BUILayout.h>

class CUIType : public BUILayout {
public:
    CUIType();
    virtual ~CUIType();
    void Initialize(int background);
    void SetType(int type);
};

#endif
