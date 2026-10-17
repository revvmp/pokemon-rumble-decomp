#include <TMC_JPEG/tmcjpeg.h>
#include <string.h>

static s32 WriteScanHeader(TMCJPEGContext* ctx);

static s32 WriteFrameHeader(TMCJPEGContext* ctx)
{
    TMCJPEGStream* stream = &ctx->stream;
    u16 height = ctx->height;
    u16 width = ctx->width;
    u32 vSampling = 1 << ctx->vShift;
    u32 hSampling = 1 << ctx->hShift;

    TMCJPEGStreamPutWord(stream, 0xFFC0);
    TMCJPEGStreamPutWord(stream, 17);
    TMCJPEGStreamPutByte(stream, 8);
    TMCJPEGStreamPutWord(stream, height);
    TMCJPEGStreamPutWord(stream, width);
    TMCJPEGStreamPutByte(stream, 3);
    TMCJPEGStreamPutByte(stream, 0);
    TMCJPEGStreamPutBits(stream, hSampling, 4);
    TMCJPEGStreamPutBits(stream, vSampling, 4);
    TMCJPEGStreamPutByte(stream, 0);
    TMCJPEGStreamPutByte(stream, 1);
    TMCJPEGStreamPutByte(stream, 0x11);
    TMCJPEGStreamPutByte(stream, 1);
    TMCJPEGStreamPutByte(stream, 2);
    TMCJPEGStreamPutByte(stream, 0x11);
    TMCJPEGStreamPutByte(stream, 1);
    if (WriteScanHeader(ctx) < 0) {
        return TMCJPEG_ERR_ENCODE;
    }
    return 0;
}

static s32 WriteHeaders(TMCJPEGContext* ctx)
{
    TMCJPEGStream* stream = &ctx->stream;
    int i;
    s32* chrQuant = ctx->chrQuant;
    const s32* zigzag = ctx->zigzag;
    s32* lumQuant;

    TMCJPEGStreamPutWord(stream, 0xFFD8);
    TMCJPEGStreamPutWord(stream, 0xFFDB);
    TMCJPEGStreamPutWord(stream, 0x43);
    TMCJPEGStreamPutByte(stream, 0);
    lumQuant = ctx->lumQuant;
    for (i = 0; i < 64; i++) {
        TMCJPEGStreamPutByte(stream, lumQuant[zigzag[i]]);
    }

    TMCJPEGStreamPutWord(stream, 0xFFDB);
    TMCJPEGStreamPutWord(stream, 0x43);
    TMCJPEGStreamPutByte(stream, 1);
    for (i = 0; i < 64; i++) {
        TMCJPEGStreamPutByte(stream, chrQuant[zigzag[i]]);
    }

    TMCJPEGStreamPutWord(stream, 0xFFC4);
    TMCJPEGStreamPutWord(stream, 0x1F);
    TMCJPEGStreamPutByte(stream, 0x00);
    TMCJPEGStreamPutBytes(stream, ctx->dcLumBits, 16);
    TMCJPEGStreamPutBytes(stream, ctx->dcLumVals, 12);

    TMCJPEGStreamPutWord(stream, 0xFFC4);
    TMCJPEGStreamPutWord(stream, 0x1F);
    TMCJPEGStreamPutByte(stream, 0x01);
    TMCJPEGStreamPutBytes(stream, ctx->dcChrBits, 16);
    TMCJPEGStreamPutBytes(stream, ctx->dcChrVals, 12);

    TMCJPEGStreamPutWord(stream, 0xFFC4);
    TMCJPEGStreamPutWord(stream, 0xB5);
    TMCJPEGStreamPutByte(stream, 0x10);
    TMCJPEGStreamPutBytes(stream, ctx->acLumBits, 16);
    TMCJPEGStreamPutBytes(stream, ctx->acLumVals, 0xA2);

    TMCJPEGStreamPutWord(stream, 0xFFC4);
    TMCJPEGStreamPutWord(stream, 0xB5);
    TMCJPEGStreamPutByte(stream, 0x11);
    TMCJPEGStreamPutBytes(stream, ctx->acChrBits, 16);
    TMCJPEGStreamPutBytes(stream, ctx->acChrVals, 0xA2);

    if (ctx->restartInterval != 0) {
        TMCJPEGStreamPutWord(stream, 0xFFDD);
        TMCJPEGStreamPutWord(stream, 4);
        TMCJPEGStreamPutWord(stream, ctx->restartInterval);
    }

    if (WriteFrameHeader(ctx) < 0) {
        return TMCJPEG_ERR_ENCODE;
    }
    return 0;
}

