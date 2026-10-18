#ifndef TMC_JPEG_TMCJPEG_H
#define TMC_JPEG_TMCJPEG_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TMCJPEGSampling {
    TMCJPEG_SAMPLING_411 = 0,
    TMCJPEG_SAMPLING_422 = 1,
    TMCJPEG_SAMPLING_420 = 2
} TMCJPEGSampling;

typedef enum TMCJPEGFormat {
    TMCJPEG_FORMAT_RGB565 = 4,
    TMCJPEG_FORMAT_RGBA8 = 6
} TMCJPEGFormat;

#define TMCJPEG_ERR_PARAM (-1)
#define TMCJPEG_ERR_ENCODE (-2)
#define TMCJPEG_ERR_BUFFER (-3)
#define TMCJPEG_ERR_FORMAT (-4)
#define TMCJPEG_ERR_SAMPLING (-0x70)
#define TMCJPEG_ERR_WORK (-0x90)

#define TMCJPEG_WORK_SIZE 0x164C
#define TMCJPEG_STREAM_SIZE 0x400
#define TMCJPEG_STREAM_LIMIT 0x5B4
#define TMCJPEG_STREAM_MIN 0x25F

typedef struct TMCJPEGStream {
    u8* start;
    u8* current;
    s32 bitsFree;
    u8 overflow;
    u32 size;
    u32 flushed;
} TMCJPEGStream;

typedef struct TMCJPEGHuffCodes {
    s32 count;
    u32* codes;
    u32* sizes;
} TMCJPEGHuffCodes;

typedef struct TMCJPEGHuffTable {
    TMCJPEGHuffCodes dc;
    TMCJPEGHuffCodes ac;
} TMCJPEGHuffTable;

typedef void (*TMCJPEGPutBitsFunc)(TMCJPEGStream* stream, u32 bits, s32 count);

typedef struct TMCJPEGComponent {
    u32 reserved;
    s32 stride;
    s32 lastDC;
    s32 minScale;
    TMCJPEGHuffTable* huff;
    s32* scale;
    const s32* zigzag;
    TMCJPEGStream* stream;
    TMCJPEGPutBitsFunc putBits;
} TMCJPEGComponent;

typedef struct TMCJPEGPlane {
    u32 reserved;
    u16 width;
    u16 height;
    u8 mcuWidth;
    u8 mcuHeight;
    u8 blockCount;
    u8 lastWidth;
    u8 lastHeight;
    u8 sampling;
    TMCJPEGComponent comp;
} TMCJPEGPlane;

typedef struct TMCJPEGMCUBuffer {
    u32 reserved;
    u8 pixels[0x180];
} TMCJPEGMCUBuffer;

struct TMCJPEGEncoder;
struct TMCJPEGState;

typedef s32 (*TMCJPEGConvertFunc)(TMCJPEGMCUBuffer* buffer, struct TMCJPEGEncoder* enc, s32 x, s32 y);

typedef struct TMCJPEGState {
    u16 mcusX;
    u16 mcusY;
    u16 mcuX;
    u16 mcuY;
    u8 remainderX;
    u8 remainderY;
    TMCJPEGPlane planes[3];
    struct TMCJPEGEncoder* enc;
    TMCJPEGConvertFunc convert;
    TMCJPEGConvertFunc convertEdge;
} TMCJPEGState;

struct TMCJPEGContext;

typedef s32 (*TMCJPEGEncodeMCUFunc)(s32 x, s32 y, TMCJPEGState* state);
typedef s32 (*TMCJPEGEncodeEdgeMCUFunc)(s32 x, s32 y, s32 edge, TMCJPEGState* state);

typedef struct TMCJPEGContext {
    u8 sampling;
    u16 width;
    u16 height;
    u16 cbWidth;
    u16 cbHeight;
    u16 crWidth;
    u16 crHeight;
    u8 hShift;
    u8 vShift;
    u32 restartInterval;
    u32 restartCount;
    u32 restartIndex;
    u8 quantMode;
    u32 quality;
    TMCJPEGEncodeMCUFunc encodeMCU;
    TMCJPEGEncodeEdgeMCUFunc encodeEdgeMCU;
    const u8* dcLumBits;
    const u8* dcLumVals;
    const u8* dcChrBits;
    const u8* dcChrVals;
    const u8* acLumBits;
    const u8* acLumVals;
    const u8* acChrBits;
    const u8* acChrVals;
    u32 dcLumCodes[16];
    u32 dcLumSizes[16];
    u32 dcChrCodes[16];
    u32 dcChrSizes[16];
    u32 acLumCodes[256];
    u32 acLumSizes[256];
    u32 acChrCodes[256];
    u32 acChrSizes[256];
    TMCJPEGHuffTable lumHuff;
    TMCJPEGHuffTable chrHuff;
    s32 lumQuant[64];
    s32 lumScale[64];
    s32 chrQuant[64];
    s32 chrScale[64];
    const s32* zigzag;
    TMCJPEGState state;
    TMCJPEGStream stream;
} TMCJPEGContext;

