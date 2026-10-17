#include <MetroTRK/Os/dolphin/targcont.h>

void TRKTargetSetStopped(BOOL stopped);
void UnreserveEXI2Port(void);
void TRKSwapAndGo(void);
void ReserveEXI2Port(void);

DSError TRKTargetContinue(void) {
    TRKTargetSetStopped(0);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return 0;
}
