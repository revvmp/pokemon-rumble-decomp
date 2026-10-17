#include <MetroTRK/Portable/msg.h>
#include <MetroTRK/Portable/msgbuf.h>

int TRKWriteUARTN(const void* data, u32 length);
void OSReport(const char*, int);
static u16 sSequence;

DSError TRKMessageSend(TRKBuffer* msg) {
    DSError error;
    if (sSequence == 0) {
        sSequence = 1;
    }
    *(u16*)(msg->data + 6) = sSequence++;
    error = TRKWriteUARTN(msg->data, msg->length);
    if (error != DS_NoError) {
        OSReport("MetroTRK - TRK_WriteUARTN returned %ld\n", error);
    }
    return DS_NoError;
}
