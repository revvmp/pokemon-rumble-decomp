#include "revolution/os.h"
#include "revolution/wpad.h"
#include "revolution/wud/wud.h"
#include <math.h>
#include <string.h>

#define WPAD_EXT_REG_ADDR 0x04A40000
#define WPAD_EXT_REG_KEY 0x04A40040
#define WPAD_EXT_REG_CONFIG 0x04A40020
#define WPAD_EXT_REG_INIT 0x04A400F0
#define WPAD_EXT_REG_BLC 0x04A400F1
#define WPAD_EXT_REG_TYPE 0x04A400FA
#define WPAD_EXT_REG_INIT2 0x04A400FB

#define WPAD_EXT_ENC_NONE 0
#define WPAD_EXT_ENC_INIT 1
#define WPAD_EXT_ENC_MAIN 2
#define WPAD_EXT_ENC_3RD 3

#define WPAD_DEV_VSM 4
#define WPAD_DEV_TRAIN 16
#define WPAD_DEV_GUITAR 17
#define WPAD_DEV_DRUM 18
#define WPAD_DEV_INITIALIZING 255

#define WPAD_DPD_STANDARD 3
#define WPAD_DPD_BASIC 1

#define DPD_MAX_X (WPAD_DPD_IMG_RESO_WX - 1)
#define DPD_MAX_Y (WPAD_DPD_IMG_RESO_WY - 1)

#define HID_BUTTON_MASK 0x9F1F
#define HID_BUTTON_FS_MASK 0x6000

#define MAKE_BUTTON(high, low) ((u16)((u16)((u16)((u16)(low) & 0x00FF) | (u16)((u16)((high) << 8) & 0xFF00)) & HID_BUTTON_MASK))
#define MAKE_CONFIG_ACC_X(high, low) ((u16)(((u16)(high) << 2) & 0xFFFC) | (u16)(((u16)(low) >> 4) & 0x0003))
#define MAKE_CONFIG_ACC_Y(high, low) ((u16)(((u16)(high) << 2) & 0xFFFC) | (u16)(((u16)(low) >> 2) & 0x0003))
#define MAKE_CONFIG_ACC_Z(high, low) ((u16)(((u16)(high) << 2) & 0xFFFC) | (u16)((u16)(low) & 0x0003))

extern BOOL __OSInIPL;

typedef void (*WPADReportParser)(u8 chan, u8* data, void* buf);

static void __a1_20_status_report(u8 chan, u8* data, void* buf);
static void __a1_21_user_data(u8 chan, u8* data, void* buf);
static void __a1_22_ack(u8 chan, u8* data, void* buf);
static void __a1_30_data_type(u8 chan, u8* data, void* buf);
static void __a1_31_data_type(u8 chan, u8* data, void* buf);
static void __a1_32_data_type(u8 chan, u8* data, void* buf);
static void __a1_33_data_type(u8 chan, u8* data, void* buf);
static void __a1_34_data_type(u8 chan, u8* data, void* buf);
static void __a1_35_data_type(u8 chan, u8* data, void* buf);
static void __a1_36_data_type(u8 chan, u8* data, void* buf);
static void __a1_37_data_type(u8 chan, u8* data, void* buf);
static void __a1_3d_data_type(u8 chan, u8* data, void* buf);
static void __a1_3e_data_type(u8 chan, u8* data, void* buf);
static void __a1_3f_data_type(u8 chan, u8* data, void* buf);
static void __a1_unused_report(u8 chan, u8* data, void* buf);

static const u8 _cExtInvalidData[21] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static WPADReportParser __a1_input_reports_array[] = {
    __a1_20_status_report, __a1_21_user_data,  __a1_22_ack,        __a1_unused_report, __a1_unused_report, __a1_unused_report,
    __a1_unused_report,    __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report,
    __a1_unused_report,    __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_30_data_type,  __a1_31_data_type,
    __a1_32_data_type,     __a1_33_data_type,  __a1_34_data_type,  __a1_35_data_type,  __a1_36_data_type,  __a1_37_data_type,
    __a1_unused_report,    __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_3d_data_type,
    __a1_3e_data_type,     __a1_3f_data_type,
};

static f32 _wpadCalibrationX[WPAD_MAX_CONTROLLERS];
static f32 _wpadCalibrationY[WPAD_MAX_CONTROLLERS];
static f32 _wpadCenterX[WPAD_MAX_CONTROLLERS];
static f32 _wpadCenterY[WPAD_MAX_CONTROLLERS];
static f32 _wpadRolag[WPAD_MAX_CONTROLLERS];
static u8 _wpadExtData[21];

static u8 _wpadDevType[WPAD_MAX_CONTROLLERS];
static u8 _wpadDevMode[WPAD_MAX_CONTROLLERS];
static u8 _wpadCLCompt[WPAD_MAX_CONTROLLERS];
static u8 _wpadExtInitRetryCnt[WPAD_MAX_CONTROLLERS];
static u8 _recv_3f[WPAD_MAX_CONTROLLERS];
static u8 _recv_3e[WPAD_MAX_CONTROLLERS];

static void __wpadInitExtension(s32 chan);

static void __wpadAbortInitExtension(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 devType;

    if (result == WPAD_ERR_NONE) {
        return;
    }

    WPADiClearQueue(&p_wpd->extq);
    p_wpd->extEnc = WPAD_EXT_ENC_NONE;

    if (result == WPAD_ERR_NO_CONTROLLER) {
        devType = WPAD_DEV_NOT_FOUND;
    } else if (p_wpd->infos.attach) {
        if (_wpadExtInitRetryCnt[chan]++ < 32) {
            __wpadInitExtension(chan);
            return;
        }
        devType = WPAD_DEV_NOT_SUPPORTED;
    } else {
        DEBUGPrint("detaching extension during initialization.\n");
        WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, NULL);
        return;
    }

    p_wpd->devType = devType;
    p_wpd->savedDevType = devType;
    p_wpd->extDisconnected = FALSE;

    if (p_wpd->extensionCallback) {
        p_wpd->extensionCallback(chan, devType);
    }
}