static inline void SetEncoderFormat(TMCJPEGEncoder* enc, const TMCJPEGParam* param)
{
    enc->format = param->format;
}

static s32 SetParam(TMCJPEGParam* param, TMCJPEGContext* ctx)
{
    if (param->width == 0) {
        return TMCJPEG_ERR_PARAM;
    }
    if (param->height == 0) {
        return TMCJPEG_ERR_PARAM;
    }

    ctx->width = param->width;
    ctx->height = param->height;
    ctx->restartInterval = param->restartInterval;
    ctx->quantMode = param->quantMode;
    switch (ctx->quantMode) {
    case 1:
        if (param->quality == 0 || param->quality > 500) {
            return TMCJPEG_ERR_PARAM;
        }
        ctx->quality = param->quality;
        break;
    default:
        return TMCJPEG_ERR_PARAM;
    }

    switch (param->sampling) {
    case TMCJPEG_SAMPLING_411:
        ctx->sampling = TMCJPEG_SAMPLING_411;
        ctx->hShift = 2;
        ctx->vShift = 0;
        ctx->cbWidth = (ctx->width + 3) / 4;
        ctx->cbHeight = ctx->height;
        ctx->crWidth = (ctx->width + 3) / 4;
        ctx->crHeight = ctx->height;
        break;
    case TMCJPEG_SAMPLING_422:
        ctx->sampling = TMCJPEG_SAMPLING_422;
        ctx->hShift = 1;
        ctx->vShift = 0;
        ctx->cbWidth = (ctx->width + 1) / 2;
        ctx->cbHeight = ctx->height;
        ctx->crWidth = (ctx->width + 1) / 2;
        ctx->crHeight = ctx->height;
        break;
    case TMCJPEG_SAMPLING_420:
        ctx->sampling = TMCJPEG_SAMPLING_420;
        ctx->hShift = 1;
        ctx->vShift = 1;
        ctx->cbWidth = (ctx->width + 1) / 2;
        ctx->cbHeight = (ctx->height + 1) / 2;
        ctx->crWidth = (ctx->width + 1) / 2;
        ctx->crHeight = (ctx->height + 1) / 2;
        break;
    default:
        return TMCJPEG_ERR_PARAM;
    }

    if (param->format != TMCJPEG_FORMAT_RGB565 && param->format != TMCJPEG_FORMAT_RGBA8) {
        return TMCJPEG_ERR_FORMAT;
    }
    SetEncoderFormat(ctx->state.enc, param);
    return 0;
}

static inline s32 GetMinScale(const s32* scale)
{
    int i;
    s32 min = 0x10000;

    for (i = 0; i < 64; i++) {
        s32 value = 0x10000 / scale[i];
        if (min > value) {
            min = value;
        }
    }
    return min;
}

