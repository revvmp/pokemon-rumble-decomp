#ifndef RVL_SDK_BTE_BD_H
#define RVL_SDK_BTE_BD_H

#include <revolution/bte/bt_types.h>

void bdcpy(BD_ADDR a, const BD_ADDR b);

int bdcmp(const BD_ADDR a, const BD_ADDR b);

extern const BD_ADDR bd_addr_null;

#endif
