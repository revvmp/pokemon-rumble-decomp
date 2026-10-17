#include <TMC_JPEG/tmcjpeg.h>

s32 TMCJPEGEncodeBlock(s32* coef, TMCJPEGComponent* comp)
{
    u16 mask[17] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF,
                    0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};
    TMCJPEGHuffTable* huff;
    s32 value;
    s32 nbits;
    s32 k;
    s32 run;
    s32* scale;
    TMCJPEGStream* stream;
    const s32* zigzag;
    TMCJPEGPutBitsFunc putBits;
    s32 dc;
    s32 diff;
    s32 size;
    s32 magnitude;
    u32 bits;
    s32 free;

    zigzag = comp->zigzag + 1;
    stream = comp->stream;
    TMCJPEGSelectPutBits(comp);
    putBits = comp->putBits;
    scale = comp->scale;
    huff = comp->huff;

    dc = coef[0];
    if (dc < 0) {
        dc = -dc;
        dc = -((scale[0] * dc + 0x8000) >> 16);
    } else {
        dc = (scale[0] * dc + 0x8000) >> 16;
    }

    diff = dc - comp->lastDC;
    comp->lastDC = dc;
    magnitude = diff;
    if (diff < 0) {
        magnitude = -diff;
        diff--;
    }

    size = 0;
    while (magnitude > 0) {
        magnitude >>= 1;
        size++;
    }

    if (size >= huff->dc.count) {
        return TMCJPEG_ERR_ENCODE;
    }

    free = 32 - huff->dc.sizes[size];
    bits = huff->dc.codes[size];
    free -= size;
    bits <<= size;
    bits |= diff & mask[size];
    if (free < 16) {
        putBits(stream, bits, 32 - free);
        bits = 0;
        free = 32;
    }

    run = 0;
    for (k = 1; k < 64; k++) {
        s32 index = *zigzag++;
        s32 c = coef[index];
        s32 s = scale[index];

        if (c <= 0) {
            value = -((-c * s + 0x8000) >> 16);
        } else {
            value = (c * s + 0x8000) >> 16;
        }

        if (value == 0) {
            run++;
            continue;
        }

        while (run > 15) {
            run -= 16;
            free -= huff->ac.sizes[0xF0];
            bits <<= huff->ac.sizes[0xF0];
            bits += huff->ac.codes[0xF0];
            if (free < 16) {
                putBits(stream, bits, 32 - free);
                bits = 0;
                free = 32;
            }
        }

        magnitude = value;
        if (value < 0) {
            magnitude = -value;
            value--;
        }

        nbits = 0;
        do {
            nbits++;
        } while (magnitude >> nbits);

        if (nbits >= huff->ac.count) {
            return TMCJPEG_ERR_ENCODE;
        }

        free -= huff->ac.sizes[(run << 4) + nbits];
        bits <<= huff->ac.sizes[(run << 4) + nbits];
        bits += huff->ac.codes[(run << 4) + nbits];
        run = 0;
        if (free < 16) {
            putBits(stream, bits, 32 - free);
            bits = 0;
            free = 32;
        }

        free -= nbits;
        bits <<= nbits;
        bits |= value & mask[nbits];
        if (free < 16) {
            putBits(stream, bits, 32 - free);
            bits = 0;
            free = 32;
        }
    }

    if (run > 0) {
        free -= huff->ac.sizes[0];
        bits <<= huff->ac.sizes[0];
        bits += huff->ac.codes[0];
        putBits(stream, bits, 32 - free);
    } else if (free != 32) {
        putBits(stream, bits, 32 - free);
    }

    return 0;
}

static s32 EncodeEdgeMCU411(s32 x, s32 y, s32 edge, TMCJPEGState* state);
static s32 EncodeEdgeMCU420(s32 x, s32 y, s32 edge, TMCJPEGState* state);
static s32 EncodeEdgeMCU422(s32 x, s32 y, s32 edge, TMCJPEGState* state);

