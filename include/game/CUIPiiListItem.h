#ifndef GAME_CUIPIILISTITEM_H
#define GAME_CUIPIILISTITEM_H

#include <game/CPiiPersonalData.h>
#include <game/IUIListItem.h>
#include <memory>

class CUIPiiListItem : public IUIListItem {
public:
    const std::tr1::shared_ptr<CPiiPersonalData>& GetPPD() const { return mPPD; }

private:
    u32 mFlags;
    std::tr1::shared_ptr<CPiiPersonalData> mPPD;
    int mPlayer;
    u32 mSpeciesTypeMask;
    u32 mMoveTypeMask;
};

#endif
