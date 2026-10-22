#ifndef GAME_PANERECTANGLE_H
#define GAME_PANERECTANGLE_H
#include <revolution/types.h>
namespace nw4r { namespace lyt { class Pane; } }
namespace NErrMsg {
struct PaneRectangle {
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;
    f32 GetWidth() const { return right - left; }
    f32 GetHeight() const { return top - bottom; }
};
PaneRectangle GetPaneRectangle(const nw4r::lyt::Pane* pPane);
void SetPaneRectangle(nw4r::lyt::Pane* pPane, const PaneRectangle& rRectangle);
PaneRectangle UnionRectangles(const PaneRectangle& rLeft, const PaneRectangle& rRight);
}
#endif
