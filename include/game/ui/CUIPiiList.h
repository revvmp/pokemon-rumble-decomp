#ifndef GAME_CUIPIILIST_H
#define GAME_CUIPIILIST_H

#include <game/ui/CUIList.h>

class CUIPiiList : public CUIList {
public:
    void SelectIndex(int index) { SetSelectedIndex(index); }
};

#endif