static void SetupPlanes(TMCJPEGContext* ctx)
{
    TMCJPEGStream* stream = &ctx->stream;
    s32 width;
    s32 height;
    s32 cbWidth;
    s32 cbHeight;
    s32 crWidth;
    s32 crHeight;
    s32* lumScale = ctx->lumScale;
    s32* chrScale = ctx->chrScale;
    const s32* zigzag;
    TMCJPEGState* state = &ctx->state;

    ctx->restartCount = ctx->restartInterval;
    zigzag = ctx->zigzag;
    ctx->restartIndex = 0;
    width = ctx->width;
    height = ctx->height;
    cbWidth = ctx->cbWidth;
    cbHeight = ctx->cbHeight;
    crWidth = ctx->crWidth;
    crHeight = ctx->crHeight;

    switch (ctx->sampling) {
    case TMCJPEG_SAMPLING_411:
        state->mcuX = 0;
        state->mcusX = (width + 31) >> 5;
        state->mcusY = (height + 7) >> 3;
        state->mcuY = 0;
        state->remainderX = width % 32;
        state->remainderY = height % 8;
        state->planes[0].width = width;
        state->planes[0].height = height;
        state->planes[0].lastWidth = width % 32 != 0 ? width % 32 : 32;
        state->planes[0].lastHeight = height % 8 != 0 ? height % 8 : 8;
        state->planes[0].mcuWidth = 32;
        state->planes[0].mcuHeight = 8;
        state->planes[0].blockCount = 4;
        state->planes[0].sampling = TMCJPEG_SAMPLING_411;
        state->planes[0].comp.stride = 32;
        state->planes[0].comp.lastDC = 0;
        state->planes[0].comp.minScale = GetMinScale(lumScale);
        state->planes[0].comp.huff = &ctx->lumHuff;
        state->planes[0].comp.scale = lumScale;
        state->planes[0].comp.zigzag = zigzag;
        state->planes[0].comp.stream = stream;

        state->planes[1].width = cbWidth;
        state->planes[1].height = cbHeight;
        state->planes[1].lastWidth = cbWidth % 8 != 0 ? cbWidth % 8 : 8;
        state->planes[1].lastHeight = cbHeight % 8 != 0 ? cbHeight % 8 : 8;
        state->planes[1].mcuWidth = 8;
        state->planes[1].mcuHeight = 8;
        state->planes[1].blockCount = 1;
        state->planes[1].sampling = TMCJPEG_SAMPLING_411;
        state->planes[1].comp.stride = 8;
        state->planes[1].comp.lastDC = 0;
        state->planes[1].comp.minScale = GetMinScale(chrScale);
        state->planes[1].comp.huff = &ctx->chrHuff;
        state->planes[1].comp.scale = chrScale;
        state->planes[1].comp.zigzag = zigzag;
        state->planes[1].comp.stream = stream;

        state->planes[2].width = crWidth;
        state->planes[2].height = crHeight;
        state->planes[2].lastWidth = crWidth % 8 != 0 ? crWidth % 8 : 8;
        state->planes[2].lastHeight = crHeight % 8 != 0 ? crHeight % 8 : 8;
        state->planes[2].mcuWidth = 8;
        state->planes[2].mcuHeight = 8;
        state->planes[2].blockCount = 1;
        state->planes[2].sampling = TMCJPEG_SAMPLING_411;
        state->planes[2].comp.stride = 8;
        state->planes[2].comp.lastDC = 0;
        state->planes[2].comp.minScale = state->planes[1].comp.minScale;
        state->planes[2].comp.huff = &ctx->chrHuff;
        state->planes[2].comp.scale = chrScale;
        state->planes[2].comp.zigzag = zigzag;
        state->planes[2].comp.stream = stream;
        break;
    case TMCJPEG_SAMPLING_420:
        state->mcuX = 0;
        state->mcusX = (width + 15) >> 4;
        state->mcusY = (height + 15) >> 4;
        state->mcuY = 0;
        state->remainderX = width % 16;
        state->remainderY = height % 16;
        state->planes[0].width = width;
        state->planes[0].height = height;
        state->planes[0].lastWidth = width % 16 != 0 ? width % 16 : 16;
        state->planes[0].lastHeight = height % 16 != 0 ? height % 16 : 16;
        state->planes[0].mcuWidth = 16;
        state->planes[0].mcuHeight = 16;
        state->planes[0].blockCount = 4;
        state->planes[0].sampling = TMCJPEG_SAMPLING_420;
        state->planes[0].comp.stride = 16;
        state->planes[0].comp.lastDC = 0;
        state->planes[0].comp.minScale = GetMinScale(lumScale);
        state->planes[0].comp.huff = &ctx->lumHuff;
        state->planes[0].comp.scale = lumScale;
        state->planes[0].comp.zigzag = zigzag;
        state->planes[0].comp.stream = stream;

        state->planes[1].width = cbWidth;
        state->planes[1].height = cbHeight;
        state->planes[1].lastWidth = cbWidth % 8 != 0 ? cbWidth % 8 : 8;
        state->planes[1].lastHeight = cbHeight % 8 != 0 ? cbHeight % 8 : 8;
        state->planes[1].mcuWidth = 8;
        state->planes[1].mcuHeight = 8;
        state->planes[1].blockCount = 1;
        state->planes[1].sampling = TMCJPEG_SAMPLING_420;
        state->planes[1].comp.stride = 8;
        state->planes[1].comp.lastDC = 0;
        state->planes[1].comp.minScale = GetMinScale(chrScale);
        state->planes[1].comp.huff = &ctx->chrHuff;
        state->planes[1].comp.scale = chrScale;
        state->planes[1].comp.zigzag = zigzag;
        state->planes[1].comp.stream = stream;

        state->planes[2].width = crWidth;
        state->planes[2].height = crHeight;
        state->planes[2].lastWidth = crWidth % 8 != 0 ? crWidth % 8 : 8;
        state->planes[2].lastHeight = crHeight % 8 != 0 ? crHeight % 8 : 8;
        state->planes[2].mcuWidth = 8;
        state->planes[2].mcuHeight = 8;
        state->planes[2].blockCount = 1;
        state->planes[2].sampling = TMCJPEG_SAMPLING_420;
        state->planes[2].comp.stride = 8;
        state->planes[2].comp.lastDC = 0;
        state->planes[2].comp.minScale = state->planes[1].comp.minScale;
        state->planes[2].comp.huff = &ctx->chrHuff;
        state->planes[2].comp.scale = chrScale;
        state->planes[2].comp.zigzag = zigzag;
        state->planes[2].comp.stream = stream;
        break;
    case TMCJPEG_SAMPLING_422:
        state->mcuX = 0;
        state->mcusX = (width + 15) >> 4;
        state->mcusY = (height + 7) >> 3;
        state->mcuY = 0;
        state->remainderX = width % 16;
        state->remainderY = height % 8;
        state->planes[0].width = width;
        state->planes[0].height = height;
        state->planes[0].lastWidth = width % 16 != 0 ? width % 16 : 16;
        state->planes[0].lastHeight = height % 8 != 0 ? height % 8 : 8;
        state->planes[0].mcuWidth = 16;
        state->planes[0].mcuHeight = 8;
        state->planes[0].blockCount = 2;
        state->planes[0].sampling = TMCJPEG_SAMPLING_422;
        state->planes[0].comp.stride = 16;
        state->planes[0].comp.lastDC = 0;
        state->planes[0].comp.minScale = GetMinScale(lumScale);
        state->planes[0].comp.huff = &ctx->lumHuff;
        state->planes[0].comp.scale = lumScale;
        state->planes[0].comp.zigzag = zigzag;
        state->planes[0].comp.stream = stream;

        state->planes[1].width = cbWidth;
        state->planes[1].height = cbHeight;
        state->planes[1].lastWidth = cbWidth % 8 != 0 ? cbWidth % 8 : 8;
        state->planes[1].lastHeight = cbHeight % 8 != 0 ? cbHeight % 8 : 8;
        state->planes[1].mcuWidth = 8;
        state->planes[1].mcuHeight = 8;
        state->planes[1].blockCount = 1;
        state->planes[1].sampling = TMCJPEG_SAMPLING_422;
        state->planes[1].comp.stride = 8;
        state->planes[1].comp.lastDC = 0;
        state->planes[1].comp.minScale = GetMinScale(chrScale);
        state->planes[1].comp.huff = &ctx->chrHuff;
        state->planes[1].comp.scale = chrScale;
        state->planes[1].comp.zigzag = zigzag;
        state->planes[1].comp.stream = stream;

        state->planes[2].width = crWidth;
        state->planes[2].height = crHeight;
        state->planes[2].lastWidth = crWidth % 8 != 0 ? crWidth % 8 : 8;
        state->planes[2].lastHeight = crHeight % 8 != 0 ? crHeight % 8 : 8;
        state->planes[2].mcuWidth = 8;
        state->planes[2].mcuHeight = 8;
        state->planes[2].blockCount = 1;
        state->planes[2].sampling = TMCJPEG_SAMPLING_422;
        state->planes[2].comp.stride = 8;
        state->planes[2].comp.lastDC = 0;
        state->planes[2].comp.minScale = state->planes[1].comp.minScale;
        state->planes[2].comp.huff = &ctx->chrHuff;
        state->planes[2].comp.scale = chrScale;
        state->planes[2].comp.zigzag = zigzag;
        state->planes[2].comp.stream = stream;
        break;
    }
}

