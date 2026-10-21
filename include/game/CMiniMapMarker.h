#ifndef GAME_CMINIMAPMARKER_H
#define GAME_CMINIMAPMARKER_H

#include <game/CLayout.h>

class CMiniMapMarker {
public:
    CMiniMapMarker();
    ~CMiniMapMarker();

    void Initialize(int type, const char* pLayoutName, bool background);

private:
    int mType;
    CLayout mLayout;
    nw4r::lyt::Pane* mpMarker;
};

#endif
