#ifndef GAME_ICOLORCONST_H
#define GAME_ICOLORCONST_H

#include <nw4r/ut/ut_Color.h>

class CLayout;

namespace nw4r {
namespace lyt {
class Pane;
}
}

class IColorConst {
public:
    virtual nw4r::ut::Color GetTextColor(int index, int end) const = 0;
    virtual nw4r::ut::Color GetVertexColor(int index, int end) const = 0;
    virtual bool ApplyTextColor(nw4r::lyt::Pane* pPane, int index, int mode) const = 0;
    virtual bool ApplyTextColor(CLayout* pLayout, const char* pName, int index, int mode) const = 0;
    virtual bool ApplyVertexColor(nw4r::lyt::Pane* pPane, int index, int mode) const = 0;
    virtual bool ApplyVertexColor(CLayout* pLayout, const char* pName, int index, int mode) const = 0;
};

void InitializeColorConstants(bool background);
void FinalizeColorConstants();
IColorConst* GetColorConstants();

#endif
