#include <revolution/ncd.h>
#include <revolution/net.h>

s32 NETGetWirelessMacAddress(u8* macAddr) {
    return NCDiGetWirelessMacAddress(macAddr);
}
