#ifndef GAME_DETAIL_CCOLORCONST_H
#define GAME_DETAIL_CCOLORCONST_H

#include <game/IColorConst.h>
#include <game/LayoutResources.h>
#include <nw4r/ut/ut_Color.h>

namespace {
class CColorConst : public IColorConst {
public:
    enum { PaletteCount = 29, PaletteColors = 4 };

    virtual nw4r::ut::Color GetTextColor(int index, int end) const;
    virtual nw4r::ut::Color GetVertexColor(int index, int end) const;
    virtual bool ApplyTextColor(nw4r::lyt::Pane* pPane, int index, int mode) const;
    virtual bool ApplyTextColor(CLayout* pLayout, const char* pName, int index, int mode) const;
    virtual bool ApplyVertexColor(nw4r::lyt::Pane* pPane, int index, int mode) const;
    virtual bool ApplyVertexColor(CLayout* pLayout, const char* pName, int index, int mode) const;

    void LoadPalettes(CLayout& rLayout);

private:
    nw4r::ut::Color mColors[PaletteCount][PaletteColors];
};
}

#endif