void TMCJPEGWriteRestartMarker(TMCJPEGContext* ctx)
{
    TMCJPEGStreamPutMarker(&ctx->stream, 0xFFD0 + (ctx->restartIndex & 7));
    ctx->restartIndex++;
    ctx->state.planes[0].comp.lastDC = 0;
    ctx->state.planes[1].comp.lastDC = 0;
    ctx->state.planes[2].comp.lastDC = 0;
}

static void GetEncoderInfo(TMCJPEGContext* ctx, TMCJPEGEncoder* enc)
{
    enc->mcuX = 0;
    enc->mcuY = 0;
    enc->size = 0;
    enc->quality = ctx->quality;
    enc->mcusX = ctx->state.mcusX;
    enc->mcusY = ctx->state.mcusY;
    enc->mcuCount = enc->mcusX * enc->mcusY;
    enc->mcuWidth = ctx->state.planes[0].mcuWidth;
    enc->mcuHeight = ctx->state.planes[0].mcuHeight;
    enc->remainderX = ctx->state.remainderX;
    enc->remainderY = ctx->state.remainderY;
    enc->paddedWidth = ctx->state.remainderX == 0 ? (enc->mcusX + 1) * enc->mcuWidth : (enc->mcusX - 1) * enc->mcuWidth;
    enc->paddedHeight =
        ctx->state.remainderY == 0 ? (enc->mcusY + 1) * enc->mcuHeight : (enc->mcusY - 1) * enc->mcuHeight;
    enc->scale = 1;
    enc->sampling = ctx->sampling;
    enc->width = ctx->width;
    enc->height = ctx->height;
    enc->cbWidth = ctx->cbWidth;
    enc->cbHeight = ctx->cbHeight;
}