static void __wpadGetDevConfig(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8* data = (u8*)p_wpd->readBufPtr;
    u8 checksum;
    u8 i;
    int j;
    int index;
    f32 x;
    f32 y;
    f32 dx;
    f32 dy;
    DPDObject obj[WPAD_DPD_MAX_OBJECTS];
    f32 a[WPAD_DPD_MAX_OBJECTS];
    f32 b[WPAD_DPD_MAX_OBJECTS];
    f32 c[WPAD_DPD_MAX_OBJECTS];
    f32 d[WPAD_DPD_MAX_OBJECTS];
    s16 defaultX[WPAD_DPD_MAX_OBJECTS] = {127, 896, 896, 127};
    s16 defaultY[WPAD_DPD_MAX_OBJECTS] = {93, 93, 674, 674};

    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        p_wpd->conf.obj[i].x = defaultX[i];
        p_wpd->conf.obj[i].y = defaultY[i];
        p_wpd->conf.obj[i].size = p_wpd->dpdDummyObjSize;
        p_wpd->conf.obj[i].traceId = i;
    }

    p_wpd->conf.acc_0g.x = 530;
    p_wpd->conf.acc_0g.y = 530;
    p_wpd->conf.acc_0g.z = 530;
    p_wpd->conf.acc_1g.x = 636;
    p_wpd->conf.acc_1g.y = 636;
    p_wpd->conf.acc_1g.z = 636;
    p_wpd->conf.volume = 0;
    p_wpd->conf.motor = 0;

    if (result != WPAD_ERR_NONE) {
        return;
    }

    if (p_wpd->oldFw == FALSE) {
        for (i = 0; i < 2; i++) {
            checksum = 0;
            index = i * 11;
            for (j = index; j < index + 10; j++) {
                checksum += data[j];
            }
            checksum += 0x55;

            if (checksum == data[index + 10]) {
                DEBUGPrint("Dpd Setting is ok.\n");

                obj[0].x = (s16)((s16)(data[index + 0] & 0xFF) | (u16)((u16)((u16)data[index + 2] & 0x30) << 4));
                obj[0].y = (s16)(DPD_MAX_Y - (s16)((s16)(data[index + 1] & 0xFF) | (u16)((u16)((u16)data[index + 2] & 0xC0) << 2)));
                obj[0].size = p_wpd->dpdDummyObjSize;
                obj[0].traceId = 0;

                obj[1].x = (s16)((s16)(data[index + 3] & 0xFF) | (u16)((u16)((u16)data[index + 2] & 0x03) << 8));
                obj[1].y = (s16)(DPD_MAX_Y - (s16)((s16)(data[index + 4] & 0xFF) | (u16)((u16)((u16)data[index + 2] & 0x0C) << 6)));
                obj[1].size = p_wpd->dpdDummyObjSize;
                obj[1].traceId = 1;

                obj[2].x = (s16)((s16)(data[index + 5] & 0xFF) | (u16)((u16)((u16)data[index + 7] & 0x30) << 4));
                obj[2].y = (s16)(DPD_MAX_Y - (s16)((s16)(data[index + 6] & 0xFF) | (u16)((u16)((u16)data[index + 7] & 0xC0) << 2)));
                obj[2].size = p_wpd->dpdDummyObjSize;
                obj[2].traceId = 2;

                obj[3].x = (s16)((s16)(data[index + 8] & 0xFF) | (u16)((u16)((u16)data[index + 7] & 0x03) << 8));
                obj[3].y = (s16)(DPD_MAX_Y - (s16)((s16)(data[index + 9] & 0xFF) | (u16)((u16)((u16)data[index + 7] & 0x0C) << 6)));
                obj[3].size = p_wpd->dpdDummyObjSize;
                obj[3].traceId = 3;

                for (j = 0; j < WPAD_DPD_MAX_OBJECTS; j++) {
                    if (obj[j].x < 0x200 && obj[j].y < 0x180) {
                        p_wpd->conf.obj[0].x = obj[j].x;
                        p_wpd->conf.obj[0].y = obj[j].y;
                        p_wpd->conf.obj[0].size = obj[j].size;
                        p_wpd->conf.obj[0].traceId = obj[j].traceId;
                    }
                    if (obj[j].x > 0x200 && obj[j].y < 0x180) {
                        p_wpd->conf.obj[1].x = obj[j].x;
                        p_wpd->conf.obj[1].y = obj[j].y;
                        p_wpd->conf.obj[1].size = obj[j].size;
                        p_wpd->conf.obj[1].traceId = obj[j].traceId;
                    }
                    if (obj[j].x > 0x200 && obj[j].y > 0x180) {
                        p_wpd->conf.obj[2].x = obj[j].x;
                        p_wpd->conf.obj[2].y = obj[j].y;
                        p_wpd->conf.obj[2].size = obj[j].size;
                        p_wpd->conf.obj[2].traceId = obj[j].traceId;
                    }
                    if (obj[j].x < 0x200 && obj[j].y > 0x180) {
                        p_wpd->conf.obj[3].x = obj[j].x;
                        p_wpd->conf.obj[3].y = obj[j].y;
                        p_wpd->conf.obj[3].size = obj[j].size;
                        p_wpd->conf.obj[3].traceId = obj[j].traceId;
                    }
                }
                break;
            }

            DEBUGPrint("Dpd Setting is broken.\n");
        }
    } else {
        DEBUGPrint("Old Firmware uses default values because it has no Dpd Setting.\n");
    }

    x = y = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        a[i] = p_wpd->conf.obj[i].x;
        b[i] = p_wpd->conf.obj[i].y;
        x += p_wpd->conf.obj[i].x - defaultX[i];
        y += p_wpd->conf.obj[i].y - defaultY[i];
        DEBUGPrint("x = %lf, y = %lf\n", a[i], b[i]);
    }

    x = x / 4.0f;
    y = y / 4.0f;

    dx = (f32)atan((x + 126.5f) / 1332.5f) - (f32)atan(0.09493433684110641);
    dy = (f32)atan((y + 93.0f) / 1337.4f) - (f32)atan(0.06953790783882141);

    _wpadCalibrationX[chan] = (f32)tan(dx) * -1.0f * 1332.5f;
    _wpadCalibrationY[chan] = (f32)tan(dy) * -1.0f * 1337.4f;

    DEBUGPrint("difaveX = %lf, difaveY = %lf\n", x, y);
    DEBUGPrint("deltaX  = %lf, deltaY  = %lf\n", dx, dy);
    DEBUGPrint("_wpadCalibrationX = %lf, _wpadCalibrationY = %lf\n", _wpadCalibrationX[chan], _wpadCalibrationY[chan]);

    _wpadCenterX[chan] = _wpadCenterY[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        a[i] += _wpadCalibrationX[chan];
        b[i] += _wpadCalibrationY[chan];
        _wpadCenterX[chan] += a[i];
        _wpadCenterY[chan] += b[i];
    }

    _wpadCenterX[chan] = _wpadCenterX[chan] / 4.0f;
    _wpadCenterY[chan] = _wpadCenterY[chan] / 4.0f;

    DEBUGPrint("centX  = %lf, centY  = %lf\n", _wpadCenterX[chan], _wpadCenterY[chan]);

    _wpadRolag[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        c[i] = atan((b[i] - _wpadCenterY[chan]) / (a[i] - _wpadCenterX[chan]));
        d[i] = atan((defaultY[i] - 383.5f) / (defaultX[i] - 511.5f));
        _wpadRolag[chan] += c[i] - d[i];
    }

    _wpadRolag[chan] = _wpadRolag[chan] / 4.0f;

    DEBUGPrint("_wpadRolag  = %lf\n", _wpadRolag[chan]);

    for (i = 0; i < 2; i++) {
        checksum = 0;
        index = i * 10;
        if (p_wpd->oldFw == FALSE) {
            index += 22;
        }
        for (j = index; j < index + 9; j++) {
            checksum += data[j];
        }
        checksum += 0x55;

        if (checksum == data[index + 9]) {
            DEBUGPrint("Acc is ok.\n");
            p_wpd->conf.acc_0g.x = MAKE_CONFIG_ACC_X(data[index + 0], data[index + 3]);
            p_wpd->conf.acc_0g.y = MAKE_CONFIG_ACC_Y(data[index + 1], data[index + 3]);
            p_wpd->conf.acc_0g.z = MAKE_CONFIG_ACC_Z(data[index + 2], data[index + 3]);
            p_wpd->conf.acc_1g.x = MAKE_CONFIG_ACC_X(data[index + 4], data[index + 7]);
            p_wpd->conf.acc_1g.y = MAKE_CONFIG_ACC_Y(data[index + 5], data[index + 7]);
            p_wpd->conf.acc_1g.z = MAKE_CONFIG_ACC_Z(data[index + 6], data[index + 7]);
            p_wpd->conf.volume = (u8)(data[index + 8] & 0x7F);
            p_wpd->conf.motor = (u8)(data[index + 8] & 0x80);
            break;
        }

        DEBUGPrint("Acc is broken.\n");
    }

    DEBUGPrint("0G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->conf.acc_0g.x, p_wpd->conf.acc_0g.y, p_wpd->conf.acc_0g.z);
    DEBUGPrint("1G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->conf.acc_1g.x, p_wpd->conf.acc_1g.y, p_wpd->conf.acc_1g.z);
    DEBUGPrint("Volume: %d,   Motor: %d\n", p_wpd->conf.volume, p_wpd->conf.motor);
}



static void __wpadGetExtConfig(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8* data = (u8*)p_wpd->readBufPtr;
    int offset = -1;
    int base;
    int i;
    int j;
    u32 sum;

    switch (result) {
    case WPAD_ERR_NO_CONTROLLER:
        p_wpd->devType = WPAD_DEV_NOT_FOUND;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extDisconnected = FALSE;
        break;
    case WPAD_ERR_NONE:
        p_wpd->devType = _wpadDevType[chan];
        break;
    default:
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extDisconnected = FALSE;
        break;
    }

    p_wpd->devMode = _wpadDevMode[chan];

    if (p_wpd->devType == WPAD_DEV_FREESTYLE || p_wpd->devType == WPAD_DEV_CLASSIC) {
        for (i = 0, base = 0; i < 2; i++, base += 16) {
            sum = 0;
            for (j = base; j < base + 14; j++) {
                sum += data[j];
            }
            if ((u8)(sum + 0x55) == data[base + 14] && (u8)(sum + 0xAA) == data[base + 15]) {
                offset = base;
            }
        }

        switch (p_wpd->devType) {
        case WPAD_DEV_FREESTYLE:
            if (offset < 0) {
                DEBUGPrint("check sum error.\n");
                p_wpd->extConf.fs.acc_0g.x = 512;
                p_wpd->extConf.fs.acc_0g.y = 512;
                p_wpd->extConf.fs.acc_0g.z = 512;
                p_wpd->extConf.fs.acc_1g.x = 716;
                p_wpd->extConf.fs.acc_1g.y = 716;
                p_wpd->extConf.fs.acc_1g.z = 716;
                break;
            }

            p_wpd->extConf.fs.acc_0g.x = MAKE_CONFIG_ACC_X(data[offset + 0], data[offset + 3]);
            p_wpd->extConf.fs.acc_0g.y = MAKE_CONFIG_ACC_Y(data[offset + 1], data[offset + 3]);
            p_wpd->extConf.fs.acc_0g.z = MAKE_CONFIG_ACC_Z(data[offset + 2], data[offset + 3]);
            p_wpd->extConf.fs.acc_1g.x = MAKE_CONFIG_ACC_X(data[offset + 4], data[offset + 7]);
            p_wpd->extConf.fs.acc_1g.y = MAKE_CONFIG_ACC_Y(data[offset + 5], data[offset + 7]);
            p_wpd->extConf.fs.acc_1g.z = MAKE_CONFIG_ACC_Z(data[offset + 6], data[offset + 7]);
            p_wpd->extConf.fs.stick.x_max = (s8)data[offset + 8];
            p_wpd->extConf.fs.stick.x_min = (s8)data[offset + 9];
            p_wpd->extConf.fs.stick.y_max = (s8)data[offset + 11];
            p_wpd->extConf.fs.stick.y_min = (s8)data[offset + 12];
            if (!p_wpd->isSetStickOrigin) {
                p_wpd->extConf.fs.stick.x = (s8)data[offset + 10];
                p_wpd->extConf.fs.stick.y = (s8)data[offset + 13];
            }
            DEBUGPrint("0G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_0g.x, p_wpd->extConf.fs.acc_0g.y,
                       p_wpd->extConf.fs.acc_0g.z);
            DEBUGPrint("1G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_1g.x, p_wpd->extConf.fs.acc_1g.y,
                       p_wpd->extConf.fs.acc_1g.z);
            DEBUGPrint("FS:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.fs.stick.x, p_wpd->extConf.fs.stick.x_max,
                       p_wpd->extConf.fs.stick.x_min);
            DEBUGPrint("FS:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.fs.stick.y, p_wpd->extConf.fs.stick.y_max,
                       p_wpd->extConf.fs.stick.y_min);
            break;
        case WPAD_DEV_CLASSIC:
            p_wpd->extConf.cl.lstk.x_max = (s8)data[offset + 0];
            p_wpd->extConf.cl.lstk.x_min = (s8)data[offset + 1];
            p_wpd->extConf.cl.lstk.y_max = (s8)data[offset + 3];
            p_wpd->extConf.cl.lstk.y_min = (s8)data[offset + 4];
            p_wpd->extConf.cl.rstk.x_max = (s8)data[offset + 6];
            p_wpd->extConf.cl.rstk.x_min = (s8)data[offset + 7];
            p_wpd->extConf.cl.rstk.y_max = (s8)data[offset + 9];
            p_wpd->extConf.cl.rstk.y_min = (s8)data[offset + 10];
            if (!p_wpd->isSetStickOrigin) {
                p_wpd->extConf.cl.lstk.x = (s8)data[offset + 2];
                p_wpd->extConf.cl.lstk.y = (s8)data[offset + 5];
                p_wpd->extConf.cl.rstk.x = (s8)data[offset + 8];
                p_wpd->extConf.cl.rstk.y = (s8)data[offset + 11];
                p_wpd->extConf.cl.triggerL = data[offset + 12];
                p_wpd->extConf.cl.triggerR = data[offset + 13];
            }
            DEBUGPrint("CL:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.lstk.x, p_wpd->extConf.cl.lstk.x_max,
                       p_wpd->extConf.cl.lstk.x_min);
            DEBUGPrint("CL:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.lstk.y, p_wpd->extConf.cl.lstk.y_max,
                       p_wpd->extConf.cl.lstk.y_min);
            DEBUGPrint("CR:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.rstk.x, p_wpd->extConf.cl.rstk.x_max,
                       p_wpd->extConf.cl.rstk.x_min);
            DEBUGPrint("CR:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.rstk.y, p_wpd->extConf.cl.rstk.y_max,
                       p_wpd->extConf.cl.rstk.y_min);
            DEBUGPrint("LR:  L = %d,  R = %d\n", p_wpd->extConf.cl.triggerL, p_wpd->extConf.cl.triggerR);
            break;
        }
    }

    if (p_wpd->devType != WPAD_DEV_BALANCE_CHECKER || p_wpd->readLength != 1) {
        if (p_wpd->resultCallback && p_wpd->resultCallback == (WPADCallback)p_wpd->extensionCallback) {
            p_wpd->resultCallback(chan, p_wpd->devType);
            p_wpd->resultCallback = NULL;
        }
    }
}

static BOOL __wpadIsExtEncMain(u8 devType) {
    if (devType == WPAD_DEV_FREESTYLE || devType == WPAD_DEV_CLASSIC || devType == WPAD_DEV_BALANCE_CHECKER || devType == WPAD_DEV_VSM ||
        devType == WPAD_DEV_GUITAR || devType == WPAD_DEV_DRUM)
    {
        return TRUE;
    }
    return FALSE;
}

static void __wpadGetExtType(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8* data = (u8*)p_wpd->readBufPtr;
    WPADCmdQueue* queue = &p_wpd->extq;

    if (result == WPAD_ERR_NONE) {
        DEBUGPrint("devId: %d, subId: %d\n", data[5], data[0]);

        _wpadCLCompt[chan] = FALSE;
        _wpadDevMode[chan] = data[4];

        switch (data[5]) {
        case 0:
            _wpadDevType[chan] = WPAD_DEV_FREESTYLE;
            break;
        case 1:
            _wpadDevType[chan] = WPAD_DEV_CLASSIC;
            break;
        case 2:
            _wpadDevType[chan] = WUDIsLinkedWBC() ? WPAD_DEV_BALANCE_CHECKER : WPAD_DEV_FUTURE;
            break;
        case 4:
            _wpadDevType[chan] = WPADIsEnabledVSM() ? WPAD_DEV_VSM : WPAD_DEV_FUTURE;
            break;
        case 16:
            _wpadDevType[chan] = WPADIsEnabledTRN() ? WPAD_DEV_TRAIN : WPAD_DEV_FUTURE;
            break;
        case 3:
            if (__OSInIPL) {
                _wpadDevType[chan] = WPAD_DEV_CLASSIC;
                _wpadDevMode[chan] = WPAD_DEV_MODE_CLASSIC_REDUCED;
                _wpadCLCompt[chan] = TRUE;
            } else {
                switch (data[0]) {
                case 0:
                    _wpadDevType[chan] = WPADIsEnabledGTR() ? WPAD_DEV_GUITAR : WPAD_DEV_FUTURE;
                    break;
                case 1:
                    _wpadDevType[chan] = WPADIsEnabledDRM() ? WPAD_DEV_DRUM : WPAD_DEV_FUTURE;
                    break;
                default:
                    _wpadDevType[chan] = WPAD_DEV_FUTURE;
                    break;
                }
            }
            break;
        default:
            _wpadDevType[chan] = WPAD_DEV_FUTURE;
            break;
        }

        if (_wpadDevType[chan] == WPAD_DEV_CLASSIC &&
            (_wpadDevMode[chan] < WPAD_DEV_MODE_CLASSIC_REDUCED || _wpadDevMode[chan] > WPAD_DEV_MODE_CLASSIC_STANDARD))
        {
            _wpadDevType[chan] = WPAD_DEV_NOT_SUPPORTED;
        }

        DEBUGPrint("type : %d\n", _wpadDevType[chan]);
        DEBUGPrint("mode : %d\n", _wpadDevMode[chan]);
        DEBUGPrint("last : %d\n", p_wpd->savedDevType);

        if (p_wpd->extDisconnected && p_wpd->savedDevType == _wpadDevType[chan]) {
            if (p_wpd->isSetStickOrigin) {
                DEBUGPrint("Keep calibration!\n");
                p_wpd->isSetStickOrigin = TRUE;
            }
        } else {
            DEBUGPrint("Recalibration!\n");
            p_wpd->isSetStickOrigin = FALSE;
        }

        p_wpd->savedDevType = _wpadDevType[chan];
        p_wpd->extDisconnected = FALSE;

        if (_wpadDevType[chan] == WPAD_DEV_FUTURE || _wpadDevType[chan] == WPAD_DEV_NOT_SUPPORTED) {
            p_wpd->devType = _wpadDevType[chan];
            p_wpd->devMode = _wpadDevMode[chan];
            if (p_wpd->extensionCallback) {
                p_wpd->extensionCallback(chan, p_wpd->devType);
            }
        } else {
            if (__wpadIsExtEncMain(_wpadDevType[chan])) {
                p_wpd->extEnc = WPAD_EXT_ENC_MAIN;
                WPADiCreateKey(chan);
            } else {
                p_wpd->extEnc = WPAD_EXT_ENC_3RD;
                WPADiCreateKeyFor3rd(chan);
            }

            WPADiSendWriteDataCmd(queue, 0xAA, WPAD_EXT_REG_INIT, __wpadAbortInitExtension);
            WPADiSendWriteData(queue, &p_wpd->key[0], 6, WPAD_EXT_REG_KEY + 0, __wpadAbortInitExtension);
            WPADiSendWriteData(queue, &p_wpd->key[6], 6, WPAD_EXT_REG_KEY + 6, __wpadAbortInitExtension);
            WPADiSendWriteData(queue, &p_wpd->key[12], 4, WPAD_EXT_REG_KEY + 12, __wpadAbortInitExtension);

            if (_wpadDevType[chan] == WPAD_DEV_BALANCE_CHECKER) {
                WPADiSendWriteDataCmd(queue, 0xAA, WPAD_EXT_REG_BLC, __wpadAbortInitExtension);
                WPADiSendWriteDataCmd(queue, 0xAA, WPAD_EXT_REG_BLC, __wpadAbortInitExtension);
                WPADiSendWriteDataCmd(queue, 0xAA, WPAD_EXT_REG_BLC, __wpadAbortInitExtension);
            }

            WPADiSendReadData(queue, p_wpd->readBuf, 32, WPAD_EXT_REG_CONFIG, (WPADCallback)p_wpd->extensionCallback);
        }
    } else {
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        p_wpd->devMode = WPAD_DEV_MODE_NORMAL;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extDisconnected = FALSE;
    }
}

static void __wpadGetGameInfo(s32 chan, s32 result, u8 index) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    int i;
    u8* p_read = (u8*)p_wpd->readBufPtr;
    u8* p_calc = (u8*)p_wpd->readBufPtr;
    u8 sum = 0;

    if (result == WPAD_ERR_NONE) {
        for (i = 0; i < 47; i++) {
            sum += p_calc[i];
        }
        sum += 0x55;

        if (p_read[47] == sum) {
            memcpy(&p_wpd->gameInfo, p_calc, sizeof(WPADMEMGameInfo));
            p_wpd->gameInfoErr[index] = WPAD_ERR_NONE;
        } else {
            p_wpd->gameInfoErr[index] = WPAD_ERR_INVALID;
        }
    } else {
        p_wpd->gameInfoErr[index] = WPAD_ERR_INVALID;
    }
}

