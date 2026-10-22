#ifndef GAME_CUIPIILISTITEM_H
#define GAME_CUIPIILISTITEM_H

#include <game/CPiiPersonalData.h>
#include <game/IUIListItem.h>
#include <memory>

class CUIPiiListItem : public IUIListItem {
public:
    enum Flag {
        FLAG_SELL_MARKED = 1 << 1,
        FLAG_REMOTE_MARKED = 1 << 2
    };

    CUIPiiListItem(const std::tr1::shared_ptr<CPiiPersonalData>& rPersonalData, int player);
    virtual ~CUIPiiListItem();
    virtual bool IsSelectable() const;
    virtual void Update(CUIList* pList, CLayout* pLayout);
    const std::tr1::shared_ptr<CPiiPersonalData>& GetPPD() const { return mPPD; }
    int GetPlayer() const { return mPlayer; }
    u32 GetSpeciesTypeMask() const { return mSpeciesTypeMask; }
    u32 GetMoveTypeMask() const { return mMoveTypeMask; }
    bool IsSellMarked() const { return (mFlags & FLAG_SELL_MARKED) != 0; }
    bool IsRemoteMarked() const { return (mFlags & FLAG_REMOTE_MARKED) != 0; }
    void SetSellMarked(bool marked);
    void SetRemoteMarked(bool marked);

private:
    u32 mFlags;
    std::tr1::shared_ptr<CPiiPersonalData> mPPD;
    int mPlayer;
    u32 mSpeciesTypeMask;
    u32 mMoveTypeMask;
};

#endif
