#include <TMC_JPEG/tmcjpeg.h>
#include <string.h>

#define RGB565_TILE(x, y, stride) (((y) >> 2) * (stride) + ((x) >> 2))
#define RGB565_TEXEL(image, x, y, stride) (image)[((y) & 3) * 4 + RGB565_TILE(x, y, stride) * 16 + ((x) & 3)]

static s32 ConvertRGB565MCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGB565MCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGB565MCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGB565EdgeMCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGB565EdgeMCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);
static s32 ConvertRGB565EdgeMCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y);

typedef struct TMCJPEGValue {
    int value;
} TMCJPEGValue;

static inline TMCJPEGValue MakeValue(int value)
{
    TMCJPEGValue result;
    result.value = value;
    return result;
}

static inline int AddValues(TMCJPEGValue lhs, TMCJPEGValue rhs)
{
    return lhs.value + rhs.value;
}

s32 TMCJPEGSetupRGB565(TMCJPEGState* state, TMCJPEGEncoder* enc)
{
    switch (enc->sampling) {
    case TMCJPEG_SAMPLING_411:
        state->convert = ConvertRGB565MCU411;
        state->convertEdge = ConvertRGB565EdgeMCU411;
        break;
    case TMCJPEG_SAMPLING_422:
        state->convert = ConvertRGB565MCU422;
        state->convertEdge = ConvertRGB565EdgeMCU422;
        break;
    case TMCJPEG_SAMPLING_420:
        state->convert = ConvertRGB565MCU420;
        state->convertEdge = ConvertRGB565EdgeMCU420;
        break;
    default:
        return TMCJPEG_ERR_SAMPLING;
    }

    enc->alignedWidth = (enc->width / 4 + (enc->width % 4 != 0)) * 4;
    enc->alignedHeight = AddValues(MakeValue(enc->height / 4), MakeValue(enc->height % 4 != 0)) * 4;
    return 0;
}

static s32 ConvertRGB565MCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    s32 secondTexel;
    s32 thirdTexel;
    s32 fourthTexel;
    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    u8 r, g, b;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

    width = 32 / enc->scale;
    height = 8 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 4) {
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            secondTexel = texel;
            b = ((u8)(secondTexel << 3)) & 0xFF;
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
            *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;

            secondTexel = RGB565_TEXEL(image, px + 1, y, tileStride);
            r = (u8)(secondTexel >> 8) & 0xF8;
            g = (u8)(secondTexel >> 3) & 0xFC;
            b = ((u8)(secondTexel << 3)) & 0xFF;
            dstY[1] = (r * 77 + g * 150 + b * 29 + 128) >> 8;

            thirdTexel = RGB565_TEXEL(image, px + 2, y, tileStride);
            r = (u8)(thirdTexel >> 8) & 0xF8;
            g = (u8)(thirdTexel >> 3) & 0xFC;
            b = (u8)(thirdTexel << 3);
            dstY[2] = (r * 77 + g * 150 + b * 29 + 128) >> 8;

            fourthTexel = RGB565_TEXEL(image, px + 3, y, tileStride);
            r = (u8)(fourthTexel >> 8) & 0xF8;
            g = (u8)(fourthTexel >> 3) & 0xFC;
            b = (u8)(fourthTexel << 3);
            dstY[3] = (r * 77 + g * 150 + b * 29 + 128) >> 8;

            dstY += 4;
        }

        dstY += 32 - width;
        dstCb += (32 - width) >> 2;
        dstCr += (32 - width) >> 2;
    }

    return 0;
}

static s32 ConvertRGB565MCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{    s32 nextTexel;

    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    u8 r, g, b;
    u8* dstCb = buffer->pixels + 0x80;
    s32 width;
    u8* dstCr = buffer->pixels + 0xC0;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

    width = 16 / enc->scale;
    height = 8 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 2) {
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = ((u8)(texel << 3)) & 0xFF;
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
            *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;

            nextTexel = RGB565_TEXEL(image, px + 1, y, tileStride);
            r = (u8)(nextTexel >> 8) & 0xF8;
            g = (u8)(nextTexel >> 3) & 0xFC;
            b = (u8)(nextTexel << 3);
            dstY[1] = (r * 77 + g * 150 + b * 29 + 128) >> 8;

            dstY += 2;
        }

        dstY += 16 - width;
        dstCb += (16 - width) >> 1;
        dstCr += (16 - width) >> 1;
    }

    return 0;
}

static s32 ConvertRGB565MCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    const u16* image = (const u16*)enc->source;
    u8* dstY = buffer->pixels;
    u8 r, g, b;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 px;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

    width = 16 / enc->scale;
    height = 16 / enc->scale;
    xEnd = x + width;
    yEnd = y + height;

    for (; y < yEnd; y++) {
        for (px = x; px < xEnd; px += 2) {
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = (u8)(texel << 3);
            dstY[0] = (r * 77 + g * 150 + b * 29 + 128) >> 8;
            if ((y & 1) == 0) {
                *dstCb++ = ((b - dstY[0]) * 144 + 128) >> 8;
                *dstCr++ = ((r - dstY[0]) * 183 + 128) >> 8;
            }

            texel = (u64)RGB565_TEXEL(image, px + 1, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = (u8)(texel << 3);
            dstY[1] = (r * 77 + g * 150 + b * 29 + 128) >> 8;

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

static s32 ConvertRGB565EdgeMCU411(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    u8 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

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
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = (u8)(texel << 3);
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

static s32 ConvertRGB565EdgeMCU422(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    u8 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x80;
    s32 width;
    u8* dstCr = buffer->pixels + 0xC0;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

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
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = (u8)(texel << 3);
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

static s32 ConvertRGB565EdgeMCU420(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)
{
    u8* dstY = buffer->pixels;
    const u16* image = (const u16*)enc->source;
    u8 r, g, b;
    s32 px;
    u8* dstCb = buffer->pixels + 0x100;
    s32 width;
    u8* dstCr = buffer->pixels + 0x140;
    s32 xEnd;
    s32 height;
    s32 yEnd;
    s32 i;
    u16 texel;
    u32 tileStride = enc->alignedWidth / 4;

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
            texel = RGB565_TEXEL(image, px, y, tileStride);
            r = (u8)(texel >> 8) & 0xF8;
            g = (u8)(texel >> 3) & 0xFC;
            b = (u8)(texel << 3);
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