static void __wpadInitExtension(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    WPADiClearQueue(&p_wpd->extq);
    WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, __wpadAbortInitExtension);
    p_wpd->extEnc = WPAD_EXT_ENC_INIT;
    WPADiSendWriteDataCmd(&p_wpd->extq, 0x55, WPAD_EXT_REG_INIT, __wpadAbortInitExtension);
    WPADiSendWriteDataCmd(&p_wpd->extq, 0x00, WPAD_EXT_REG_INIT2, __wpadAbortInitExtension);
    WPADiSendReadData(&p_wpd->extq, p_wpd->readBuf, 6, WPAD_EXT_REG_TYPE, __wpadAbortInitExtension);
}

static void __wpadInvertRxBuffer(u8 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    p_wpd->rxBufIndex = (u8)(p_wpd->rxBufIndex ? 0 : 1);
}

static void* __wpadGetWorkRxBuffer(u8 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    void* buf = p_wpd->rxBuf[p_wpd->rxBufIndex];

    return buf;
}

s32 WPADiHIDParser(u8 chan, u8* data) {
    u8* buf;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    s32 ret = 0;

    if (data[0] >= 0x20 && data[0] <= 0x3F) {
        enabled = OSDisableInterrupts();

        buf = __wpadGetWorkRxBuffer(chan);
        if (data[0] != 0x3E && data[0] != 0x3F) {
            memset(buf, 0, WPAD_RX_DATASIZE);
        }

        __a1_input_reports_array[data[0] - 0x20](chan, data, buf);

        if (!p_wpd->setup) {
            ((WPADStatus*)buf)->err = WPAD_ERR_INVALID;
        }

        if (data[0] != 0x3E && data[0] != 0x3F) {
            __wpadInvertRxBuffer(chan);
        }

        OSRestoreInterrupts(enabled);

        WPADiExcludeButton(chan);
        WPADiCopyOut(chan);
    } else {
        ret = -1;
    }

    return ret;
}

