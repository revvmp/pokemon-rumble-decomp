#include <game/PaneRectangle.h>

#include <nw4r/lyt/lyt_pane.h>
#include <algorithm>

namespace NErrMsg {
PaneRectangle GetPaneRectangle(const nw4r::lyt::Pane* pPane) {
    const nw4r::math::VEC3 position(pPane->GetTranslate());
    PaneRectangle rectangle;
    const nw4r::lyt::Size size(pPane->GetSize());
    switch (pPane->GetBasePositionH()) {
    case 0: rectangle.left = position.x; break;
    case 1: rectangle.left = position.x - size.width / 2.0f; break;
    case 2: rectangle.left = position.x - size.width; break;
    }
    rectangle.right = rectangle.left + size.width;
    switch (pPane->GetBasePositionV()) {
    case 2: rectangle.bottom = position.y; break;
    case 1: rectangle.bottom = position.y - size.height / 2.0f; break;
    case 0: rectangle.bottom = position.y - size.height; break;
    }
    rectangle.top = rectangle.bottom + size.height;
    return rectangle;
}
void SetPaneRectangle(nw4r::lyt::Pane* pPane, const PaneRectangle& rRectangle) {
    nw4r::math::VEC3 position = pPane->GetTranslate();
    switch (pPane->GetBasePositionH()) {
    case 0: position.x = rRectangle.left; break;
    case 1: position.x = rRectangle.left + rRectangle.GetWidth() / 2.0f; break;
    case 2: position.x = rRectangle.left + rRectangle.GetWidth(); break;
    }
    switch (pPane->GetBasePositionV()) {
    case 2: position.y = rRectangle.bottom; break;
    case 1: position.y = rRectangle.bottom + rRectangle.GetHeight() / 2.0f; break;
    case 0: position.y = rRectangle.bottom + rRectangle.GetHeight(); break;
    }
    pPane->SetTranslate(position);
    pPane->SetSize(nw4r::lyt::Size(rRectangle.GetWidth(), rRectangle.GetHeight()));
}
PaneRectangle UnionRectangles(const PaneRectangle& rLeft, const PaneRectangle& rRight) {
    PaneRectangle rectangle;
    rectangle.left = std::min(rLeft.left, rRight.left);
    rectangle.right = std::max(rLeft.right, rRight.right);
    rectangle.bottom = std::min(rLeft.bottom, rRight.bottom);
    rectangle.top = std::max(rLeft.top, rRight.top);
    return rectangle;
}
}