void TMCJPEGSetEncodeEdgeMCU(TMCJPEGContext* ctx, u8 sampling)
{
    switch (sampling) {
    case TMCJPEG_SAMPLING_411:
        ctx->encodeEdgeMCU = EncodeEdgeMCU411;
        break;
    case TMCJPEG_SAMPLING_420:
        ctx->encodeEdgeMCU = EncodeEdgeMCU420;
        break;
    case TMCJPEG_SAMPLING_422:
        ctx->encodeEdgeMCU = EncodeEdgeMCU422;
        break;
    }
}

s32 TMCJPEGEncodeMCUsWithEdges(TMCJPEGContext* ctx, TMCJPEGEncoder* enc, s32 count)
{
    TMCJPEGState* state;
    s32 x;
    s32 y;
    s32 lastX;
    s32 lastY;
    s32 ret;
    s32 total;
    s32 done;
    s32 restart;

    state = &ctx->state;
    done = 0;
    restart = ctx->restartCount;
    lastX = state->mcusX - 1;
    y = state->mcuY;
    total = state->mcusX * state->mcusY;
    x = state->mcuX;
    lastY = state->mcusY - 1;

    while (y < lastY && done < count) {
        while (x < lastX && done < count) {
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
        if (done < count) {
            ret = ctx->encodeEdgeMCU(x, y, 0, state);
            if (ret < 0) {
                return ret;
            }
            x = 0;
            done++;
            y++;
            if (--restart == 0) {
                TMCJPEGWriteRestartMarker(ctx);
                restart = ctx->restartInterval;
            }
        }
    }

    while (x < lastX && done < count) {
        ret = ctx->encodeEdgeMCU(x, y, 1, state);
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

    if (done < count) {
        ret = ctx->encodeEdgeMCU(x, y, 2, state);
        if (ret < 0) {
            return ret;
        }
        x = 0;
        y++;
        if (--restart == 0) {
            TMCJPEGWriteRestartMarker(ctx);
            restart = ctx->restartInterval;
        }
        TMCJPEGStreamPutEOI(&ctx->stream);
        if (ctx->stream.overflow) {
            TMCJPEGStreamGetSize(&ctx->stream, &enc->size);
            return TMCJPEG_ERR_BUFFER;
        }
    }

    state->mcuX = x;
    enc->mcuX = x;
    state->mcuY = y;
    enc->mcuY = y;
    ctx->restartCount = restart;
    TMCJPEGStreamGetSize(&ctx->stream, &enc->size);
    return total - x - y * state->mcusX;
}

static s32 EncodeEdgeMCU411(s32 x, s32 y, s32 edge, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    TMCJPEGMCUBuffer* buf;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    buf = &buffer;
    ret = state->convertEdge(buf, state->enc, x * 32, y * 8);
    if (ret < 0) {
        return ret;
    }

    luma = buf->pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, 32);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, 32);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 16, coef, 32);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 24, coef, 32);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0x100, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[1].comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0x140, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[2].comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static s32 EncodeEdgeMCU420(s32 x, s32 y, s32 edge, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    TMCJPEGMCUBuffer* buf;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    buf = &buffer;
    ret = state->convertEdge(buf, state->enc, x * 16, y * 16);
    if (ret < 0) {
        return ret;
    }

    luma = buf->pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 0x80, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 0x88, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0x100, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[1].comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0x140, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[2].comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

static s32 EncodeEdgeMCU422(s32 x, s32 y, s32 edge, TMCJPEGState* state)
{
    s32 coef[64];
    TMCJPEGMCUBuffer buffer;
    TMCJPEGMCUBuffer* buf;
    u8* luma;
    TMCJPEGComponent* comp;
    s32 ret;

    buf = &buffer;
    ret = state->convertEdge(buf, state->enc, x * 16, y * 8);
    if (ret < 0) {
        return ret;
    }

    luma = buf->pixels;
    comp = &state->planes[0].comp;
    TMCJPEGForwardDCT(luma, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCT(luma + 8, coef, 16);
    ret = TMCJPEGEncodeBlock(coef, comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0x80, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[1].comp);
    if (ret < 0) {
        return ret;
    }
    TMCJPEGForwardDCTSigned((s8*)buf->pixels + 0xC0, coef, 8);
    ret = TMCJPEGEncodeBlock(coef, &state->planes[2].comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}