static void* __wpadGetLastRxBuffer(u8 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u32 index = p_wpd->rxBufIndex ? 0 : 1;
    return p_wpd->rxBuf[index];
}

static void __wpadRetrieveButton(WPADControlBlock* p_wpd, u8 chan, u8* data, WPADStatus* status) {
    u16 extButton;

    memcpy(status, __wpadGetLastRxBuffer(chan), WPAD_RX_DATASIZE);
    extButton = (u16)(status->button & HID_BUTTON_FS_MASK);
    status->button = MAKE_BUTTON(data[2], data[1]);
    status->button |= extButton;
    if (status->dev != p_wpd->devType) {
        status->dev = p_wpd->devType;
        status->err = WPAD_ERR_INVALID;
    }
}

static void __a1_20_status_report(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    BOOL attached = p_wpd->infos.attach;
    WPADStatus* status = (WPADStatus*)buf;
    WPADExtensionCallback callback;

    DEBUGPrint("Received report 20\n");
    enabled = OSDisableInterrupts();

    if (!p_wpd->setup) {
        OSRestoreInterrupts(enabled);
        return;
    }

    p_wpd->infos.attach = (data[3] & 0x02) >> 1;
    p_wpd->infos.lowBat = data[3] & 0x01;
    p_wpd->infos.led = (u8)((data[3] & 0xF0) >> 4);
    p_wpd->infos.protocol = (u8)((data[5] & 0x0F) >> 4);
    p_wpd->infos.firmware = (u8)((data[5] & 0xF0) >> 0);
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    p_wpd->infos.dpd = (data[3] & 0x08) >> 3;
    p_wpd->infos.speaker = (data[3] & 0x04) >> 2;

    if (p_wpd->devType == WPAD_DEV_BALANCE_CHECKER) {
        p_wpd->infos.battery = p_wpd->blcBattery;
    } else if (data[6] >= 0x55) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_MAX;
    } else if (data[6] >= 0x44) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_HIGH;
    } else if (data[6] >= 0x33) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_MEDIUM;
    } else if (data[6] >= 0x03) {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_LOW;
    } else {
        p_wpd->infos.battery = WPAD_BATTERY_LEVEL_CRITICAL;
    }

    if (p_wpd->infos.attach) {
        if (!attached) {
            DEBUGPrint("initialize attachment\n");
            _wpadExtInitRetryCnt[chan] = 0;
            callback = p_wpd->extensionCallback;
            __wpadInitExtension(chan);
            p_wpd->devType = WPAD_DEV_INITIALIZING;
            p_wpd->devMode = WPAD_DEV_MODE_NORMAL;
            if (callback) {
                callback(chan, WPAD_DEV_INITIALIZING);
            }
        }
    } else {
        p_wpd->devType = WPAD_DEV_CORE;
        p_wpd->devMode = WPAD_DEV_MODE_NORMAL;
        WPADiClearQueue(&p_wpd->extq);
        WPADiSendSetReportType(&p_wpd->extq, p_wpd->dataFormat, p_wpd->pwrSave, NULL);
        if (attached) {
            p_wpd->extDisconnected = TRUE;
            p_wpd->extReconnectWait = 300;
            if (p_wpd->extensionCallback) {
                p_wpd->extensionCallback(chan, WPAD_DEV_CORE);
            }
        }
    }

    if (p_wpd->infoBuf) {
        memcpy(p_wpd->infoBuf, &p_wpd->infos, sizeof(WPADInfo));
        p_wpd->infoBuf = NULL;
    }

    __wpadRetrieveButton(p_wpd, chan, data, status);

    if (p_wpd->resultCallback && p_wpd->getStatFlag) {
        p_wpd->resultCallback(chan, WPAD_ERR_NONE);
        p_wpd->resultCallback = NULL;
    }

    p_wpd->getStatFlag = FALSE;
    OSRestoreInterrupts(enabled);
}

