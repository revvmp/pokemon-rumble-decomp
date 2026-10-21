#ifndef GAME_CBATTLEROYALDATA_H
#define GAME_CBATTLEROYALDATA_H

#include <memory>

class CBattleRoyalRecord {
public:
    float GetClearTime() const { return mClearTime; }

private:
    float mClearTime;
};

class CBattleRoyalData {
public:
    std::tr1::shared_ptr<CBattleRoyalRecord> GetRecord(int kind, int rank);
};

#endif
