#ifndef GAME_CUIPLAYERID_H
#define GAME_CUIPLAYERID_H

#include <game/ui/BUILayout.h>

class CUIPlayerID : public BUILayout {
public:
    CUIPlayerID();
    virtual ~CUIPlayerID();
    void SetDisplayMode(int mode) { mDisplayMode = mode; }
    void Setup(int player, int mode, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    void Update();

private:
    int mDisplayMode;
    int mPlayerNo;
};

#endif