static void __a1_21_user_data(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u16 addr;
    u16 readAddrLow;
    u16 readAddrHigh;
    s16 offset;
    u8 size;
    s32 result;
    WPADStatus* status = (WPADStatus*)buf;

    if (data[3] & 0x0F) {
        DEBUGPrint("read error happens!\n");
        p_wpd->readError = -1;
        if (p_wpd->resultCallback) {
            if (!p_wpd->extensionCallback || p_wpd->extensionCallback != (WPADExtensionCallback)p_wpd->resultCallback) {
                p_wpd->resultCallback(chan, WPAD_ERR_TRANSFER);
            }
            p_wpd->resultCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_NONE;
    }

    size = (u8)((data[3] >> 4) + 1);
    addr = (u16)((u16)((u16)data[4] << 8) & 0xFF00) | (u16)((u16)data[5] & 0x00FF);
    readAddrLow = (u16)(p_wpd->readBaseAddr & 0xFFFF);
    readAddrHigh = (u16)((p_wpd->readBaseAddr & 0xFFFF0000) >> 16);
    offset = (s16)(addr - readAddrLow);

    if (addr >= readAddrLow && addr <= readAddrLow + p_wpd->readLength) {
        memcpy((u8*)p_wpd->readBufPtr + offset, &data[6], size);

        if (readAddrLow + p_wpd->readLength == addr + size) {
            DEBUGPrint("base addr: %08x\n", p_wpd->readBaseAddr);
            DEBUGPrint("length   : %d\n", p_wpd->readLength);
            result = p_wpd->readError < 0 ? WPAD_ERR_TRANSFER : WPAD_ERR_NONE;
            DEBUGPrint("i2c = %04x\n", readAddrHigh);
            DEBUGPrint("enc = %d\n", p_wpd->extEnc);

            if (readAddrHigh == (WPAD_EXT_REG_ADDR >> 16)) {
                DEBUGPrint("Access to extension register.\n");
                if (p_wpd->extEnc == WPAD_EXT_ENC_MAIN || p_wpd->extEnc == WPAD_EXT_ENC_3RD) {
                    DEBUGPrint("Decode!!!!\n");
                    DEBUGPrint("    len = %d, addr = %04x\n", size, readAddrLow);
                    WPADiDecode(chan, p_wpd->readBufPtr, size, readAddrLow);
                }
            }

            if ((p_wpd->readBaseAddr == 0 && p_wpd->oldFw == FALSE) || (p_wpd->readBaseAddr == 0x176C && p_wpd->oldFw == TRUE)) {
                __wpadGetDevConfig(chan, result);
            }

            if (p_wpd->readBaseAddr == WPAD_EXT_REG_CONFIG) {
                __wpadGetExtConfig(chan, result);
            }

            if (p_wpd->readBaseAddr == WPAD_EXT_REG_TYPE) {
                __wpadGetExtType(chan, result);
            }

            if (p_wpd->readBaseAddr == 0x2A) {
                __wpadGetGameInfo(chan, result, 0);
            }

            if (p_wpd->readBaseAddr == 0x62) {
                __wpadGetGameInfo(chan, result, 1);
            }

            if (p_wpd->resultCallback) {
                p_wpd->resultCallback(chan, result);
                p_wpd->resultCallback = NULL;
            }

            p_wpd->status = WPAD_ERR_NONE;
        }
    } else {
        DEBUGPrint("received data is out of range!\n");
    }

    __wpadRetrieveButton(p_wpd, chan, data, status);
    OSRestoreInterrupts(enabled);
}

static void __a1_22_ack(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    WPADStatus* status = (WPADStatus*)buf;
    u8 id;
    u8 err;
    s32 result;

    DEBUGPrint("Received ack!\n");
    __wpadRetrieveButton(p_wpd, chan, data, status);

    id = data[3];
    err = data[4];
    result = err == 0 ? WPAD_ERR_NONE : WPAD_ERR_TRANSFER;
    DEBUGPrint("ack --> report ID = %02x, error code = %d\n", id, err);

    if (p_wpd->cmdId == id) {
        if (p_wpd->resultCallback) {
            p_wpd->resultCallback(chan, result);
            p_wpd->resultCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_NONE;
    } else {
        DEBUGPrint("invalid ack!\n");
    }

    OSRestoreInterrupts(enabled);
}

static void __parse_dpd_data(s32 chan, WPADStatus** status, u8 format, u8* data, u8 len) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 i;
    u8 a;
    u8 b;
    u8 c;
    f32 fx;
    f32 fy;
    DPDObject* p_obj;

    if (p_wpd->dpdBusy) {
        status[0]->err = WPAD_ERR_INVALID;
        return;
    }

    if (format == WPAD_DPD_STANDARD) {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
            if (i * 3 + 2 < len) {
                a = ((u8*)(data + i * 3))[0];
                b = ((u8*)(data + i * 3))[1];
                c = ((u8*)(data + i * 3))[2];
                status[0]->obj[i].x = (s16)((s16)(a & 0xFF) | (u16)(((u16)c & 0x30) << 4));
                status[0]->obj[i].y = (s16)(DPD_MAX_Y - (s16)((s16)(b & 0xFF) | (u16)(((u16)c & 0xC0) << 2)));
                status[0]->obj[i].size = (u16)(c & 0x0F);
                status[0]->obj[i].size = (u8)((f32)status[0]->obj[i].size * (f32)status[0]->obj[i].size * 3.141592f);
                if (status[0]->obj[i].size == 0 || status[0]->obj[i].x == DPD_MAX_X || status[0]->obj[i].y == DPD_MAX_Y) {
                    status[0]->obj[i].x = 0;
                    status[0]->obj[i].y = DPD_MAX_Y;
                    status[0]->obj[i].size = 0;
                }
                status[0]->obj[i].traceId = i;
            } else {
                status[0]->obj[i].x = 0;
                status[0]->obj[i].y = DPD_MAX_Y;
                status[0]->obj[i].size = 0;
                status[0]->obj[i].traceId = i;
            }
        }
    } else if (format == WPAD_DPD_BASIC) {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
            a = ((u8*)((u32)data + i * 3 - i / 2))[0];
            b = ((u8*)((u32)data + i * 3 - i / 2))[1];
            c = ((u8*)((u32)data + 5 * (i / 2)))[2];
            if (i % 2 == 0) {
                status[0]->obj[i].x = (s16)((s16)(a & 0xFF) | (u16)((u16)((u16)c & 0x30) << 4));
                status[0]->obj[i].y = (s16)(DPD_MAX_Y - (s16)((s16)(b & 0xFF) | (u16)((u16)((u16)c & 0xC0) << 2)));
            } else {
                status[0]->obj[i].x = (s16)((s16)(a & 0xFF) | (u16)((u16)((u16)c & 0x03) << 8));
                status[0]->obj[i].y = (s16)(DPD_MAX_Y - (s16)((s16)(b & 0xFF) | (u16)((u16)((u16)c & 0x0C) << 6)));
            }
            if (status[0]->obj[i].x == DPD_MAX_X || status[0]->obj[i].y == DPD_MAX_Y) {
                status[0]->obj[i].x = 0;
                status[0]->obj[i].y = DPD_MAX_Y;
                status[0]->obj[i].size = 0;
            } else {
                status[0]->obj[i].size = 12;
            }
            status[0]->obj[i].traceId = i;
        }
    }

    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++) {
        if (status[0]->obj[i].x != 0 || status[0]->obj[i].y != DPD_MAX_Y) {
            fx = (f32)status[0]->obj[i].x + _wpadCalibrationX[chan] - _wpadCenterX[chan];
            fy = (f32)status[0]->obj[i].y + _wpadCalibrationY[chan] - _wpadCenterY[chan];
            p_obj = &status[0]->obj[i];
            p_obj->x =
                (s16)(_wpadCenterX[chan] + (fx * (f32)cos(_wpadRolag[chan] * -1.0f) - fy * (f32)sin(_wpadRolag[chan] * -1.0f)));
            status[0]->obj[i].y =
                _wpadCenterY[chan] + (fx * (f32)sin(_wpadRolag[chan] * -1.0f) + fy * (f32)cos(_wpadRolag[chan] * -1.0f));
        }
    }
}