static void SetEncodeMCU(TMCJPEGContext* ctx, u8 sampling);

s32 TMCJPEGInitEncoder(TMCJPEGEncoder* enc, void* buffer, const void* image, TMCJPEGParam* param)
{
    TMCJPEGContext* ctx;
    s32 ret;

    ctx = param->work;
    if (ctx == NULL) {
        return TMCJPEG_ERR_PARAM;
    }

    memset(enc, 0, sizeof(TMCJPEGEncoder));
    memset(ctx, 0, sizeof(TMCJPEGContext));
    enc->ctx = ctx;
    ctx->state.enc = enc;

    ret = SetParam(param, ctx);
    if (ret < 0) {
        return ret;
    }

    TMCJPEGInitHuffTables(ctx);
    TMCJPEGBuildHuffTables(ctx);
    TMCJPEGBuildQuantTables(ctx);
    SetupPlanes(ctx);
    TMCJPEGStreamInit(&ctx->stream, buffer, param->streamSize);
    SetEncodeMCU(ctx, ctx->sampling);
    TMCJPEGSetEncodeEdgeMCU(ctx, ctx->sampling);

    if (WriteHeaders(ctx) < 0) {
        return TMCJPEG_ERR_ENCODE;
    }

    GetEncoderInfo(ctx, enc);

    switch (enc->format) {
    case TMCJPEG_FORMAT_RGB565:
        ret = TMCJPEGSetupRGB565(&ctx->state, enc);
        if (ret < 0) {
            return ret;
        }
        break;
    case TMCJPEG_FORMAT_RGBA8:
        ret = TMCJPEGSetupRGBA8(&ctx->state, enc);
        if (ret < 0) {
            return ret;
        }
        break;
    }

    TMCJPEGStreamGetSize(&ctx->stream, &enc->size);
    return enc->mcuCount;
}

