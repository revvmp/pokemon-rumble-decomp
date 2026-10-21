#include <game/CUIStar.h>
#include <game/StringFormat.h>

CUIStar::CUIStar() {}

CUIStar::~CUIStar() {}

void CUIStar::SetStars(int count, bool temporary) {
    for (int i = 0; i < 5; ++i) {
        GetLayout()->SetPaneVisible(FormatString("STAR%d", i).c_str(), i < count && !temporary);
        GetLayout()->SetPaneVisible(FormatString("TSTAR%d", i).c_str(), i < count && temporary);
    }
}
