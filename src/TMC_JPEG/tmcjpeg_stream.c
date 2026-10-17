#include <TMC_JPEG/tmcjpeg.h>

static const u8 sBitMask[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

static void PutBitsFast(TMCJPEGStream* stream, u32 bits, s32 count);

static inline void CheckOverflow(TMCJPEGStream* stream)
{
    u8* start = stream->start;
    u32 used = stream->current - start;

    if (used >= stream->size) {
        stream->overflow = TRUE;
        stream->flushed += used;
        stream->current = start;
    }
}

static inline void WriteByte(TMCJPEGStream* stream, u8 value)
{
    *stream->current++ = value;
    CheckOverflow(stream);
}

static inline void FlushBits(TMCJPEGStream* stream)
{
    if (stream->bitsFree != 8) {
        TMCJPEGStreamPutBits(stream, sBitMask[stream->bitsFree], stream->bitsFree);
    }
}

void TMCJPEGStreamInit(TMCJPEGStream* stream, void* buffer, u32 size)
{
    stream->start = buffer;
    stream->current = buffer;
    stream->bitsFree = 8;
    stream->size = size;
    stream->flushed = 0;
    stream->overflow = FALSE;
}

void TMCJPEGStreamGetSize(TMCJPEGStream* stream, u32* size)
{
    *size = stream->flushed + (stream->current - stream->start);
}

void TMCJPEGStreamPutByte(TMCJPEGStream* stream, u8 value)
{
    WriteByte(stream, value);
    *stream->current = 0;
}

void TMCJPEGStreamPutWord(TMCJPEGStream* stream, u16 value)
{
    WriteByte(stream, value >> 8);
    WriteByte(stream, value);
    *stream->current = 0;
}

void TMCJPEGStreamPutBytes(TMCJPEGStream* stream, const u8* data, s32 count)
{
    int i;

    for (i = 0; i < count; i++) {
        CheckOverflow(stream);
        *stream->current = *data++;
        stream->current++;
    }
    *stream->current = 0;
}

void TMCJPEGStreamPutMarker(TMCJPEGStream* stream, u16 marker)
{
    FlushBits(stream);
    CheckOverflow(stream);
    TMCJPEGStreamPutWord(stream, marker);
}

void TMCJPEGStreamPutEOI(TMCJPEGStream* stream)
{
    FlushBits(stream);
    CheckOverflow(stream);
    WriteByte(stream, 0xFF);
    *stream->current++ = 0xD9;
}

void TMCJPEGSelectPutBits(TMCJPEGComponent* comp)
{
    TMCJPEGStream* stream = comp->stream;

    if (stream->current - stream->start >= stream->size - 0x100) {
        comp->putBits = TMCJPEGStreamPutBits;
    } else {
        comp->putBits = PutBitsFast;
    }
}

static void PutBitsFast(TMCJPEGStream* stream, u32 bits, s32 count)
{
    s32 byte = *stream->current;
    s32 free = stream->bitsFree;

    while (count > free) {
        byte |= sBitMask[free] & ((s32)bits >> (count - free));
        *stream->current++ = byte;
        if (byte == 0xFF) {
            *stream->current++ = 0;
        }
        count -= free;
        free = 8;
        byte = 0;
    }

    if (count == free) {
        byte |= bits & sBitMask[count];
        stream->bitsFree = 8;
        *stream->current++ = byte;
        if (byte == 0xFF) {
            *stream->current++ = 0;
        }
        *stream->current = 0;
    } else {
        free -= count;
        byte |= (bits & sBitMask[count]) << free;
        *stream->current = byte;
        stream->bitsFree = free;
    }
}

void TMCJPEGStreamPutBits(TMCJPEGStream* stream, u32 bits, s32 count)
{
    s32 byte = *stream->current;
    s32 free = stream->bitsFree;

    while (count > free) {
        byte |= sBitMask[free] & ((s32)bits >> (count - free));
        WriteByte(stream, byte);
        if (byte == 0xFF) {
            WriteByte(stream, 0);
        }
        count -= free;
        free = 8;
        byte = 0;
    }

    if (count == free) {
        byte |= bits & sBitMask[count];
        stream->bitsFree = 8;
        WriteByte(stream, byte);
        if (byte == 0xFF) {
            WriteByte(stream, 0);
        }
        *stream->current = 0;
    } else {
        free -= count;
        byte |= (bits & sBitMask[count]) << free;
        *stream->current = byte;
        stream->bitsFree = free;
    }
}
