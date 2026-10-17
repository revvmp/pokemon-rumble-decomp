#include <MetroTRK/Portable/main_TRK.h>
#include <MetroTRK/Portable/nubinit.h>
#include <MetroTRK/Portable/mainloop.h>

static DSError TRK_mainError;

DSError TRK_main(void) {
    TRK_mainError = TRKInitializeNub();

    if (TRK_mainError == DS_NoError) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }

    TRK_mainError = TRKTerminateNub();
    return TRK_mainError;
}
