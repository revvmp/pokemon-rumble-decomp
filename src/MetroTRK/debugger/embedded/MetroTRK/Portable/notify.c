#include <MetroTRK/Portable/notify.h>
#include <MetroTRK/Portable/msgbuf.h>
#include <MetroTRK/Portable/support.h>
#include <MetroTRK/ppc/Generic/targimpl.h>

DSError TRKDoNotifyStopped(MessageCommandID command) {
    int replyBufferID;
    int bufferID;
    TRKBuffer* buffer;
    DSError error;

    error = TRKGetFreeBuffer(&bufferID, &buffer);
    if (error == DS_NoError) {
        if (command == DSMSG_NotifyStopped) {
            TRKTargetAddStopInfo(buffer);
        } else {
            TRKTargetAddExceptionInfo(buffer);
        }
        error = TRKRequestSend(buffer, &replyBufferID);
        if (error == DS_NoError) {
            TRKReleaseBuffer(replyBufferID);
        }
        TRKReleaseBuffer(bufferID);
    }
    return error;
}
