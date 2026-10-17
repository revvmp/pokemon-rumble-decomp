#ifndef NW4R_SND_TYPES_H
#define NW4R_SND_TYPES_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

static const int CHANNEL_MIN = 1;
static const int CHANNEL_MAX = 2;

static const int REMOTE_FILTER_MAX = 127;

static const f32 VOLUME_MIN_DB = -90.4f;
static const f32 VOLUME_MAX_DB = 6.0f;
static const f32 VOLUME_RANGE_DB = -(VOLUME_MIN_DB - VOLUME_MAX_DB);
static const int VOLUME_RANGE_MB = static_cast<int>(10 * VOLUME_RANGE_DB);

enum OutputLineFlag {
    OUTPUT_LINE_MAIN = (1 << 0),
    OUTPUT_LINE_REMOTE_N = (1 << 1),
};

enum AuxBus { AUX_A, AUX_B, AUX_C, AUX_BUS_NUM };

enum SampleFormat {
    SAMPLE_FORMAT_PCM_S32,
    SAMPLE_FORMAT_PCM_S16,
    SAMPLE_FORMAT_PCM_S8,
    SAMPLE_FORMAT_DSP_ADPCM
};

enum OutputMode {
    OUTPUT_MODE_STEREO,
    OUTPUT_MODE_SURROUND,
    OUTPUT_MODE_DPL2,
    OUTPUT_MODE_MONO
};

struct VoiceOutParam {
    VoiceOutParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f) {}

    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 fxSend;
    f32 lpf;
};

struct SoundParam {
    SoundParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f),
          biquadFilterValue(0.0f),
          biquadFilterType(0),
          priority(0),
          userData(0) {}

    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 fxSend;
    f32 lpf;
    f32 biquadFilterValue;
    int biquadFilterType;
    int priority;
    u32 userData;
};

namespace detail {

enum PanMode {
    PAN_MODE_DUAL,
    PAN_MODE_BALANCE,
};

enum PanCurve {
    PAN_CURVE_SQRT,
    PAN_CURVE_SQRT_0DB,
    PAN_CURVE_SQRT_0DB_CLAMP,
    PAN_CURVE_SINCOS,
    PAN_CURVE_SINCOS_0DB,
    PAN_CURVE_SINCOS_0DB_CLAMP,
    PAN_CURVE_LINEAR,
    PAN_CURVE_LINEAR_0DB,
    PAN_CURVE_LINEAR_0DB_CLAMP,
};

struct AdpcmParam {
    u16 coef[16];
    u16 gain;
    u16 pred_scale;
    u16 yn1;
    u16 yn2;
};

struct AdpcmLoopParam {
    u16 loop_pred_scale;
    u16 loop_yn1;
    u16 loop_yn2;
};

struct AdpcmInfo {
    AdpcmParam param;
    AdpcmLoopParam loopParam;
    u16 PADDING_0x2E;
};

}
}
}

#endif
