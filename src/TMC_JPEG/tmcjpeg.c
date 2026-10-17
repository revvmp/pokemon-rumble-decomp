#include <TMC_JPEG/tmcjpeg.h>
#include <revolution/os.h>

const char* __TMCJPEGVersion = "<< RVL_MWM - TMC_JPEG090323 \trelease build: Mar 23 2009 16:43:33 (0x4302_145) >>";

static inline s32 CheckEncodeArgs(u16 width, u16 height, s32 format)
{
    if (width == 0 || height == 0) {
        return TMCJPEG_ERR_PARAM;
    }
    if (format != TMCJPEG_FORMAT_RGB565 && format != TMCJPEG_FORMAT_RGBA8) {
        return TMCJPEG_ERR_FORMAT;
    }
    return 0;
}

s32 TMCJPEGEncode(void* work, const void* image, u16 width, u16 height, s32 format, u32* outSize, u32 quality)
{
    TMCJPEGParam param;
    TMCJPEGEncoder enc;
    s32 ret;
    s32 count;

    *outSize = 0;
    ret = CheckEncodeArgs(width, height, format);
    if (ret < 0) {
        return ret;
    }

    param.width = width;
    param.height = height;
    param.sampling = TMCJPEG_SAMPLING_420;
    param.quantMode = 1;
    param.restartInterval = 0;
    param.format = format;
    param.reserved0 = 0;
    param.quality = quality;
    param.work = work;
    param.streamSize = TMCJPEG_STREAM_SIZE;
    if (param.streamSize >= TMCJPEG_STREAM_LIMIT) {
        return TMCJPEG_ERR_WORK;
    }

    count = TMCJPEGInitEncoder(&enc, (u8*)work + TMCJPEG_WORK_SIZE, image, &param);
    if (count < 0) {
        return count;
    }

    do {
        ret = TMCJPEGEncodeMCUs(&enc, count, image);
        if (ret < 0) {
            if (ret == TMCJPEG_ERR_BUFFER) {
                break;
            }
            return ret;
        }
    } while (ret > 0);

    *outSize = enc.size;
    return 0;
}

static inline s32 CheckBufferArgs(u16 width, u16 height, s32 format, u32 bufferSize)
{
    if (width == 0 || height == 0) {
        return TMCJPEG_ERR_PARAM;
    }
    if (bufferSize < TMCJPEG_STREAM_MIN) {
        return TMCJPEG_ERR_BUFFER;
    }
    if (format != TMCJPEG_FORMAT_RGB565 && format != TMCJPEG_FORMAT_RGBA8) {
        return TMCJPEG_ERR_FORMAT;
    }
    return 0;
}

s32 TMCJPEGEncodeToBuffer(void* work, const void* image, u16 width, u16 height, s32 format, u32 quality, void* buffer,
                          u32 bufferSize, u32* outSize)
{
    TMCJPEGParam param;
    TMCJPEGEncoder enc;
    s32 ret;
    s32 count;

    OSRegisterVersion(__TMCJPEGVersion);
    *outSize = 0;
    param.quantMode = 1;
    ret = CheckBufferArgs(width, height, format, bufferSize);
    if (ret < 0) {
        return ret;
    }

    param.width = width;
    param.height = height;
    param.sampling = TMCJPEG_SAMPLING_420;
    param.quantMode = 1;
    param.quality = quality;
    param.restartInterval = 0;
    param.format = format;
    param.work = work;
    param.streamSize = bufferSize;

    count = TMCJPEGInitEncoder(&enc, buffer, image, &param);
    if (count < 0) {
        return count;
    }

    do {
        ret = TMCJPEGEncodeMCUs(&enc, count, image);
        if (ret < 0) {
            *outSize = enc.size;
            return ret;
        }
    } while (ret > 0);

    *outSize = enc.size;
    return 0;
}