s32 TMCJPEGEncodeMCUs(TMCJPEGEncoder* enc, s32 count, const void* image)
{
    s32 restart;
    s32 done;
    s32 total;
    s32 mcusX;
    s32 mcusY;
    s32 x;
    s32 y;
    TMCJPEGContext* ctx;
    s32 ret;
    TMCJPEGState* state;

    enc->image = image;
    enc->source = image;
    enc->plane1 = (const u8*)image + enc->alignedWidth * enc->alignedHeight;
    enc->plane2 = (const u8*)image + enc->plane2Width * enc->plane2Height;
    ctx = enc->ctx;
    state = &ctx->state;

    if (ctx->state.remainderY != 0 || state->remainderX != 0) {
        return TMCJPEGEncodeMCUsWithEdges(ctx, enc, count);
    }

    mcusX = state->mcusX;
    done = 0;
    mcusY = state->mcusY;
    x = enc->mcuX;
    total = mcusX * mcusY;
    y = enc->mcuY;
    restart = ctx->restartCount;

    while (y < mcusY && done < count) {
        while (x < mcusX && done < count) {
            ret = ctx->encodeMCU(x, y, state);
            if (ret < 0) {
                return ret;
            }
            done++;
            x++;
            if (--restart == 0) {
                TMCJPEGWriteRestartMarker(ctx);
                restart = ctx->restartInterval;
            }
        }
        if (x >= mcusX) {
            x = 0;
            y++;
        }
    }

    if (y == mcusY && x == 0) {
        TMCJPEGStreamPutEOI(&ctx->stream);
        if (ctx->stream.overflow == 1) {
            TMCJPEGStreamGetSize(&ctx->stream, &enc->size);
            return TMCJPEG_ERR_BUFFER;
        }
    }

    state->mcuX = x;
    enc->mcuX = x;
    state->mcuY = y;
    enc->mcuY = y;
    TMCJPEGStreamGetSize(&ctx->stream, &enc->size);
    ctx->restartCount = restart;
    return total - x - y * mcusX;
}

static s32 EncodeMCU411(s32 x, s32 y, TMCJPEGState* state);
static s32 EncodeMCU420(s32 x, s32 y, TMCJPEGState* state);
static s32 EncodeMCU422(s32 x, s32 y, TMCJPEGState* state);

static void SetEncodeMCU(TMCJPEGContext* ctx, u8 sampling)
{
    switch (sampling) {
    case TMCJPEG_SAMPLING_411:
        ctx->encodeMCU = EncodeMCU411;
        break;
    case TMCJPEG_SAMPLING_420:
        ctx->encodeMCU = EncodeMCU420;
        break;
    case TMCJPEG_SAMPLING_422:
        ctx->encodeMCU = EncodeMCU422;
        break;
    }
}

