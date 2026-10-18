#ifndef GAME_LUCKYHELPER_H
#define GAME_LUCKYHELPER_H

#include <revolution/types.h>

extern const u8 gLuckyNumberTable[100];

void ScrambleLuckyNumber(u16* pDigits);
u16 CalcLuckyNumberCheck(const u16* pDigits, const u8* pTable);

#endif