static void __parse_dpdex_data(s32 chan, WPADStatusEx** status, u8 index, u8* data, u8 len) {
    (*status)->obj[index].x = (s16)((s16)(data[0] & 0xFF) | (u16)((u16)(data[2] & 0x30) << 4));
    (*status)->obj[index].y = (s16)(DPD_MAX_Y - (s16)((s16)(data[1] & 0xFF) | (u16)((u16)(data[2] & 0xC0) << 2)));
    (*status)->exp[index].pixel = (u16)((s16)((s16)(((u16)data[7] << 8) & 0xFF00) | (u16)(data[8] & 0xFF)) << 6);
    (*status)->exp[index].radius = (s8)(data[2] & 0x0F);
    (*status)->exp[index].range_x1 = (s16)((s8)data[3] == -1 ? 0 : data[3]);
    (*status)->exp[index].range_y1 = (s16)((s8)data[4] == -1 ? 0 : data[4]);
    (*status)->exp[index].range_x2 = (s16)((s8)data[5] == -1 ? 0 : data[5]);
    (*status)->exp[index].range_y2 = (s16)((s8)data[6] == -1 ? 0 : data[6]);
    (*status)->exp[index].range_x1 = (s16)((*status)->exp[index].range_x1 << 3);
    (*status)->exp[index].range_y1 = (s16)(DPD_MAX_Y - (s16)((*status)->exp[index].range_y1 << 3));
    (*status)->exp[index].range_x2 = (s16)((*status)->exp[index].range_x2 << 3);
    (*status)->exp[index].range_y2 = (s16)(DPD_MAX_Y - (s16)((*status)->exp[index].range_y2 << 3));
    (*status)->obj[index].size = (u16)((f32)(*status)->exp[index].radius * (f32)(*status)->exp[index].radius * 3.141592f);
    if ((*status)->obj[index].size == 0 || (*status)->obj[index].x == DPD_MAX_X || (*status)->obj[index].y == DPD_MAX_Y ||
        (*status)->exp[index].radius == 15)
    {
        (*status)->obj[index].x = 0;
        (*status)->obj[index].y = DPD_MAX_Y;
        (*status)->obj[index].size = 0;
        (*status)->exp[index].pixel = 0;
        (*status)->exp[index].radius = 0;
    }
    (*status)->obj[index].traceId = index;
}

static void __parse_acc_data(s32 chan, WPADStatus** status, u8 format, u8* data, u8 len) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    switch (format) {
    case 0:
        (*status)->accX = (s16)((s16)((s16)((s16)data[2] << 2) & ~3) | (s16)((s16)(u16)(data[0] >> 5) & 3)) - p_wpd->conf.acc_0g.x;
        (*status)->accY = (s16)((s16)((s16)((s16)data[3] << 2) & ~3) | (s16)((s16)(u16)(data[1] >> 4) & 2)) - p_wpd->conf.acc_0g.y;
        (*status)->accZ = (s16)((s16)((s16)((s16)data[4] << 2) & ~3) | (s16)((s16)(u16)(data[1] >> 5) & 2)) - p_wpd->conf.acc_0g.z;
        break;
    case 1:
        (*status)->accX = (s16)((s16)((s16)((s16)data[2] << 2) & ~3) | (s16)((s16)(u16)(data[0] >> 6) & 2)) - p_wpd->conf.acc_0g.x;
        (*status)->accZ |= (s16)((s16)((s16)((s16)data[1] << 3) & ~0xFF) | (s16)((s16)(u16)(data[0] << 1) & 0xC0));
        break;
    case 2:
        (*status)->accY = (s16)((s16)((s16)((s16)data[2] << 2) & ~3) | (s16)((s16)(u16)(data[0] >> 6) & 2)) - p_wpd->conf.acc_0g.y;
        (*status)->accZ |= (s16)((s16)((s16)((s16)data[1] >> 1) & 0x30) | (s16)((s16)(u16)(data[0] >> 3) & 0x0C));
        break;
    }
}

static s16 __clamp(s16 x, s16 min, s16 max) {
    s16 result = x;

    if (x < min) {
        result = min;
    }
    if (x > max) {
        result = max;
    }
    return result;
}

static void __parse_fs_data(s32 chan, WPADFSStatus** status, u8 format, u8* data, u8 len) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    (*status)->fsStickX = (s8)data[0];
    (*status)->fsStickY = (s8)data[1];
    (*status)->fsAccX = (s16)((s16)((s16)((s16)data[2] << 2) & ~3) | (s16)((s16)(data[5] >> 2) & 3)) - p_wpd->extConf.fs.acc_0g.x;
    (*status)->fsAccY = (s16)((s16)((s16)((s16)data[3] << 2) & ~3) | (s16)((s16)(data[5] >> 4) & 3)) - p_wpd->extConf.fs.acc_0g.y;
    (*status)->fsAccZ = (s16)((s16)((s16)((s16)data[4] << 2) & ~3) | (s16)((s16)(data[5] >> 6) & 3)) - p_wpd->extConf.fs.acc_0g.z;
    (*status)->button |= (u16)(((u16)~data[5] << 13) & HID_BUTTON_FS_MASK);

    if (!p_wpd->isSetStickOrigin) {
        p_wpd->isSetStickOrigin = TRUE;
        p_wpd->extConf.fs.stick.x = (*status)->fsStickX;
        p_wpd->extConf.fs.stick.y = (*status)->fsStickY;
    }

    (*status)->fsStickX = (s8)__clamp((u8)(*status)->fsStickX - (u8)p_wpd->extConf.fs.stick.x, -128, 127);
    (*status)->fsStickY = (s8)__clamp((u8)(*status)->fsStickY - (u8)p_wpd->extConf.fs.stick.y, -128, 127);
}