static s32 EncodeMCU411(s32 x, s32 y, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    ret = state->convert(&buffer, state->enc, x * 32, y * 8);
    if (ret < 0) {
        return ret;
    }

    luma = buffer.pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 16, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 24, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[1].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0x100, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[2].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0x140, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static s32 EncodeMCU420(s32 x, s32 y, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    ret = state->convert(&buffer, state->enc, x * 16, y * 16);
    if (ret < 0) {
        return ret;
    }

    luma = buffer.pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 0x80, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 0x88, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[1].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0x100, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[2].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0x140, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static s32 EncodeMCU422(s32 x, s32 y, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    ret = state->convert(&buffer, state->enc, x * 16, y * 8);
    if (ret < 0) {
        return ret;
    }

    luma = buffer.pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[1].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0x80, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    comp = &state->planes[2].comp;
    TMCJPEGForwardDCTSigned((s8*)buffer.pixels + 0xC0, coef, comp->stride);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static s32 WriteScanHeader(TMCJPEGContext* ctx)
{
    TMCJPEGStream* stream = &ctx->stream;

    TMCJPEGStreamPutWord(stream, 0xFFDA);
    TMCJPEGStreamPutWord(stream, 12);
    TMCJPEGStreamPutByte(stream, 3);
    TMCJPEGStreamPutByte(stream, 0);
    TMCJPEGStreamPutByte(stream, 0x00);
    TMCJPEGStreamPutByte(stream, 1);
    TMCJPEGStreamPutByte(stream, 0x11);
    TMCJPEGStreamPutByte(stream, 2);
    TMCJPEGStreamPutByte(stream, 0x11);
    TMCJPEGStreamPutByte(stream, 0);
    TMCJPEGStreamPutByte(stream, 0x3F);
    TMCJPEGStreamPutByte(stream, 0);
    return 0;
}

void TMCJPEGForwardDCT(const u8* src, s32* dst, s32 stride)
{
    s32 work[64];
    s32 z3;
    s32 tmp1;
    s32 tmp6;
    s32 tmp12;
    s32 z4;
    s32 tmp0;
    s32 tmp14;
    s32 z1;
    s32 tmp7;
    s32 tmp4;
    s32 tmp2;
    s32 tmp3;
    s32 tmp16;
    int i;
    s32 z11;
    s32 tmp15;
    s32 tmp5;
    s32 z2;
    s32 tmp10;
    s32 tmp13;
    s32* p;
    s32 tmp11;
    s32 z13;

    p = work;
    for (i = 0; i < 8; i++) {
        tmp0 = src[0] + src[7];
        tmp7 = src[0] - src[7];
        tmp1 = src[1] + src[6];
        tmp6 = src[1] - src[6];
        tmp2 = src[2] + src[5];
        tmp5 = src[2] - src[5];
        tmp3 = src[3] + src[4];
        tmp4 = src[3] - src[4];

        tmp14 = tmp4 + tmp5;
        tmp15 = tmp5 + tmp6;
        tmp16 = tmp6 + tmp7;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        z1 = ((tmp13 + tmp12) * 11585) >> 14;
        p[0] = tmp10 + tmp11 - 1024;
        p[32] = tmp10 - tmp11;
        p[16] = tmp13 + z1;
        p[48] = tmp13 - z1;

        z2 = (tmp14 * 15137 - tmp16 * 6270) >> 14;
        z4 = (tmp16 * 15137 + tmp14 * 6270) >> 14;
        z3 = (tmp15 * 11585) >> 14;

        z11 = tmp7 + z3;
        z13 = tmp7 - z3;

        p[8] = z11 + z4;
        p[56] = z11 - z4;
        p[24] = z13 - z2;
        p[40] = z13 + z2;

        src += stride;
        p++;
    }

    p = work;
    for (i = 0; i < 8; i++) {
        tmp0 = p[0] + p[7];
        tmp7 = p[0] - p[7];
        tmp1 = p[1] + p[6];
        tmp6 = p[1] - p[6];
        tmp2 = p[2] + p[5];
        tmp5 = p[2] - p[5];
        tmp3 = p[3] + p[4];
        tmp4 = p[3] - p[4];

        tmp14 = tmp4 + tmp5;
        tmp15 = tmp5 + tmp6;
        tmp16 = tmp6 + tmp7;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        z1 = ((tmp13 + tmp12) * 11585) >> 14;
        dst[0] = tmp10 + tmp11;
        dst[32] = tmp10 - tmp11;
        dst[16] = tmp13 + z1;
        dst[48] = tmp13 - z1;

        z2 = (tmp14 * 8867 + (tmp14 - tmp16) * 6270) >> 14;
        z4 = (tmp16 * 21407 + (tmp14 - tmp16) * 6270) >> 14;
        z3 = (tmp15 * 11585) >> 14;

        z11 = tmp7 + z3;
        z13 = tmp7 - z3;

        dst[40] = z13 + z2;
        dst[24] = z13 - z2;
        dst[8] = z11 + z4;
        dst[56] = z11 - z4;

        p += 8;
        dst++;
    }
}

void TMCJPEGForwardDCTSigned(const s8* src, s32* dst, s32 stride)
{
    s32 work[64];
    s32 tmp14;
    s32 tmp6;
    s32 tmp0;
    s32 z3;
    s32 tmp10;
    s32 z4;
    s32 tmp11;
    s32 tmp1;
    s32 tmp7;
    s32 z2;
    s32 tmp3;
    s32 tmp2;
    s32 tmp12;
    s32 z11;
    s32 tmp5;
    int i;
    s32 tmp16;
    s32 z1;
    s32* p;
    s32 z13;
    s32 tmp4;
    s32 tmp13;
    s32 tmp15;

    p = work;
    for (i = 0; i < 8; i++) {
        tmp0 = src[0] + src[7];
        tmp7 = src[0] - src[7];
        tmp1 = src[1] + src[6];
        tmp6 = src[1] - src[6];
        tmp2 = src[2] + src[5];
        tmp5 = src[2] - src[5];
        tmp3 = src[3] + src[4];
        tmp4 = src[3] - src[4];

        tmp14 = tmp4 + tmp5;
        tmp15 = tmp5 + tmp6;
        tmp16 = tmp6 + tmp7;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        z1 = ((tmp13 + tmp12) * 11585) >> 14;
        p[0] = tmp10 + tmp11;
        p[32] = tmp10 - tmp11;
        p[16] = tmp13 + z1;
        p[48] = tmp13 - z1;

        z2 = (tmp14 * 15137 - tmp16 * 6270) >> 14;
        z4 = (tmp16 * 15137 + tmp14 * 6270) >> 14;
        z3 = (tmp15 * 11585) >> 14;

        z11 = tmp7 + z3;
        z13 = tmp7 - z3;

        p[8] = z11 + z4;
        p[56] = z11 - z4;
        p[24] = z13 - z2;
        p[40] = z13 + z2;

        src += stride;
        p++;
    }

    p = work;
    for (i = 0; i < 8; i++) {
        tmp0 = p[0] + p[7];
        tmp7 = p[0] - p[7];
        tmp1 = p[1] + p[6];
        tmp6 = p[1] - p[6];
        tmp2 = p[2] + p[5];
        tmp5 = p[2] - p[5];
        tmp3 = p[3] + p[4];
        tmp4 = p[3] - p[4];

        tmp14 = tmp4 + tmp5;
        tmp15 = tmp5 + tmp6;
        tmp16 = tmp6 + tmp7;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        z1 = ((tmp13 + tmp12) * 11585) >> 14;
        dst[0] = tmp10 + tmp11;
        dst[32] = tmp10 - tmp11;
        dst[16] = tmp13 + z1;
        dst[48] = tmp13 - z1;

        z2 = (tmp14 * 8867 + (tmp14 - tmp16) * 6270) >> 14;
        z4 = (tmp16 * 21407 + (tmp14 - tmp16) * 6270) >> 14;
        z3 = (tmp15 * 11585) >> 14;

        z11 = tmp7 + z3;
        z13 = tmp7 - z3;

        dst[40] = z13 + z2;
        dst[24] = z13 - z2;
        dst[8] = z11 + z4;
        dst[56] = z11 - z4;

        p += 8;
        dst++;
    }
}
