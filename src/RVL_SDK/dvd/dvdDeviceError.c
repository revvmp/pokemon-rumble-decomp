#include "revolution/os.h"
#include "revolution/gx.h"
#include "revolution/dvd.h"
#include "revolution/os/OSThread.h"
#include "revolution/sc.h"

vu8 __OSDeviceCheckCode : 0x8000319C;
extern u16 OSSetFontEncode(u16);
void OSFatal(GXColor, GXColor, const char*);
extern void __DVDShowFatalMessage(void);
static u8 CheckBuffer[32] __attribute__ ((aligned(32)));

static volatile BOOL lowDone = TRUE;
static volatile u32 lowIntType = 0;

static void lowCallback(u32 intType) {
    lowIntType = intType;
    lowDone = TRUE;
}

static void __DVDShowDeviceErrorMessage(void);

BOOL __DVDCheckDevice(void) {
    u32 checkCode = 0x460A0000;
    u32 outOfRangeError = 0xFFFFFFFF;
    u32 reportKeyError = 0xFFFFFFFF;
    OSIOSRev iosRev;

    if (OSGetPhysicalMem2Size() == 0x08000000) {
        return TRUE;
    }

    __OSGetIOSRev(&iosRev);
    
    if (iosRev.major < 30 || iosRev.major >= 254) {
        return TRUE;
    }

    if (__OSDeviceCheckCode == 129) {
        checkCode = 0x7ED40000;
    }

    lowDone = FALSE;
    DVDLowUnencryptedRead((void*)CheckBuffer, 32, checkCode, lowCallback);

    while (!lowDone) {

    }

    switch (lowIntType) {
        case 2:
            break;
        case 1:
            goto invalid;
            break;
        default:
            goto fatal;
            break;
    }

    lowDone = FALSE;
    DVDLowRequestError(lowCallback);

    while (!lowDone) {

    }

    outOfRangeError = DVDLowGetImmBufferReg();

    switch (lowIntType) {
        case 1:
        {
            if ((DVDLowGetImmBufferReg() & 0xFF000000) != 0) {
                goto recover;
                break;
            }

            switch (DVDLowGetImmBufferReg() & 0xFFFFFF) {
                case 0x52100:
                    break;
                default:
                    goto invalid;
                    break;
            }

            break;
        }

        default:
            goto fatal;
            break;
    }

    lowDone = FALSE;
    DVDLowReportKey((DVDVideoReportKey*)CheckBuffer, 0x40000, 0, lowCallback);

    while (!lowDone) {

    }

    switch (lowIntType) {
        case 2:
            break;
        case 1:
            goto invalid;
            break;
        default:
            goto fatal;
            break;
    }

    lowDone = FALSE;
    DVDLowRequestError(lowCallback);

    while (!lowDone) {

    }

    reportKeyError = DVDLowGetImmBufferReg();
    switch (lowIntType) {
        case 1:
            if ((DVDLowGetImmBufferReg() & 0xFF000000) != 0) {
                goto recover;
                break;
            }

            switch (DVDLowGetImmBufferReg() & 0xFFFFFF) {
                case 0x53100:
                case 0x52000:
                    break;
                default:
                    goto invalid;
                    break;
            }

            break;

        default:
            goto fatal;
            break;
    }

valid:
    return TRUE;

invalid:
    __DVDShowDeviceErrorMessage();
    return FALSE;

recover:
    return FALSE;

fatal:
    __DVDShowFatalMessage();
    return FALSE;
}

const char* const __DVDDeviceErrorMessage[] = {
    "\n"
    "\n"
    "\n"
    "エラーコード００１。\n"
    "不明なデバイスが見つかりました。",

    "\n"
    "\n"
    "\n"
    "Error #001,\n"
    "unauthorized device has been detected.",

    "\n"
    "\n"
    "\n"
    "Fehler #001:\n"
    "Es wurde eine unzul\xE4""ssige Komponente\n"
    "entdeckt.",

    "\n"
    "\n"
    "\n"
    "Erreur 001:\n"
    "un dispositif non autoris\xE9"" a \xE9""t\xE9"" d\xE9""tect\xE9"".",

    "\n"
    "\n"
    "\n"
    "Error 001:\n"
    "Se ha detectado un dispositivo no\n"
    "autorizado.",

    "\n"
    "\n"
    "\n"
    "Errore #001:\n"
    "rilevato un dispositivo non autorizzato.",

    "\n"
    "\n"
    "\n"
    "Fout #001:\n"
    "ongeoorloofd onderdeel gevonden."
};

static void __DVDShowDeviceErrorMessage(void) {
    const char* message;
    const char* const* messageList;
    GXColor bg = { 0, 0, 0, 0 };
    GXColor fg = { 255, 255, 255, 0 };

    if (SCGetLanguage() == 0) {
        OSSetFontEncode(1);
    }
    else {
        OSSetFontEncode(0);
    }

    messageList = __DVDDeviceErrorMessage;

    if (SCGetLanguage() > 6) {
        message = messageList[1];
    }
    else {
        message = messageList[SCGetLanguage()];
    }

    OSFatal(fg, bg, message);
}