static void __parse_cl_data(s32 chan, WPADCLStatus** status, u8 format, u8* data, u8 len) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    switch (format) {
    case WPAD_DEV_MODE_CLASSIC_EXTENDED:
        (*status)->clLStickX = (s16)((s16)((s16)data[0] << 2) & ~3) | (s16)((s16)data[4] & 3);
        (*status)->clRStickX = (s16)((s16)((s16)data[1] << 2) & ~3) | (s16)((s16)((s16)data[4] >> 2) & 3);
        (*status)->clLStickY = (s16)((s16)((s16)data[2] << 2) & ~3) | (s16)((s16)((s16)data[4] >> 4) & 3);
        (*status)->clRStickY = (s16)((s16)((s16)data[3] << 2) & ~3) | (s16)((s16)((s16)data[4] >> 6) & 3);
        (*status)->clTriggerL = data[5];
        (*status)->clTriggerR = (u8)(len < 9 ? 0 : data[6]);
        (*status)->clButton = (u16)(len < 9 ? 0 : (u16)((u16)(((u16)((u16)data[7] << 8) & 0xFF00) | (u8)data[8]) ^ 0xFFFF));
        break;
    case WPAD_DEV_MODE_CLASSIC_STANDARD:
        (*status)->clLStickX = (s16)((s16)(data[0] & 0xFF) << 2);
        (*status)->clRStickX = (s16)((s16)(data[1] & 0xFF) << 2);
        (*status)->clLStickY = (s16)((s16)(data[2] & 0xFF) << 2);
        (*status)->clRStickY = (s16)((s16)(data[3] & 0xFF) << 2);
        (*status)->clTriggerL = data[4];
        (*status)->clTriggerR = data[5];
        (*status)->clButton = (u16)(len < 8 ? 0 : (u16)((u16)(((u16)((u16)data[6] << 8) & 0xFF00) | (u8)data[7]) ^ 0xFFFF));
        break;
    default:
        (*status)->clLStickX = (s16)((s16)(data[0] & 0x3F) << 4);
        (*status)->clLStickY = (s16)((s16)(data[1] & 0x3F) << 4);
        (*status)->clRStickX = (s16)((s16)((s16)((s16)((s16)data[0] >> 3) & 0x18) | (s16)((s16)((s16)data[1] >> 5) & 0x06) |
                                          (s16)((s16)((s16)data[2] >> 7) & 0x01))
                                      << 5);
        (*status)->clRStickY = (s16)((s16)(data[2] & 0x1F) << 5);
        (*status)->clTriggerL = (u8)((u8)((u8)((u8)(data[2] >> 2) & 0x18) | (u8)((u8)(data[3] >> 5) & 0x07)) << 3);
        (*status)->clTriggerR = (u8)((u8)(data[3] & 0x1F) << 3);
        (*status)->clButton = (u16)((u16)(((u16)((u16)data[4] << 8) & 0xFF00) | (u8)data[5]) ^ 0xFFFF);
        break;
    }

    if (p_wpd->devType == WPAD_DEV_CLASSIC) {
        (*status)->clLStickX -= 512;
        (*status)->clLStickY -= 512;
        (*status)->clRStickX -= 512;
        (*status)->clRStickY -= 512;
    } else {
        (*status)->clLStickX -= 512;
        (*status)->clLStickY -= 512;
    }

    if (!p_wpd->isSetStickOrigin) {
        p_wpd->isSetStickOrigin = TRUE;
        p_wpd->extConf.cl.lstk.x = (*status)->clLStickX;
        p_wpd->extConf.cl.lstk.y = (*status)->clLStickY;
        if (p_wpd->devType == WPAD_DEV_CLASSIC) {
            p_wpd->extConf.cl.rstk.x = (*status)->clRStickX;
            p_wpd->extConf.cl.rstk.y = (*status)->clRStickY;
            p_wpd->extConf.cl.triggerL = (*status)->clTriggerL;
            p_wpd->extConf.cl.triggerR = (*status)->clTriggerR;
        } else {
            p_wpd->extConf.cl.rstk.x = 0;
            p_wpd->extConf.cl.rstk.y = 0;
            p_wpd->extConf.cl.triggerL = 0;
            p_wpd->extConf.cl.triggerR = 0;
        }
    }

    (*status)->clLStickX = __clamp((*status)->clLStickX - p_wpd->extConf.cl.lstk.x, -512, 511);
    (*status)->clLStickY = __clamp((*status)->clLStickY - p_wpd->extConf.cl.lstk.y, -512, 511);

    if (p_wpd->devType == WPAD_DEV_CLASSIC) {
        (*status)->clRStickX = __clamp((*status)->clRStickX - p_wpd->extConf.cl.rstk.x, -512, 511);
        (*status)->clRStickY = __clamp((*status)->clRStickY - p_wpd->extConf.cl.rstk.y, -512, 511);
        (*status)->clTriggerL = (u8)__clamp((*status)->clTriggerL - p_wpd->extConf.cl.triggerL, 0, 255);
        (*status)->clTriggerR = (u8)__clamp((*status)->clTriggerR - p_wpd->extConf.cl.triggerR, 0, 255);
    }

    if (_wpadCLCompt[chan]) {
        (*status)->clRStickX = 0;
        (*status)->clRStickY = 0;
        (*status)->clTriggerL = 0;
        (*status)->clTriggerR = 0;
    }
}

static void __parse_tr_data(s32 chan, WPADTRStatus** status, u8 format, u8* data, u8 len) {
    WPADTRStatus* p_status = *status;

    p_status->brake = data[2];
    p_status->mascon = data[3];
    p_status->trButton = (u16)((u16)(((u16)((u16)data[6] << 8) & 0xFF00) | (u8)data[7]) ^ 0xFFFF);
}

static void __parse_vs_data(s32 chan, WPADVSStatus** status, u8 format, u8* data, u8 len) {
    (*status)->prevSampleCount = data[0];
    (*status)->prevSamples[0] = (u16)((u16)((u16)data[1] << 2) & 0xFFFC) | (u16)((u16)(data[6] >> 6) & 3);
    (*status)->prevSamples[1] = (u16)((u16)((u16)data[2] << 2) & 0xFFFC) | (u16)((u16)(data[6] >> 4) & 3);
    (*status)->prevSamples[2] = (u16)((u16)((u16)data[3] << 2) & 0xFFFC) | (u16)((u16)(data[6] >> 2) & 3);
    (*status)->prevSamples[3] = (u16)((u16)((u16)data[4] << 2) & 0xFFFC) | (u16)((u16)(data[6] >> 0) & 3);
    (*status)->prevSamples[4] = (u16)((u16)((u16)data[5] << 2) & 0xFFFC) | (u16)((u16)(data[7] >> 6) & 3);
    (*status)->sampleCount = data[8];
    (*status)->samples[0] = (u16)((u16)((u16)data[9] << 2) & 0xFFFC) | (u16)((u16)(data[14] >> 6) & 3);
    (*status)->samples[1] = (u16)((u16)((u16)data[10] << 2) & 0xFFFC) | (u16)((u16)(data[14] >> 4) & 3);
    (*status)->samples[2] = (u16)((u16)((u16)data[11] << 2) & 0xFFFC) | (u16)((u16)(data[14] >> 2) & 3);
    (*status)->samples[3] = (u16)((u16)((u16)data[12] << 2) & 0xFFFC) | (u16)((u16)(data[14] >> 0) & 3);
    (*status)->samples[4] = (u16)((u16)((u16)data[13] << 2) & 0xFFFC) | (u16)((u16)(data[15] >> 6) & 3);
    (*status)->baseline = (u16)((u16)((u16)((u16)data[7] << 4) & 0xF3F0)) | (u16)((u16)(data[15] >> 2) & 0x000F);
    (*status)->state = (u8)(data[15] & 3);

    if (len < 21) {
        (*status)->level = 0;
        (*status)->auxData[0] = 0;
        (*status)->auxData[1] = 0;
        (*status)->auxData[2] = 0;
    } else {
        (*status)->level = (u16)((u16)((u16)data[16] << 2) & 0xFFFC) | (u16)((u16)(data[17] >> 6) & 3);
        (*status)->auxData[0] = (u8)(data[18] & 7);
        (*status)->auxData[1] = data[19];
        (*status)->auxData[2] = data[20];
    }
}

static void __a1_30_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    status->dev = p_wpd->devType;
    if (p_wpd->dataFormat == WPAD_FMT_CORE) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
}

static void __a1_31_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    status->dev = p_wpd->devType;
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    __parse_acc_data(chan, &status, 0, &data[1], 0);
}

static void __a1_32_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_FREESTYLE || p_wpd->dataFormat == WPAD_FMT_CLASSIC ||
        p_wpd->dataFormat == WPAD_FMT_TRAIN)
    {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);

    memcpy(_wpadExtData, &data[3], 8);
    WPADiDecode(chan, &data[3], 8, 0);

    if (p_wpd->infos.attach) {
        if (p_wpd->devType == WPAD_DEV_FREESTYLE) {
            __parse_fs_data(chan, (WPADFSStatus**)&buf, p_wpd->devMode, &data[3], 8);
        } else if (p_wpd->devType == WPAD_DEV_CLASSIC) {
            __parse_cl_data(chan, (WPADCLStatus**)&buf, p_wpd->devMode, &data[3], 8);
        } else if (p_wpd->devType == WPAD_DEV_TRAIN) {
            __parse_tr_data(chan, (WPADTRStatus**)&buf, p_wpd->devMode, &data[3], 8);
        }

        if (memcmp(_wpadExtData, _cExtInvalidData, 8) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_33_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    status->dev = p_wpd->devType;
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC || p_wpd->dataFormat == WPAD_FMT_CORE_ACC_DPD) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    __parse_acc_data(chan, &status, 0, &data[1], 0);
    __parse_dpd_data(chan, &status, p_wpd->dpdFormat, &data[6], 12);
}

