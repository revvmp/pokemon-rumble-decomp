#include <TMC_JPEG/tmcjpeg.h>
#include <string.h>

#define RGBA8_TILE(x, y, stride) (((y) >> 2) * (stride) + ((x) >> 2) * 2)
#define RGBA8_AR(image, x, y, stride) (image)[((y) & 3) * 4 + RGBA8_TILE(x, y, stride) * 16 + ((x) & 3)]
#define RGBA8_GB(image, x, y, stride) (image)[((y) & 3) * 4 + (RGBA8_TILE(x, y, stride) + 1) * 16 + ((x) & 3)]
#define RGBA8_LUMA(image, x, y, stride) (((RGBA8_AR(image, x, y, stride) & 0xFF) * 77 + (RGBA8_GB(image, x, y, stride) >> 8) * 150 + (RGBA8_GB(image, x, y, stride) & 0xFF) * 29 + 128) >> 8)

static s32 ConvertRGBA8MCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGBA8MCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGBA8MCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGBA8EdgeMCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGBA8EdgeMCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGBA8EdgeMCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);

typedef struct TMCJPEGTileCount {
    int value;
} TMCJPEGTileCount;

static inline TMCJPEGTileCount MakeTileCount(int value)
{
    TMCJPEGTileCount count;
    count.value = value;
    return count;
}

static inline int AddTileCounts(TMCJPEGTileCount lhs, TMCJPEGTileCount rhs)
{
    return lhs.value + rhs.value;
}

s32 TMCJPEGSetupRGBA8(TMCJPEGState* state, TMCJPEGEncoder* enc)
{
    switch (enc->sampling) {
    case TMCJPEG_SAMPLING_411:
        state->convert = ConvertRGBA8MCU411;
        state->convertEdge = ConvertRGBA8EdgeMCU411;
        break;
    case TMCJPEG_SAMPLING_422:
        state->convert = ConvertRGBA8MCU422;
        state->convertEdge = ConvertRGBA8EdgeMCU422;
        break;
    case TMCJPEG_SAMPLING_420:
        state->convert = ConvertRGBA8MCU420;
        state->convertEdge = ConvertRGBA8EdgeMCU420;
        break;
    default:
        return TMCJPEG_ERR_SAMPLING;
    }

    enc->alignedWidth = (enc->width / 4 + (enc->width % 4 != 0)) * 4;
    enc->alignedHeight = AddTileCounts(MakeTileCount(enc->height / 4), MakeTileCount(enc->height % 4 != 0)) * 4;
    return 0;
}

static s32 ConvertRGBA8MCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    s32 r, g, b;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    width = 32 / enc->scale;
    height = 8 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 4) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
            *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;

            dstY[1] = RGBA8_LUMA(image, px + 1, y, tileStride);

            dstY[2] = RGBA8_LUMA(image, px + 2, y, tileStride);

            dstY[3] = RGBA8_LUMA(image, px + 3, y, tileStride);

            dstY += 4;
        }

        dstY += 32 - width;
        dstCb += (32 - width) >> 2;
        dstCr += (32 - width) >> 2;
    }

    return 0;
}

static s32 ConvertRGBA8MCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    s32 r, g, b;
    u8* dstCb = buffer->pixels + 0x80;
    s32 width;
    u8* dstCr = buffer->pixels + 0xC0;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    width = 16 / enc->scale;
    height = 8 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 2) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
            *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;

            dstY[1] = ((image[(y & 3) * 4 + ((u32)((y >> 2) * tileStride) + ((px + 1) >> 2) * 2) * 16 + ((px + 1) & 3)] & 0xFF) * 77 + (RGBA8_GB(image, px + 1, y, tileStride) >> 8) * 150 + (RGBA8_GB(image, px + 1, y, tileStride) & 0xFF) * 29 + 128) >> 8;

            dstY += 2;
        }

        dstY += 16 - width;
        dstCb += (16 - width) >> 1;
        dstCr += (16 - width) >> 1;
    }

    return 0;
}

static s32 ConvertRGBA8MCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    s32 r, g, b;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    width = 16 / enc->scale;
    height = 16 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 2) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            if ((y & 1) == 0) {
                *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
                *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;
            }

            dstY[1] = RGBA8_LUMA(image, px + 1, y, tileStride);

            dstY += 2;
        }

        dstY += 16 - width;
        if ((y & 1) == 0) {
            dstCb += (16 - width) >> 1;
            dstCr += (16 - width) >> 1;
        }
    }

    return 0;
}

