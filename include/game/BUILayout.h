#ifndef GAME_BUILAYOUT_H
#define GAME_BUILAYOUT_H

#include <game/BUI.h>
#include <game/CLayout.h>

class BUILayout : public BUI {
public:
    BUILayout();
    virtual ~BUILayout();
    virtual void Calc();
    virtual void Draw();
    virtual void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    virtual void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background);
    virtual void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts);
    virtual void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts);

    CLayout* GetLayout();

private:
    CLayout mLayout;
};

#endif
