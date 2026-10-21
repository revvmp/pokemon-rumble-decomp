#ifndef GAME_CUIPLAYERID_H
#define GAME_CUIPLAYERID_H

#include <game/BUILayout.h>

class CUIPlayerID : public BUILayout {
public:
    CUIPlayerID();
    void Update();
    void Setup(int player, int flag, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);

private:
    int mReserved134;
    int mReserved138;
};

#endif