static s32 ConvertRGBA8EdgeMCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    s32 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    if (enc->paddedWidth == x) {
        width = enc->remainderX;
    } else {
        width = 32 / enc->scale;
    }

    if (enc->paddedHeight == y) {
        height = enc->remainderY;
    } else {
        height = 8 / enc->scale;
    }

    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px++) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            *dstY = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            if ((px & 3) == 0) {
                *dstCb++ = ((b - *dstY) * 144 + 128) >> 8;
                *dstCr++ = ((r - *dstY) * 183 + 128) >> 8;
            }
            dstY++;
        }

        for (i = width; i < 32; i++) {
            *dstY = dstY[-1];
            dstY++;
            if ((i & 3) == 0) {
                *dstCb = dstCb[-1];
                dstCb++;
                *dstCr = dstCr[-1];
                dstCr++;
            }
        }
    }

    for (; height < 8; height++) {
        memcpy(dstY, dstY - 32, 32);
        dstY += 32;
        memcpy(dstCb, dstCb - 8, 8);
        memcpy(dstCr, dstCr - 8, 8);
        dstCb += 8;
        dstCr += 8;
    }

    return 0;
}

static s32 ConvertRGBA8EdgeMCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    s32 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x80;
    s32 width;
    u8* dstCr = buffer->pixels + 0xC0;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    if (enc->paddedWidth == x) {
        width = enc->remainderX;
    } else {
        width = 16 / enc->scale;
    }

    if (enc->paddedHeight == y) {
        height = enc->remainderY;
    } else {
        height = 8 / enc->scale;
    }

    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px++) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            *dstY = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            if ((px & 1) == 0) {
                *dstCb++ = ((b - *dstY) * 144 + 128) >> 8;
                *dstCr++ = ((r - *dstY) * 183 + 128) >> 8;
            }
            dstY++;
        }

        for (i = width; i < 16; i++) {
            *dstY = dstY[-1];
            dstY++;
            if ((i & 1) == 0) {
                *dstCb = dstCb[-1];
                dstCb++;
                *dstCr = dstCr[-1];
                dstCr++;
            }
        }
    }

    for (; height < 8; height++) {
        memcpy(dstY, dstY - 16, 16);
        dstY += 16;
        memcpy(dstCb, dstCb - 8, 8);
        memcpy(dstCr, dstCr - 8, 8);
        dstCb += 8;
        dstCr += 8;
    }

    return 0;
}

static s32 ConvertRGBA8EdgeMCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    s32 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 ar, gb;
    u32 tileStride = enc->alignedWidth / 4 * 2;

    if (enc->paddedWidth == x) {
        width = enc->remainderX;
    } else {
        width = 16 / enc->scale;
    }

    if (enc->paddedHeight == y) {
        height = enc->remainderY;
    } else {
        height = 16 / enc->scale;
    }

    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px++) {
            ar = RGBA8_AR(image, px, y, tileStride);
            gb = RGBA8_GB(image, px, y, tileStride);
            r = ar & 0xFF;
            g = gb >> 8;
            b = gb & 0xFF;
            *dstY = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            if ((px & 1) == 0 && (y & 1) == 0) {
                *dstCb++ = ((b - *dstY) * 144 + 128) >> 8;
                *dstCr++ = ((r - *dstY) * 183 + 128) >> 8;
            }
            dstY++;
        }

        for (i = width; i < 16; i++) {
            *dstY = dstY[-1];
            dstY++;
            if ((i & 1) == 0 && (y & 1) == 0) {
                *dstCb = dstCb[-1];
                dstCb++;
                *dstCr = dstCr[-1];
                dstCr++;
            }
        }
    }

    for (; height < 16; height++) {
        memcpy(dstY, dstY - 16, 16);
        dstY += 16;
        if ((height & 1) == 0) {
            memcpy(dstCb, dstCb - 8, 8);
            memcpy(dstCr, dstCr - 8, 8);
            dstCb += 8;
            dstCr += 8;
        }
    }

    return 0;
}
