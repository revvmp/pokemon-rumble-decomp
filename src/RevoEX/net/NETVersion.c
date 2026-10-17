#include <revolution/net.h>

const char* NETRexPPCVersionPrintableString = "<< REX-PPC 2.4.255.0 (RevoEX-2.4) REL 090609111526 >>";

#pragma force_active on
const char* NETGetRexPPCVersionPrintable(void) {
    return NETRexPPCVersionPrintableString;
}
#pragma force_active reset
