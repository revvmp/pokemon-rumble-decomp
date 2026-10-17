#ifndef AX_H
#define AX_H

#include "revolution/types.h"

typedef struct _AXPBMIX {
    u16 vL;
    u16 vDeltaL;
    u16 vR;
    u16 vDeltaR;
    u16 vAuxAL;
    u16 vDeltaAuxAL;
    u16 vAuxAR;
    u16 vDeltaAuxAR;
    u16 vAuxBL;
    u16 vDeltaAuxBL;
    u16 vAuxBR;
    u16 vDeltaAuxBR;
    u16 vAuxCL;
    u16 vDeltaAuxCL;
    u16 vAuxCR;
    u16 vDeltaAuxCR;
    u16 vS;
    u16 vDeltaS;
    u16 vAuxAS;
    u16 vDeltaAuxAS;
    u16 vAuxBS;
    u16 vDeltaAuxBS;
    u16 vAuxCS;
    u16 vDeltaAuxCS;
} AXPBMIX;

typedef struct _AXPBITD {
    u16 flag;
    u16 bufferHi;
    u16 bufferLo;
    u16 shiftL;
    u16 shiftR;
    u16 targetShiftL;
    u16 targetShiftR;
} AXPBITD;

typedef struct _AXPBDPOP {
    s16 aL;
    s16 aAuxAL;
    s16 aAuxBL;
    s16 aAuxCL;
    s16 aR;
    s16 aAuxAR;
    s16 aAuxBR;
    s16 aAuxCR;
    s16 aS;
    s16 aAuxAS;
    s16 aAuxBS;
    s16 aAuxCS;
} AXPBDPOP;

typedef struct _AXPBVE {
    u16 currentVolume;
    s16 currentDelta;
} AXPBVE;

typedef struct _AXPBADDR {
    u16 loopFlag;
    u16 format;
    u16 loopAddressHi;
    u16 loopAddressLo;
    u16 endAddressHi;
    u16 endAddressLo;
    u16 currentAddressHi;
    u16 currentAddressLo;
} AXPBADDR;

typedef struct _AXPBADPCM {
    u16 a[8][2];
    u16 gain;
    u16 pred_scale;
    u16 yn1;
    u16 yn2;
} AXPBADPCM;

typedef struct _AXPBSRC {
    u16 ratioHi;
    u16 ratioLo;
    u16 currentAddressFrac;
    u16 last_samples[4];
} AXPBSRC;

typedef struct _AXPBADPCMLOOP {
    u16 loop_pred_scale;
    u16 loop_yn1;
    u16 loop_yn2;
} AXPBADPCMLOOP;

typedef struct _AXPBLPF {
    u16 on;
    u16 yn1;
    u16 a0;
    u16 b0;
} AXPBLPF;

typedef struct _AXPBBIQUAD {
    u16 on;
    u16 xn1;
    u16 xn2;
    u16 yn1;
    u16 yn2;
    u16 b0;
    u16 b1;
    u16 b2;
    u16 a1;
    u16 a2;
} AXPBBIQUAD;

typedef struct _AXPBRMTMIX {
    u16 vMain0;
    u16 vDeltaMain0;
    u16 vAux0;
    u16 vDeltaAux0;
    u16 vMain1;
    u16 vDeltaMain1;
    u16 vAux1;
    u16 vDeltaAux1;
    u16 vMain2;
    u16 vDeltaMain2;
    u16 vAux2;
    u16 vDeltaAux2;
    u16 vMain3;
    u16 vDeltaMain3;
    u16 vAux3;
    u16 vDeltaAux3;
} AXPBRMTMIX;

typedef struct _AXPBRMTDPOP {
    s16 aMain0;
    s16 aMain1;
    s16 aMain2;
    s16 aMain3;
    s16 aAux0;
    s16 aAux1;
    s16 aAux2;
    s16 aAux3;
} AXPBRMTDPOP;

typedef struct _AXPBRMTSRC {
    u16 currentAddressFrac;
    u16 last_samples[4];
} AXPBRMTSRC;

typedef union __AXPBRMTIIR {
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
} AXPBRMTIIR;

typedef struct _AXPB {
    u16 nextHi;
    u16 nextLo;
    u16 currHi;
    u16 currLo;
    u16 srcSelect;
    u16 coefSelect;
    u32 mixerCtrl;
    u16 state;
    u16 type;
    AXPBMIX mix;
    AXPBITD itd;
    AXPBDPOP dpop;
    AXPBVE ve;
    AXPBADDR addr;
    AXPBADPCM adpcm;
    AXPBSRC src;
    AXPBADPCMLOOP adpcmLoop;
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
    u16 remote;
    u16 rmtMixerCtrl;
    AXPBRMTMIX rmtMix;
    AXPBRMTDPOP rmtDpop;
    AXPBRMTSRC rmtSrc;
    AXPBRMTIIR rmtIIR;
    u8 padding[0x140 - 0x128];
} AXPB;

typedef void (*AXVoiceCallback)(void* vpb);

typedef struct _AXVPB {
    void* next;
    void* prev;
    void* next1;
    u32 priority;
    AXVoiceCallback callback;
    u32 userContext;
    u32 index;
    u32 sync;
    u32 depop;
    void* itdBuffer;
    AXPB pb;
} AXVPB;

typedef void (*AXAuxCallback)(void* chans, void* context);

void __AXSetPBDefault(AXVPB* vpb);

#endif
