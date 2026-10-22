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
    virtual void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background);
    virtual void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, int background);
    virtual void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts) {
        Load(pArchive, pLayoutName, rFonts, false);
    }
    virtual void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts) {
        Load(pArchives, pLayoutName, rFonts, false);
    }

    CLayout* GetLayout() { return &mLayout; }
    bool AttachChild(const char* pPaneName, std::tr1::shared_ptr<BUILayout> child);
    nw4r::lyt::TextBox* SetText(const char* pName, const wchar_t* pText) { return mLayout.SetText(pName, pText); }

private:
    CLayout mLayout;
};

#endif
