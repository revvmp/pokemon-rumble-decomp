#ifndef GAME_CMINIMAPMARKER_H
#define GAME_CMINIMAPMARKER_H

#include <game/ui/CLayout.h>

class CMiniMapMarker {
public:
    CMiniMapMarker();
    ~CMiniMapMarker();

    void Initialize(int type, const char* pLayoutName, bool background);
    int GetType() const { return mType; }
    CLayout& GetLayout() { return mLayout; }
    nw4r::lyt::Pane* GetPane() const { return mpMarker; }

private:
    int mType;
    CLayout mLayout;
    nw4r::lyt::Pane* mpMarker;
};

#endif