static void __a1_34_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADBLStatus* status = (WPADBLStatus*)buf;
    int battery;

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_BALANCE_CHECKER) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);

    memcpy(_wpadExtData, &data[3], 19);
    WPADiDecode(chan, &data[3], 19, 0);

    if (p_wpd->infos.attach) {
        if (p_wpd->devType == WPAD_DEV_BALANCE_CHECKER) {
            status->press[0] = (u16)((u16)(data[3] << 8) & 0xFF00) | (u16)(data[4] & 0x00FF);
            status->press[1] = (u16)((u16)(data[5] << 8) & 0xFF00) | (u16)(data[6] & 0x00FF);
            status->press[2] = (u16)((u16)(data[7] << 8) & 0xFF00) | (u16)(data[8] & 0x00FF);
            status->press[3] = (u16)((u16)(data[9] << 8) & 0xFF00) | (u16)(data[10] & 0x00FF);
            status->temp = (s8)data[11];
            status->battery = data[13];

            battery = status->battery << 1;
            if (battery >= 260) {
                p_wpd->blcBattery = 4;
            } else if (battery < 260 && battery >= 250) {
                p_wpd->blcBattery = 3;
            } else if (battery < 250 && battery >= 240) {
                p_wpd->blcBattery = 2;
            } else if (battery < 240 && battery >= 212) {
                p_wpd->blcBattery = 1;
            } else {
                p_wpd->blcBattery = 0;
            }

            if (!p_wpd->isSetStickOrigin) {
                p_wpd->isSetStickOrigin = TRUE;
            }
        }

        if (memcmp(_wpadExtData, _cExtInvalidData, 19) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_35_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC || p_wpd->dataFormat == WPAD_FMT_FREESTYLE ||
        p_wpd->dataFormat == WPAD_FMT_FREESTYLE_ACC || p_wpd->dataFormat == WPAD_FMT_CLASSIC || p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC ||
        p_wpd->dataFormat == WPAD_FMT_VSM)
    {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    __parse_acc_data(chan, &status, 0, &data[1], 0);

    memcpy(_wpadExtData, &data[6], 16);
    WPADiDecode(chan, &data[6], 16, 0);

    if (p_wpd->infos.attach) {
        if (p_wpd->devType == WPAD_DEV_FREESTYLE) {
            __parse_fs_data(chan, (WPADFSStatus**)&buf, p_wpd->devMode, &data[6], 16);
        } else if (p_wpd->devType == WPAD_DEV_CLASSIC) {
            __parse_cl_data(chan, (WPADCLStatus**)&buf, p_wpd->devMode, &data[6], 16);
        } else if (p_wpd->devType == WPAD_DEV_VSM) {
            __parse_vs_data(chan, (WPADVSStatus**)&buf, (u8)p_wpd->dataFormat, &data[6], 16);
        }

        if (memcmp(_wpadExtData, _cExtInvalidData, 16) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_36_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    status->err = WPAD_ERR_INVALID;
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    __parse_dpd_data(chan, &status, p_wpd->dpdFormat, &data[3], 10);

    memcpy(_wpadExtData, &data[13], 9);
    WPADiDecode(chan, &data[13], 9, 0);

    if (p_wpd->infos.attach) {
        if (memcmp(_wpadExtData, _cExtInvalidData, 9) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_37_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC || p_wpd->dataFormat == WPAD_FMT_CORE_ACC_DPD ||
        p_wpd->dataFormat == WPAD_FMT_FREESTYLE || p_wpd->dataFormat == WPAD_FMT_FREESTYLE_ACC ||
        p_wpd->dataFormat == WPAD_FMT_FREESTYLE_ACC_DPD || p_wpd->dataFormat == WPAD_FMT_CLASSIC ||
        p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC || p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC_DPD ||
        p_wpd->dataFormat == WPAD_FMT_GUITAR || p_wpd->dataFormat == WPAD_FMT_DRUM)
    {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = (u8)((data[1] & 0x80) >> 7);
    __parse_acc_data(chan, &status, 0, &data[1], 0);
    __parse_dpd_data(chan, &status, p_wpd->dpdFormat, &data[6], 10);

    memcpy(_wpadExtData, &data[16], 6);
    WPADiDecode(chan, &data[16], 6, 0);

    if (p_wpd->infos.attach) {
        if (p_wpd->devType == WPAD_DEV_FREESTYLE) {
            __parse_fs_data(chan, (WPADFSStatus**)&buf, p_wpd->devMode, &data[16], 6);
        } else if (p_wpd->devType == WPAD_DEV_CLASSIC || p_wpd->devType == WPAD_DEV_GUITAR || p_wpd->devType == WPAD_DEV_DRUM) {
            __parse_cl_data(chan, (WPADCLStatus**)&buf, p_wpd->devMode, &data[16], 6);
        }

        if (memcmp(_wpadExtData, _cExtInvalidData, 6) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_3d_data_type(u8 chan, u8* data, void* buf) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADStatus* status = (WPADStatus*)buf;

    if (p_wpd->dataFormat == WPAD_FMT_VSM_EXTENDED) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;

    memcpy(_wpadExtData, &data[1], 21);
    WPADiDecode(chan, &data[1], 21, 0);

    if (p_wpd->infos.attach) {
        if (p_wpd->devType == WPAD_DEV_VSM) {
            __parse_vs_data(chan, (WPADVSStatus**)&buf, (u8)p_wpd->dataFormat, &data[1], 21);
        }

        if (memcmp(_wpadExtData, _cExtInvalidData, 21) == 0 && status->err == WPAD_ERR_NONE) {
            status->err = WPAD_ERR_CORRUPTED;
        }
    }
}

static void __a1_3e_data_type(u8 chan, u8* data, void* buf) {
    WPADStatus* status = (WPADStatus*)buf;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0) {
        memset(status, 0, WPAD_RX_DATASIZE);
    }

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC || p_wpd->dataFormat == WPAD_FMT_CORE_ACC_DPD_FULL) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = FALSE;
    __parse_acc_data(chan, &status, 1, &data[1], 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&buf, 0, &data[4], 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&buf, 1, &data[13], 0);

    enabled = OSDisableInterrupts();
    _recv_3e[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan]) {
        status->accZ -= p_wpd->conf.acc_0g.z;
        p_wpd->rxBufIndex = (u8)(p_wpd->rxBufIndex ? 0 : 1);
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(enabled);
}

static void __a1_3f_data_type(u8 chan, u8* data, void* buf) {
    WPADStatus* status = (WPADStatus*)buf;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0) {
        memset(status, 0, WPAD_RX_DATASIZE);
    }

    status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_wpd->dataFormat == WPAD_FMT_CORE || p_wpd->dataFormat == WPAD_FMT_CORE_ACC || p_wpd->dataFormat == WPAD_FMT_CORE_ACC_DPD_FULL) {
        status->err = WPAD_ERR_NONE;
    } else {
        status->err = WPAD_ERR_INVALID;
    }
    status->dev = p_wpd->devType;
    p_wpd->infos.nearempty = FALSE;
    __parse_acc_data(chan, &status, 2, &data[1], 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&buf, 2, &data[4], 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&buf, 3, &data[13], 0);

    enabled = OSDisableInterrupts();
    _recv_3f[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan]) {
        status->accZ -= p_wpd->conf.acc_0g.z;
        p_wpd->rxBufIndex = (u8)(p_wpd->rxBufIndex ? 0 : 1);
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(enabled);
}

static void __a1_unused_report(u8 chan, u8* data, void* buf) {
#pragma unused(chan, data, buf)
}