typedef struct TMCJPEGParam {
    void* work;
    s32 format;
    u16 width;
    u16 height;
    u8 sampling;
    u16 restartInterval;
    u8 quantMode;
    u32 quality;
    u32 reserved0;
    u8 reserved1[0x80];
    u32 streamSize;
} TMCJPEGParam;

typedef struct TMCJPEGEncoder {
    TMCJPEGContext* ctx;
    u32 reserved;
    s32 format;
    u16 mcuX;
    u16 mcuY;
    u32 size;
    u32 quality;
    s32 mcuCount;
    u16 mcusX;
    u16 mcusY;
    u8 mcuWidth;
    u8 mcuHeight;
    u8 remainderX;
    u8 remainderY;
    u32 paddedWidth;
    u32 paddedHeight;
    u8 scale;
    u8 reserved1;
    u8 sampling;
    u16 width;
    u16 height;
    u16 cbWidth;
    u16 cbHeight;
    u8 reserved2[0x10];
    u32 alignedWidth;
    u32 alignedHeight;
    s32 plane2Width;
    s32 plane2Height;
    const u8* image;
    const u8* plane1;
    const u8* plane2;
    const u8* source;
    u8 reserved3[0x694];
} TMCJPEGEncoder;

s32 TMCJPEGEncode(void* work, const void* image, u16 width, u16 height, s32 format, u32* outSize, u32 quality);
s32 TMCJPEGEncodeToBuffer(void* work, const void* image, u16 width, u16 height, s32 format, u32 quality, void* buffer,
                          u32 bufferSize, u32* outSize);

s32 TMCJPEGInitEncoder(TMCJPEGEncoder* enc, void* buffer, const void* image, TMCJPEGParam* param);
s32 TMCJPEGEncodeMCUs(TMCJPEGEncoder* enc, s32 count, const void* image);

s32 TMCJPEGEncodeBlock(s32* coef, TMCJPEGComponent* comp);
void TMCJPEGSetEncodeEdgeMCU(TMCJPEGContext* ctx, u8 sampling);
s32 TMCJPEGEncodeMCUsWithEdges(TMCJPEGContext* ctx, TMCJPEGEncoder* enc, s32 count);
void TMCJPEGWriteRestartMarker(TMCJPEGContext* ctx);
void TMCJPEGForwardDCT(const u8* src, s32* dst, s32 stride);
void TMCJPEGForwardDCTSigned(const s8* src, s32* dst, s32 stride);

void TMCJPEGInitHuffTables(TMCJPEGContext* ctx);
void TMCJPEGBuildHuffTables(TMCJPEGContext* ctx);
void TMCJPEGBuildQuantTables(TMCJPEGContext* ctx);

void TMCJPEGStreamInit(TMCJPEGStream* stream, void* buffer, u32 size);
void TMCJPEGStreamGetSize(TMCJPEGStream* stream, u32* size);
void TMCJPEGStreamPutByte(TMCJPEGStream* stream, u8 value);
void TMCJPEGStreamPutWord(TMCJPEGStream* stream, u16 value);
void TMCJPEGStreamPutBytes(TMCJPEGStream* stream, const u8* data, s32 count);
void TMCJPEGStreamPutMarker(TMCJPEGStream* stream, u16 marker);
void TMCJPEGStreamPutEOI(TMCJPEGStream* stream);
void TMCJPEGSelectPutBits(TMCJPEGComponent* comp);
void TMCJPEGStreamPutBits(TMCJPEGStream* stream, u32 bits, s32 count);

s32 TMCJPEGSetupRGBA8(TMCJPEGState* state, TMCJPEGEncoder* enc);
s32 TMCJPEGSetupRGB565(TMCJPEGState* state, TMCJPEGEncoder* enc);

#ifdef __cplusplus
}
#endif

#endif
