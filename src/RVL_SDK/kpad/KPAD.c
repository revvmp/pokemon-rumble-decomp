#include <revolution/os.h>
#include <revolution/gx.h>
#include <string.h>
#include "revolution/kpad.h"
#include "revolution/mtx.h"
#include <math.h>
#include <stdio.h>

static const char* __KPADVersion = "<< RVL_SDK - KPAD 	release build: Jun 22 2009 18:32:13 (0x4302_145) >>";

static Vec2 icenter_org = {0.000f, 0.000f};
static f32 idist_org = 1.000f;
static Vec2 iaccXY_nrm_hori = {0.000f, -1.000f};
static Vec2 isec_nrm_hori = {1.000f, 0.000f};
f32 kp_obj_interval = 0.200f;

f32 kp_acc_horizon_pw = 0.050f;
f32 kp_ah_circle_radius = 0.070f;
f32 kp_ah_circle_pw = 0.060f;
u16 kp_ah_circle_ct = 100;
BOOL kp_stick_clamp_cross = FALSE;

f32 kp_err_outside_frame = 0.050f;
f32 kp_err_dist_min;
f32 kp_err_dist_max = 3.000f;
f32 kp_err_dist_speed = 0.040f;
f32 kp_err_first_inpr = 0.900f;
f32 kp_err_next_inpr = 0.900f;
f32 kp_err_acc_inpr = 0.900f;
f32 kp_err_up_inpr = 0.700f;
f32 kp_err_near_pos = 0.100f;

static Vec2 kobj_frame_min, kobj_frame_max;
static f32 kp_err_dist_speed_1;
static f32 kp_err_dist_speedM_1;
static f32 kp_ah_circle_radius2;
static f32 kp_dist_vv1;

static s32 kp_fs_fstick_min = 15;
static s32 kp_fs_fstick_max = 71;
static s32 kp_cl_stick_min = 60;
static s32 kp_cl_stick_max = 308;
static s32 kp_cl_trigger_min = 30;
static s32 kp_cl_trigger_max = 180;
static s32 kp_gc_mstick_min = 15;
static s32 kp_gc_mstick_max = 77;
static s32 kp_gc_cstick_min = 15;
static s32 kp_gc_cstick_max = 64;
static s32 kp_gc_trigger_min = 30;
static s32 kp_gc_trigger_max = 180;
static f32 kp_rm_acc_max = 3.4f;
static f32 kp_fs_acc_max = 2.1f;
static s32 kp_tr_trigger_max = 256;
static s32 kp_gtr_stick_max = 1024;
static u8 sBalanceSettlingSamples = 50;
static f32 sBalanceAverageWindow = 400.0f;
static f32 kp_fs_revise_deg = 24.0f;

KPADInsideStatus inside_kpads[4];
static s32 kp_tr_trigger_min = 0;
static s32 kp_gtr_stick_min = 0;
static u8 sKpadInitialized = FALSE;
static Vec2 Vec2_0 = {0.0f, 0.0f};
static Mtx kp_fs_rot;
static f64 sBalanceSampleAverage[4];
static f64 sBalanceSmoothedWeights[4];
static f64 sBalanceTgcWeight = 0.0;
static u16 sBalanceSampleCount = 0;
static u8 sBalanceCalibrationState = 0;
static u8 sBalanceTgcCalibrating = FALSE;
static u8 sBalanceCalibrationResult = 0;
static u8 sBalanceControlSucceeded = FALSE;
static u8 sBalanceCommandPending = FALSE;

static u8 kp_mode_table[][2] = {
    {0, WPAD_FMT_CORE_ACC}, {3, WPAD_FMT_CORE_ACC_DPD},
    {0, WPAD_FMT_FREESTYLE_ACC}, {1, WPAD_FMT_FREESTYLE_ACC_DPD},
    {0, WPAD_FMT_CLASSIC_ACC}, {1, WPAD_FMT_CLASSIC_ACC_DPD},
    {0, WPAD_FMT_GUITAR}, {1, WPAD_FMT_GUITAR},
    {0, WPAD_FMT_BALANCE_CHECKER}, {0, WPAD_FMT_BALANCE_CHECKER},
    {0, WPAD_FMT_TRAIN}, {0, WPAD_FMT_TRAIN}
};


void KPADiSamplingCallback(s32 chan);
void KPADiConnectCallback(s32 chan, s32 result);
void KPADiControlDpdCallback(s32 chan, s32 result);

void KPADSetBtnRepeat(s32 chan, f32 delay_sec, f32 pulse_sec) {
    KPADInsideStatus* kp = &inside_kpads[chan];
    if (pulse_sec) {
        kp->btn_repeat_delay = (u16)(s32)(delay_sec * 200.0f + 0.5f);
        kp->btn_repeat_pulse = (u16)(s32)(pulse_sec * 200.0f + 0.5f);
    } else {
        kp->btn_repeat_delay = KPAD_BTN_NO_RPT_DELAY;
        kp->btn_repeat_pulse = 0;
    }

    kp->btn_repeat_time = 0;
    kp->btn_repeat_next = kp->btn_repeat_delay;
    kp->btn_cl_repeat_time = 0;
    kp->btn_cl_repeat_next = kp->btn_repeat_delay;
}

void KPADSetPosParam(s32 chan, f32 play_radius, f32 sensitivity) {
    inside_kpads[chan].pos_play_radius = play_radius;
    inside_kpads[chan].pos_sensitivity = sensitivity;
}

void KPADSetHoriParam(s32 chan, f32 play_radius, f32 sensitivity) {
    inside_kpads[chan].hori_play_radius = play_radius;
    inside_kpads[chan].hori_sensitivity = sensitivity;
}

void KPADSetDistParam(s32 chan, f32 play_radius, f32 sensitivity) {
    inside_kpads[chan].dist_play_radius = play_radius;
    inside_kpads[chan].dist_sensitivity = sensitivity;
}

void KPADSetAccParam(s32 chan, f32 play_radius, f32 sensitivity) {
    inside_kpads[chan].acc_play_radius = play_radius;
    inside_kpads[chan].acc_sensitivity = sensitivity;
}

void reset_kpad(KPADInsideStatus* kp) {
    KPADObject* op;
    KPADStatus* sp = &kp->status;
    KPADEXStatus* ep = &kp->status.ex_status;

    kp->resetReq = FALSE;

    kp->kobj_frame_min.x = -1.0f + kp_err_outside_frame;
    kp->kobj_frame_max.x = 1.0f - kp_err_outside_frame;
    kp->kobj_frame_min.y = -((f32)KPAD_DPD_RESO_WY / KPAD_DPD_RESO_WX) + kp_err_outside_frame;
    kp->kobj_frame_max.y = ((f32)KPAD_DPD_RESO_WY / KPAD_DPD_RESO_WX) - kp_err_outside_frame;

    kp->err_dist_speed_1 = 1.0f / kp_err_dist_speed;
    kp->err_dist_speedM_1 = -1.0f / kp_err_dist_speed;
    kp->ah_circle_radius2 = kp_ah_circle_radius * kp_ah_circle_radius;

    kp->err_dist_min = kp_err_dist_min;
    kp->dist_vv1 = kp_dist_vv1;

    sp->hold = sp->trig = sp->release = 0;
    kp->btn_repeat_time = 0;
    kp->btn_repeat_next = kp->btn_repeat_delay;

    sp->dpd_valid_fg = 0;
    kp->dpd_valid2_ct = 0;

    sp->pos = sp->vec = Vec2_0;
    sp->speed = 0.0f;

    sp->horizon.x = kp->acc_horizon.x = kp->obj_horizon.x = 1.0f;
    sp->horizon.y = kp->acc_horizon.y = kp->obj_horizon.y = 0.0f;
    sp->hori_vec = Vec2_0;
    sp->hori_speed = 0.0f;

    sp->acc_vertical.x = 1.0f;
    sp->acc_vertical.y = 0.0f;

    sp->dist = kp->dist_org;
    sp->dist_vec = sp->dist_speed = 0.0f;

    kp->sec_dist = sp->dist;
    kp->sec_length = kp->trust_sec_length = kp->dist_vv1 / kp->sec_dist;
    kp->sec_nrm = kp->sec_nrm_hori;

    sp->acc.x = sp->acc.z = 0.0f;
    sp->acc.y = -1.0f;
    sp->acc_value = 1.0f;
    sp->acc_speed = 0.0f;
    kp->hard_acc = sp->acc;

    kp->ah_circle_pos = kp->acc_horizon;
    kp->ah_circle_ct = kp_ah_circle_ct;

    kp->valid_objs = 0;

    op = &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1];
    do {
        op->error_fg = -1;
    } while (--op >= kp->kobj_sample);

    op = &kp->kobj_regular[KPAD_USE_OBJECTS - 1];
    do {
        op->error_fg = -1;
    } while (--op >= kp->kobj_regular);

    kp->bufCount = 0;
    kp->exResetReq = TRUE;
}

void KPADEnableAimingMode(s32 chan) {
    inside_kpads[chan].aimReq = TRUE;
    inside_kpads[chan].aimEnabled = TRUE;
}

void calc_button_repeat(KPADInsideStatus* kp, u32 dev_type, u32 count) {
    KPADStatus* sp = &kp->status;
    KPADEXStatus* ep = &kp->status.ex_status;

    if (sp->trig != 0 || sp->release != 0) {
        kp->btn_repeat_time = 0;
        kp->btn_repeat_next = kp->btn_repeat_delay;
        if (sp->trig && kp->btn_repeat_pulse) {
            sp->hold |= KPAD_BUTTON_RPT;
        }
    } else if (sp->hold != 0) {
        kp->btn_repeat_time += count;
        if (kp->btn_repeat_time >= KPAD_BTN_NO_RPT_DELAY) {
            kp->btn_repeat_time -= KPAD_BTN_NO_RPT_DELAY;
        }

        if (kp->btn_repeat_time >= kp->btn_repeat_next) {
            sp->hold |= KPAD_BUTTON_RPT;
            kp->btn_repeat_next += kp->btn_repeat_pulse;
            if (kp->btn_repeat_time >= KPAD_BTN_RPT_TIME_MAX) {
                kp->btn_repeat_time -= KPAD_BTN_RPT_TIME_MAX;
                kp->btn_repeat_next -= KPAD_BTN_RPT_TIME_MAX;
            }
        }
    }

    if (dev_type == WPAD_DEV_CLASSIC || dev_type == WPAD_DEV_TRAIN || dev_type == WPAD_DEV_GUITAR) {
        if (ep->cl.trig != 0 || ep->cl.release != 0) {
            kp->btn_cl_repeat_time = 0;
            kp->btn_cl_repeat_next = kp->btn_repeat_delay;
            if (ep->cl.trig && kp->btn_repeat_pulse) {
                ep->cl.hold |= KPAD_BUTTON_RPT;
            }
        } else if (ep->cl.hold != 0) {
            kp->btn_cl_repeat_time += count;
            if (kp->btn_cl_repeat_time >= KPAD_BTN_NO_RPT_DELAY) {
                kp->btn_cl_repeat_time -= KPAD_BTN_NO_RPT_DELAY;
            }

            if (kp->btn_cl_repeat_time >= kp->btn_cl_repeat_next) {
                ep->cl.hold |= KPAD_BUTTON_RPT;
                kp->btn_cl_repeat_next += kp->btn_repeat_pulse;
                if (kp->btn_cl_repeat_time >= KPAD_BTN_RPT_TIME_MAX) {
                    kp->btn_cl_repeat_time -= KPAD_BTN_RPT_TIME_MAX;
                    kp->btn_cl_repeat_next -= KPAD_BTN_RPT_TIME_MAX;
                }
            }
        }
    }
}

void read_kpad_button(KPADInsideStatus* kp, u32 dev_type, u32 count, u32 core, u32 fs, u32 cl) {
    KPADStatus* sp = &kp->status;
    KPADEXStatus* ep = &kp->status.ex_status;
    u32 old_fg, change_fg;

    old_fg = sp->hold & KPAD_BUTTON_MASK;
    sp->hold = core & (KPAD_BUTTON_MASK & ~(WPAD_BUTTON_Z | WPAD_BUTTON_C));
    if (dev_type == WPAD_DEV_FREESTYLE) {
        sp->hold |= (kp->exResetReq ? 0 : fs) & (WPAD_BUTTON_Z | WPAD_BUTTON_C);
    }
    change_fg = sp->hold ^ old_fg;
    sp->trig = change_fg & sp->hold;
    sp->release = change_fg & old_fg;

    if (dev_type == WPAD_DEV_CLASSIC || dev_type == WPAD_DEV_TRAIN || dev_type == WPAD_DEV_GUITAR) {
        u32 cl_button = kp->exResetReq ? 0 : cl;
        old_fg = ep->cl.hold & KPAD_BUTTON_MASK;
        ep->cl.hold = cl_button & KPAD_BUTTON_MASK;
        change_fg = ep->cl.hold ^ old_fg;
        ep->cl.trig = change_fg & ep->cl.hold;
        ep->cl.release = change_fg & old_fg;
    }

    calc_button_repeat(kp, dev_type, count);
}

void calc_acc(KPADInsideStatus* kp, f32* acc, f32 acc2) {
    f32 f1, f2;

    f2 = acc2 - *acc;

    if (kp->acc_play_mode == KPAD_PLAY_MODE_LOOSE) {
        if (f2 < 0.0f) {
            f1 = -f2;
        } else {
            f1 = f2;
        }

        if (f1 >= kp->acc_play_radius) {
            f1 = 1.0f;
        } else {
            f1 /= kp->acc_play_radius;
            f1 *= f1;
            f1 *= f1;
        }
        f1 *= kp->acc_sensitivity;
        *acc += f1 * f2;
    } else {
        if (f2 < -kp->acc_play_radius) {
            *acc += (f2 + kp->acc_play_radius) * kp->acc_sensitivity;
        } else if (f2 > kp->acc_play_radius) {
            *acc += (f2 - kp->acc_play_radius) * kp->acc_sensitivity;
        }
    }
}

void calc_acc_horizon(KPADInsideStatus* kp) {
    f32 f1, vx, vy, ax, ay;
    f1 = sqrt(kp->hard_acc.x * kp->hard_acc.x + kp->hard_acc.y * kp->hard_acc.y);
    if (f1 == 0.0f || f1 >= 2.0f) {
        return;
    }
    ax = kp->hard_acc.x / f1;
    ay = kp->hard_acc.y / f1;

    if (f1 > 1.0f) {
        f1 = 2.0f - f1;
    }
    f1 *= f1 * kp_acc_horizon_pw;

    vx = kp->accXY_nrm_hori.x * ax + kp->accXY_nrm_hori.y * ay;
    vy = kp->accXY_nrm_hori.y * ax - kp->accXY_nrm_hori.x * ay;
    ax = (vx - kp->acc_horizon.x) * f1 + kp->acc_horizon.x;
    ay = (vy - kp->acc_horizon.y) * f1 + kp->acc_horizon.y;

    f1 = sqrt(ax * ax + ay * ay);
    if (f1 == 0.0f) {
        return;
    }
    kp->acc_horizon.x = ax / f1;
    kp->acc_horizon.y = ay / f1;

    kp->ah_circle_pos.x += (kp->acc_horizon.x - kp->ah_circle_pos.x) * kp_ah_circle_pw;
    kp->ah_circle_pos.y += (kp->acc_horizon.y - kp->ah_circle_pos.y) * kp_ah_circle_pw;

    vx = kp->acc_horizon.x - kp->ah_circle_pos.x;
    vy = kp->acc_horizon.y - kp->ah_circle_pos.y;
    if (vx * vx + vy * vy <= kp->ah_circle_radius2) {
        if (kp->ah_circle_ct)
            --kp->ah_circle_ct;
    } else {
        kp->ah_circle_ct = kp_ah_circle_ct;
    }
}

static void clamp_trigger(f32* trigger, s32 tr, s32 min, s32 max) {
    if (tr <= min) {
        *trigger = 0.0f;
    } else if (tr >= max) {
        *trigger = 1.0f;
    } else {
        *trigger = (f32)(tr - min) / (f32)(max - min);
    }
}

void calc_acc_vertical(KPADInsideStatus* kp) {
    KPADStatus* sp = &kp->status;
    f32 f1, f2, ax, ay;

    ax = sqrt(f2 = kp->hard_acc.x * kp->hard_acc.x + kp->hard_acc.y * kp->hard_acc.y);
    ay = -kp->hard_acc.z;
    f1 = sqrt(f2 + ay * ay);
    if (f1 == 0.0f || f1 >= 2.0f) {
        return;
    }
    f2 = ax / f1;
    ay /= f1;

    if (f1 > 1.0f) {
        f1 = 2.0f - f1;
    }
    f1 *= f1 * kp_acc_horizon_pw;

    ax = (f2 - sp->acc_vertical.x) * f1 + sp->acc_vertical.x;
    ay = (ay - sp->acc_vertical.y) * f1 + sp->acc_vertical.y;

    f1 = sqrt(ax * ax + ay * ay);
    if (f1 == 0.0f) {
        return;
    }
    sp->acc_vertical.x = ax / f1;
    sp->acc_vertical.y = ay / f1;
}

static f32 clamp_acc(f32 acc, f32 clamp) {
    if (acc < 0.0f) {
        if (acc < -clamp)
            return -clamp;
    } else {
        if (acc > clamp)
            return clamp;
    }
    return acc;
}

void read_kpad_acc(KPADInsideStatus* kp, KPADUnifiedWpadStatus* uwp) {
    KPADStatus* sp = &kp->status;
    Vec fsrc;
    Vec vec;

    switch (uwp->fmt) {
    case WPAD_FMT_CORE_ACC:
    case WPAD_FMT_CORE_ACC_DPD:
    case WPAD_FMT_FREESTYLE_ACC:
    case WPAD_FMT_FREESTYLE_ACC_DPD:
    case WPAD_FMT_CLASSIC_ACC:
    case WPAD_FMT_CLASSIC_ACC_DPD:
    case WPAD_FMT_GUITAR:
        break;

    default:
        return;
    }

    kp->hard_acc.x = clamp_acc((f32)(s32)-uwp->u.core.accX * kp->acc_scale_x, kp_rm_acc_max);
    kp->hard_acc.y = clamp_acc((f32)(s32)-uwp->u.core.accZ * kp->acc_scale_z, kp_rm_acc_max);
    kp->hard_acc.z = clamp_acc((f32)(s32)uwp->u.core.accY * kp->acc_scale_y, kp_rm_acc_max);

    vec = sp->acc;

    calc_acc(kp, &sp->acc.x, kp->hard_acc.x);
    calc_acc(kp, &sp->acc.y, kp->hard_acc.y);
    calc_acc(kp, &sp->acc.z, kp->hard_acc.z);
    sp->acc_value = sqrt(sp->acc.x * sp->acc.x + sp->acc.y * sp->acc.y + sp->acc.z * sp->acc.z);

    vec.x -= sp->acc.x;
    vec.y -= sp->acc.y;
    vec.z -= sp->acc.z;
    sp->acc_speed = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    calc_acc_horizon(kp);
    calc_acc_vertical(kp);
    if (uwp->u.fs.err != WPAD_ERR_NONE || uwp->u.fs.dev != WPAD_DEV_FREESTYLE ||
        (uwp->fmt != WPAD_FMT_FREESTYLE_ACC && uwp->fmt != WPAD_FMT_FREESTYLE_ACC_DPD)) {
        return;
    }

    fsrc.x = clamp_acc((f32)(s32)-uwp->u.fs.fsAccX * kp->fs_acc_scale_x, kp_fs_acc_max);
    fsrc.y = clamp_acc((f32)(s32)-uwp->u.fs.fsAccZ * kp->fs_acc_scale_z, kp_fs_acc_max);
    fsrc.z = clamp_acc((f32)(s32)uwp->u.fs.fsAccY * kp->fs_acc_scale_y, kp_fs_acc_max);

    if (kp->fsAccRevise) {
        PSMTXMultVec(kp_fs_rot, &fsrc, &fsrc);
    }

    vec = sp->ex_status.fs.acc;

    calc_acc(kp, &sp->ex_status.fs.acc.x, fsrc.x);
    calc_acc(kp, &sp->ex_status.fs.acc.y, fsrc.y);
    calc_acc(kp, &sp->ex_status.fs.acc.z, fsrc.z);
    sp->ex_status.fs.acc_value = sqrt(sp->ex_status.fs.acc.x * sp->ex_status.fs.acc.x + sp->ex_status.fs.acc.y * sp->ex_status.fs.acc.y +
                                      sp->ex_status.fs.acc.z * sp->ex_status.fs.acc.z);

    vec.x -= sp->ex_status.fs.acc.x;
    vec.y -= sp->ex_status.fs.acc.y;
    vec.z -= sp->ex_status.fs.acc.z;
    sp->ex_status.fs.acc_speed = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

static f32 calc_horizon(KPADInsideStatus* kp, Vec2* p1, Vec2* p2, Vec2* hori) {
    f32 f1, f2, vx, vy;

    vx = p2->x - p1->x;
    vy = p2->y - p1->y;
    f2 = sqrt(vx * vx + vy * vy);
    f1 = 1.0f / f2;
    vx *= f1;
    vy *= f1;

    hori->x = kp->sec_nrm_hori.x * vx + kp->sec_nrm_hori.y * vy;
    hori->y = kp->sec_nrm_hori.y * vx - kp->sec_nrm_hori.x * vy;

    return kp->dist_vv1 * f1;
}

s8 select_2obj_first(KPADInsideStatus* kp) {
    KPADObject *op1, *op2, *rp1, *rp2;
    Vec2 hori;
    f32 f1, max = kp_err_first_inpr;

    op1 = kp->kobj_sample;
    do {
        if (op1->error_fg != 0) {
            continue;
        }

        op2 = op1 + 1;
        do {
            if (op2->error_fg != 0) {
                continue;
            }

            f1 = calc_horizon(kp, &op1->center, &op2->center, &hori);

            if (f1 <= kp->err_dist_min || f1 >= kp_err_dist_max) {
                continue;
            }

            f1 = kp->acc_horizon.x * hori.x + kp->acc_horizon.y * hori.y;
            if (f1 < 0.0f) {
                if (-f1 > max) {
                    max = -f1;
                    rp1 = op2;
                    rp2 = op1;
                }
            } else {
                if (f1 > max) {
                    max = f1;
                    rp1 = op1;
                    rp2 = op2;
                }
            }

        } while (++op2 <= &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1]);
    } while (++op1 < &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1]);

    if (max == kp_err_first_inpr) {
        return 0;
    }

    kp->kobj_regular[0] = *rp1;
    kp->kobj_regular[1] = *rp2;

    return 2;
}

s8 select_2obj_continue(KPADInsideStatus* kp) {
    KPADObject *op1, *op2, *rp1, *rp2;
    Vec2 nrm;
    s32 rev_fg;
    f32 f1, f2, f3, vx, vy, min = 2.0f;
    op1 = kp->kobj_sample;
    do {
        if (op1->error_fg != 0) {
            continue;
        }

        op2 = op1 + 1;
        do {
            if (op2->error_fg != 0) {
                continue;
            }
            vx = op2->center.x - op1->center.x;
            vy = op2->center.y - op1->center.y;
            f3 = sqrt(vx * vx + vy * vy);
            f1 = 1.0f / f3;
            nrm.x = vx * f1;
            nrm.y = vy * f1;

            f1 *= kp->dist_vv1;
            if (f1 <= kp->err_dist_min || f1 >= kp_err_dist_max) {
                continue;
            }

            f1 -= kp->sec_dist;
            if (f1 < 0.0f) {
                f1 *= kp->err_dist_speedM_1;
            } else {
                f1 *= kp->err_dist_speed_1;
            }
            if (f1 >= 1.0f) {
                continue;
            }

            f2 = kp->sec_nrm.x * nrm.x + kp->sec_nrm.y * nrm.y;
            if (f2 < 0.0f) {
                f2 = -f2;
                rev_fg = 1;
            } else {
                rev_fg = 0;
            }
            if (f2 <= kp_err_next_inpr) {
                continue;
            }
            f2 = (1.0f - f2) / (1.0f - kp_err_next_inpr);
            f1 += f2;
            if (f1 < min) {
                min = f1;
                if (rev_fg) {
                    rp1 = op2;
                    rp2 = op1;
                } else {
                    rp1 = op1;
                    rp2 = op2;
                }
            }

        } while (++op2 <= &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1]);
    } while (++op1 < &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1]);

    if (min == 2.0f) {
        return 0;
    }

    kp->kobj_regular[0] = *rp1;
    kp->kobj_regular[1] = *rp2;

    return 2;
}

s8 select_1obj_first(KPADInsideStatus* kp) {
    KPADObject* op1;
    f32 vx, vy;
    Vec2 p1, p2;

    vx = kp->sec_nrm_hori.x * kp->acc_horizon.x + kp->sec_nrm_hori.y * kp->acc_horizon.y;
    vy = kp->sec_nrm_hori.y * kp->acc_horizon.x - kp->sec_nrm_hori.x * kp->acc_horizon.y;

    vx *= kp->trust_sec_length;
    vy *= kp->trust_sec_length;

    op1 = kp->kobj_sample;
    do {
        if (op1->error_fg != 0) {
            continue;
        }

        p1.x = op1->center.x - vx;
        p1.y = op1->center.y - vy;
        p2.x = op1->center.x + vx;
        p2.y = op1->center.y + vy;

        if (p1.x <= kp->kobj_frame_min.x || p1.x >= kp->kobj_frame_max.x || p1.y <= kp->kobj_frame_min.y || p1.y >= kp->kobj_frame_max.y) {
            if (p2.x > kp->kobj_frame_min.x && p2.x < kp->kobj_frame_max.x && p2.y > kp->kobj_frame_min.y && p2.y < kp->kobj_frame_max.y) {
                kp->kobj_regular[1] = *op1;
                kp->kobj_regular[0].center = p1;
                kp->kobj_regular[0].error_fg = 0;
                kp->kobj_regular[0].state_fg = -1;

                return -1;
            }
        } else {
            if (p2.x <= kp->kobj_frame_min.x || p2.x >= kp->kobj_frame_max.x || p2.y <= kp->kobj_frame_min.y || p2.y >= kp->kobj_frame_max.y) {
                kp->kobj_regular[0] = *op1;

                kp->kobj_regular[1].center = p2;
                kp->kobj_regular[1].error_fg = 0;
                kp->kobj_regular[1].state_fg = -1;

                return -1;
            }
        }

    } while (++op1 < &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS]);

    return 0;
}

s8 select_1obj_continue(KPADInsideStatus* kp) {
    KPADObject *op1, *op2, *rp1, *rp2;
    f32 f1, vx, vy;
    f32 min = kp_err_near_pos * kp_err_near_pos;

    op1 = kp->kobj_regular;
    do {
        if (op1->error_fg != 0) {
            continue;
        }
        if (op1->state_fg != 0) {
            continue;
        }

        op2 = kp->kobj_sample;
        do {
            if (op2->error_fg != 0) {
                continue;
            }

            vx = op1->center.x - op2->center.x;
            vy = op1->center.y - op2->center.y;
            f1 = vx * vx + vy * vy;
            if (f1 < min) {
                min = f1;
                rp1 = op1;
                rp2 = op2;
            }
        } while (++op2 < &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS]);
    } while (++op1 < &kp->kobj_regular[KPAD_USE_OBJECTS]);

    if (min == kp_err_near_pos * kp_err_near_pos)
        return 0;

    *rp1 = *rp2;
    kp->sec_nrm.x = kp->sec_nrm_hori.x * kp->acc_horizon.x + kp->sec_nrm_hori.y * kp->acc_horizon.y;
    kp->sec_nrm.y = kp->sec_nrm_hori.y * kp->acc_horizon.x - kp->sec_nrm_hori.x * kp->acc_horizon.y;

    vx = kp->sec_length * kp->sec_nrm.x;
    vy = kp->sec_length * kp->sec_nrm.y;
    if (rp1 == &kp->kobj_regular[0]) {
        kp->kobj_regular[1].center.x = rp1->center.x + vx;
        kp->kobj_regular[1].center.y = rp1->center.y + vy;
        kp->kobj_regular[1].error_fg = 0;
        kp->kobj_regular[1].state_fg = -1;
    } else {
        kp->kobj_regular[0].center.x = rp1->center.x - vx;
        kp->kobj_regular[0].center.y = rp1->center.y - vy;
        kp->kobj_regular[0].error_fg = 0;
        kp->kobj_regular[0].state_fg = -1;
    }

    if (kp->status.dpd_valid_fg < 0) {
        return -1;
    } else {
        return 1;
    }
}

void calc_dpd_variable(KPADInsideStatus* kp, s8 valid_fg_next) {
    KPADStatus* sp = &kp->status;
    f32 f1, f2, dist;
    Vec2 pos, vec;

    if (valid_fg_next == 0) {
        sp->dpd_valid_fg = 0;
        return;
    }

    pos.x = kp->sec_nrm_hori.x * kp->sec_nrm.x + kp->sec_nrm_hori.y * kp->sec_nrm.y;
    pos.y = kp->sec_nrm_hori.y * kp->sec_nrm.x - kp->sec_nrm_hori.x * kp->sec_nrm.y;

    if (sp->dpd_valid_fg == 0) {
        sp->horizon = pos;
        sp->hori_vec = Vec2_0;
        sp->hori_speed = 0.0f;
    } else {
        vec.x = pos.x - sp->horizon.x;
        vec.y = pos.y - sp->horizon.y;
        f1 = sqrt(vec.x * vec.x + vec.y * vec.y);

        if (kp->hori_play_mode == KPAD_PLAY_MODE_LOOSE) {
            if (f1 >= kp->hori_play_radius) {
                f2 = 1.0f;
            } else {
                f2 = f1 / kp->hori_play_radius;
                f2 *= f2;
                f2 *= f2;
            }
            f2 *= kp->hori_sensitivity;
            vec.x = f2 * vec.x + sp->horizon.x;
            vec.y = f2 * vec.y + sp->horizon.y;
            f1 = sqrt(vec.x * vec.x + vec.y * vec.y);
            vec.x /= f1;
            vec.y /= f1;

            sp->hori_vec.x = vec.x - sp->horizon.x;
            sp->hori_vec.y = vec.y - sp->horizon.y;
            sp->hori_speed = sqrt(sp->hori_vec.x * sp->hori_vec.x + sp->hori_vec.y * sp->hori_vec.y);

            sp->horizon = vec;
        } else {
            if (f1 > kp->hori_play_radius) {
                f1 = (f1 - kp->hori_play_radius) / f1 * kp->hori_sensitivity;
                vec.x = vec.x * f1 + sp->horizon.x;
                vec.y = vec.y * f1 + sp->horizon.y;
                f1 = sqrt(vec.x * vec.x + vec.y * vec.y);
                vec.x /= f1;
                vec.y /= f1;

                sp->hori_vec.x = vec.x - sp->horizon.x;
                sp->hori_vec.y = vec.y - sp->horizon.y;
                sp->hori_speed = sqrt(sp->hori_vec.x * sp->hori_vec.x + sp->hori_vec.y * sp->hori_vec.y);

                sp->horizon = vec;
            } else {
                sp->hori_vec = Vec2_0;
                sp->hori_speed = 0.0f;
            }
        }
    }

    dist = kp->dist_vv1 / kp->sec_length;

    if (sp->dpd_valid_fg == 0) {
        sp->dist = dist;
        sp->dist_vec = 0.0f;
        sp->dist_speed = 0.0f;
    } else {
        f2 = dist - sp->dist;
        if (f2 < 0.0f) {
            f1 = -f2;
        } else {
            f1 = f2;
        }

        if (kp->dist_play_mode == KPAD_PLAY_MODE_LOOSE) {
            if (f1 >= kp->dist_play_radius) {
                f1 = 1.0f;
            } else {
                f1 /= kp->dist_play_radius;
                f1 *= f1;
                f1 *= f1;
            }
            f1 *= kp->dist_sensitivity;

            sp->dist_vec = f1 * f2;
            if (sp->dist_vec < 0.0f) {
                sp->dist_speed = -sp->dist_vec;
            } else {
                sp->dist_speed = sp->dist_vec;
            }

            sp->dist += sp->dist_vec;
        } else {
            if (f1 > kp->dist_play_radius) {
                f1 = (f1 - kp->dist_play_radius) / f1 * kp->dist_sensitivity;
                sp->dist_vec = f1 * f2;
                if (sp->dist_vec < 0.0f) {
                    sp->dist_speed = -sp->dist_vec;
                } else {
                    sp->dist_speed = sp->dist_vec;
                }

                sp->dist += sp->dist_vec;
            } else {
                sp->dist_vec = 0.0f;
                sp->dist_speed = 0.0f;
            }
        }
    }

    pos.x = (kp->kobj_regular[0].center.x + kp->kobj_regular[1].center.x) * 0.5f;
    pos.y = (kp->kobj_regular[0].center.y + kp->kobj_regular[1].center.y) * 0.5f;

    f1 = kp->sec_nrm.x * kp->sec_nrm_hori.x + kp->sec_nrm.y * kp->sec_nrm_hori.y;
    f2 = -kp->sec_nrm.y * kp->sec_nrm_hori.x + kp->sec_nrm.x * kp->sec_nrm_hori.y;
    vec.x = f1 * pos.x - f2 * pos.y;
    vec.y = f2 * pos.x + f1 * pos.y;

    vec.x = (kp->center_org.x - vec.x) * kp->dpd2pos_scale;
    vec.y = (kp->center_org.y - vec.y) * kp->dpd2pos_scale;

    pos.x = -kp->accXY_nrm_hori.y * vec.x + kp->accXY_nrm_hori.x * vec.y;
    pos.y = -kp->accXY_nrm_hori.x * vec.x - kp->accXY_nrm_hori.y * vec.y;
    if (sp->dpd_valid_fg == 0) {
        sp->pos = pos;
        sp->vec = Vec2_0;
        sp->speed = 0.0f;
    } else {
        vec.x = pos.x - sp->pos.x;
        vec.y = pos.y - sp->pos.y;
        f1 = sqrt(vec.x * vec.x + vec.y * vec.y);

        if (kp->pos_play_mode == KPAD_PLAY_MODE_LOOSE) {
            if (f1 >= kp->pos_play_radius) {
                f2 = 1.0f;
            } else {
                f2 = f1 / kp->pos_play_radius;
                f2 *= f2;
                f2 *= f2;
            }
            f2 *= kp->pos_sensitivity;

            sp->vec.x = f2 * vec.x;
            sp->vec.y = f2 * vec.y;
            sp->speed = sqrt(sp->vec.x * sp->vec.x + sp->vec.y * sp->vec.y);

            sp->pos.x += sp->vec.x;
            sp->pos.y += sp->vec.y;
        } else {
            if (f1 > kp->pos_play_radius) {
                f1 = (f1 - kp->pos_play_radius) / f1 * kp->pos_sensitivity;
                sp->vec.x = f1 * vec.x;
                sp->vec.y = f1 * vec.y;
                sp->speed = sqrt(sp->vec.x * sp->vec.x + sp->vec.y * sp->vec.y);

                sp->pos.x += sp->vec.x;
                sp->pos.y += sp->vec.y;
            } else {
                sp->vec = Vec2_0;
                sp->speed = 0.0f;
            }
        }
    }

    sp->dpd_valid_fg = valid_fg_next;
}

static void calc_obj_horizon(KPADInsideStatus* kp) {
    f32 f1, f2, vx, vy;

    vx = kp->kobj_regular[1].center.x - kp->kobj_regular[0].center.x;
    vy = kp->kobj_regular[1].center.y - kp->kobj_regular[0].center.y;
    f1 = sqrt(vx * vx + vy * vy);
    kp->sec_length = f1;

    f2 = 1.0f / f1;
    kp->sec_dist = kp->dist_vv1 * f2;

    vx *= f2;
    vy *= f2;
    kp->sec_nrm.x = vx;
    kp->sec_nrm.y = vy;

    kp->obj_horizon.x = kp->sec_nrm_hori.x * vx + kp->sec_nrm_hori.y * vy;
    kp->obj_horizon.y = kp->sec_nrm_hori.y * vx - kp->sec_nrm_hori.x * vy;
}

static void get_kobj(KPADInsideStatus* kp, DPDObject* wobj_p) {
    const f32 dpd_scale = 2.0f / (f32)KPAD_DPD_RESO_WX;
    const f32 dpd_cx = (f32)(KPAD_DPD_RESO_WX - 1) / (f32)KPAD_DPD_RESO_WX;
    const f32 dpd_cy = (f32)(KPAD_DPD_RESO_WY - 1) / (f32)KPAD_DPD_RESO_WX;

    KPADObject* kobj_p;

    kobj_p = &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1];
    do {
        if (wobj_p->size) {
            kobj_p->center.x = (f32)(s32)wobj_p->x * dpd_scale - dpd_cx;
            kobj_p->center.y = (f32)(s32)wobj_p->y * dpd_scale - dpd_cy;

            kobj_p->error_fg = 0;
            kobj_p->state_fg = 0;
        } else {
            kobj_p->error_fg = -1;
        }

        --wobj_p;
    } while (--kobj_p >= kp->kobj_sample);
}

static void check_kobj_outside_frame(KPADInsideStatus* kp, KPADObject* kobj_t) {
    KPADObject* kobj_p = &kobj_t[WPAD_DPD_MAX_OBJECTS - 1];

    do {
        if (kobj_p->error_fg < 0) {
            continue;
        }

        if (kobj_p->center.x <= kp->kobj_frame_min.x || kobj_p->center.x >= kp->kobj_frame_max.x || kobj_p->center.y <= kp->kobj_frame_min.y ||
            kobj_p->center.y >= kp->kobj_frame_max.y) {
            kobj_p->error_fg |= 1;
        }
    } while (--kobj_p >= kobj_t);
}

static void check_kobj_same_position(KPADObject* kobj_t) {
    KPADObject *op1, *op2;

    op1 = kobj_t;
    do {
        if (op1->error_fg != 0) {
            continue;
        }

        op2 = op1 + 1;
        do {
            if (op2->error_fg != 0) {
                continue;
            }

            if (op1->center.x == op2->center.x && op1->center.y == op2->center.y) {
                op2->error_fg |= 2;
            }
        } while (++op2 <= &kobj_t[WPAD_DPD_MAX_OBJECTS - 1]);
    } while (++op1 < &kobj_t[WPAD_DPD_MAX_OBJECTS - 1]);
}

void read_kpad_dpd(KPADInsideStatus* kp, KPADUnifiedWpadStatus* uwp) {
    KPADStatus* sp = &kp->status;
    KPADObject* op1;
    s8 valid_fg_next;

    if (uwp->fmt == WPAD_FMT_CORE_ACC_DPD || uwp->fmt == WPAD_FMT_FREESTYLE_ACC_DPD || uwp->fmt == WPAD_FMT_CLASSIC_ACC_DPD || (uwp->fmt == WPAD_FMT_GUITAR && kp->dpdHardwareEnabled)) {
        get_kobj(kp, &uwp->u.core.obj[WPAD_DPD_MAX_OBJECTS - 1]);
    } else {
        op1 = &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1];
        do {
            op1->error_fg = -1;
        } while (--op1 >= kp->kobj_sample);
    }

    check_kobj_outside_frame(kp, kp->kobj_sample);
    check_kobj_same_position(kp->kobj_sample);
    kp->valid_objs = 0;
    op1 = &kp->kobj_sample[WPAD_DPD_MAX_OBJECTS - 1];
    do {
        if (op1->error_fg == 0) {
            ++kp->valid_objs;
        }
    } while (--op1 >= kp->kobj_sample);

    if (sp->acc_vertical.x <= kp_err_up_inpr) {
        goto LABEL_select_NG;
    }

    if (sp->dpd_valid_fg == 2 || sp->dpd_valid_fg == -2) {
        if (kp->valid_objs >= 2) {
            valid_fg_next = select_2obj_continue(kp);
            if (valid_fg_next) {
                goto LABEL_select_OK;
            }
        }
        if (kp->valid_objs >= 1) {
            valid_fg_next = select_1obj_continue(kp);
            if (valid_fg_next) {
                goto LABEL_select_OK;
            }
        }
    } else if (sp->dpd_valid_fg == 1 || sp->dpd_valid_fg == -1) {
        if (kp->valid_objs >= 2) {
            valid_fg_next = select_2obj_first(kp);
            if (valid_fg_next) {
                goto LABEL_select_OK;
            }
        }
        if (kp->valid_objs >= 1) {
            valid_fg_next = select_1obj_continue(kp);
            if (valid_fg_next) {
                goto LABEL_select_OK;
            }
        }
    } else {
        if (kp->valid_objs >= 2) {
            valid_fg_next = select_2obj_first(kp);

            if (valid_fg_next) {
                goto LABEL_select_OK;
            }
        }
        if (kp->valid_objs == 1) {
            valid_fg_next = select_1obj_first(kp);
            if (valid_fg_next)
                goto LABEL_select_OK;
        }
    }

LABEL_select_NG:
    valid_fg_next = 0;

LABEL_select_OK:
    if (valid_fg_next) {
        calc_obj_horizon(kp);
        if (kp->ah_circle_ct == 0) {
            if (kp->obj_horizon.x * kp->acc_horizon.x + kp->obj_horizon.y * kp->acc_horizon.y <= kp_err_acc_inpr) {
                valid_fg_next = 0;

                kp->kobj_regular[0].error_fg = kp->kobj_regular[1].error_fg = 1;
            }
        }

        if (sp->dpd_valid_fg == 2 && valid_fg_next == 2) {
            if (kp->dpd_valid2_ct == 200) {
                kp->trust_sec_length = kp->sec_length;
            } else {
                ++kp->dpd_valid2_ct;
            }
        } else {
            kp->dpd_valid2_ct = 0;
        }
    } else {
        kp->dpd_valid2_ct = 0;
    }

    calc_dpd_variable(kp, valid_fg_next);
}

void clamp_stick_circle(Vec2* stick, s32 sx, s32 sy, s32 min, s32 max) {
    f32 length;
    f32 fx = (f32)sx;
    f32 fy = (f32)sy;
    f32 fmin = (f32)min;
    f32 fmax = (f32)max;

    length = sqrt(fx * fx + fy * fy);

    if (length <= fmin) {
        stick->x = stick->y = 0.0f;

    } else if (length >= fmax) {
        stick->x = fx / length;
        stick->y = fy / length;

    } else {
        length = (length - fmin) / (fmax - fmin) / length;
        stick->x = fx * length;
        stick->y = fy * length;
    }
}

void clamp_stick_cross(Vec2* stick, s32 sx, s32 sy, s32 min, s32 max) {
    f32 length;

    if (sx < 0) {
        clamp_trigger(&stick->x, -sx, min, max);
        stick->x = -stick->x;
    } else {
        clamp_trigger(&stick->x, sx, min, max);
    }
    if (sy < 0) {
        clamp_trigger(&stick->y, -sy, min, max);
        stick->y = -stick->y;
    } else {
        clamp_trigger(&stick->y, sy, min, max);
    }

    length = stick->x * stick->x + stick->y * stick->y;
    if (length > 1.0f) {
        length = sqrt(length);
        stick->x /= length;
        stick->y /= length;
    }
}

enum {
    KPAD_CORE_BUTTON_MASK = 0x9F1F,
    KPAD_READ_NO_DATA = -1,
    KPAD_READ_NO_CONTROLLER = -2,
    KPAD_READ_NOT_READY = -3,
    KPAD_READ_LOCKED = -4,
    KPAD_READ_NOT_INITIALIZED = -5
};

static inline void clear_balance_weights(void) {
    s32 i;

    sBalanceCalibrationState = 0;
    sBalanceSampleCount = 0;
    sBalanceTgcCalibrating = FALSE;
    for (i = 0; i < 4; i++) {
        sBalanceSampleAverage[i] = 0.0;
        sBalanceSmoothedWeights[i] = 0.0;
    }
}

static inline void reset_cl_status(KPADInsideStatus* kp, KPADEXStatus* ex)
{
    if (kp->exResetReq) {
        kp->exResetReq = 0;
        ex->cl.lstick = Vec2_0;
        ex->cl.rstick = Vec2_0;
        ex->cl.ltrigger = ex->cl.rtrigger = 0.0f;
        ex->cl.hold = ex->cl.trig = ex->cl.release = 0;
        kp->btn_cl_repeat_time = 0;
        kp->btn_cl_repeat_next = kp->btn_repeat_delay;
    }
}
void read_kpad_stick(KPADInsideStatus* kp, KPADUnifiedWpadStatus* uwp)
{
    void (*clamp)(Vec2*, s32, s32, s32, s32) = clamp_stick_circle;
    KPADEXStatus* ex = &kp->status.ex_status;
    s32 i;

    if (kp_stick_clamp_cross != 0) {
        clamp = clamp_stick_cross;
    }

    if (uwp->u.core.dev == WPAD_DEV_FREESTYLE && uwp->fmt >= WPAD_FMT_FREESTYLE && uwp->fmt <= WPAD_FMT_FREESTYLE_ACC_DPD) {
        if (kp->exResetReq) {
            kp->exResetReq = 0;
            ex->fs.stick = Vec2_0;
            ex->fs.acc.x = ex->fs.acc.z = 0.0f;
            ex->fs.acc.y = -1.0f;
            ex->fs.acc_value = 1.0f;
            ex->fs.acc_speed = 0.0f;
        }
        clamp(&ex->fs.stick, uwp->u.fs.fsStickX, uwp->u.fs.fsStickY, kp_fs_fstick_min, kp_fs_fstick_max);
    } else if (uwp->u.core.dev == WPAD_DEV_CLASSIC && uwp->fmt >= WPAD_FMT_CLASSIC && uwp->fmt <= WPAD_FMT_CLASSIC_ACC_DPD) {
        reset_cl_status(kp, ex);
        clamp(&ex->cl.lstick, uwp->u.cl.clLStickX, uwp->u.cl.clLStickY, kp_cl_stick_min, kp_cl_stick_max);
        clamp(&ex->cl.rstick, uwp->u.cl.clRStickX, uwp->u.cl.clRStickY, kp_cl_stick_min, kp_cl_stick_max);
        clamp_trigger(&ex->cl.ltrigger, uwp->u.cl.clTriggerL, kp_cl_trigger_min, kp_cl_trigger_max);
        clamp_trigger(&ex->cl.rtrigger, uwp->u.cl.clTriggerR, kp_cl_trigger_min, kp_cl_trigger_max);
    } else if (uwp->u.core.dev == WPAD_DEV_GUITAR && uwp->fmt == WPAD_FMT_GUITAR) {
        reset_cl_status(kp, ex);
        clamp(&ex->cl.lstick, uwp->u.cl.clLStickX, uwp->u.cl.clLStickY, kp_cl_stick_min, kp_cl_stick_max);
        clamp_trigger(&ex->cl.rstick.x, uwp->u.cl.clRStickX, kp_gtr_stick_min, kp_gtr_stick_max);
        clamp_trigger(&ex->cl.rstick.y, uwp->u.cl.clRStickY, kp_gtr_stick_min, kp_gtr_stick_max);
        clamp_trigger(&ex->cl.ltrigger, uwp->u.cl.clTriggerL, kp_tr_trigger_min, kp_tr_trigger_max);
        clamp_trigger(&ex->cl.rtrigger, uwp->u.cl.clTriggerR, kp_tr_trigger_min, kp_tr_trigger_max);
    } else if (uwp->u.core.dev == WPAD_DEV_TRAIN && uwp->fmt == WPAD_FMT_TRAIN) {
        reset_cl_status(kp, ex);
        ex->cl.lstick.x = ex->cl.lstick.y = 0.0f;
        ex->cl.rstick.x = ex->cl.rstick.y = 0.0f;
        clamp_trigger(&ex->cl.ltrigger, uwp->u.tr.brake, kp_tr_trigger_min, kp_tr_trigger_max);
        clamp_trigger(&ex->cl.rtrigger, uwp->u.tr.mascon, kp_tr_trigger_min, kp_tr_trigger_max);
    } else if (uwp->u.core.dev == WPAD_DEV_BALANCE_CHECKER && uwp->fmt == WPAD_FMT_BALANCE_CHECKER) {
        if (kp->exResetReq) {
            kp->exResetReq = 0;
            clear_balance_weights();
        }
        if (WBCGetBatteryLevel(uwp->u.bl.battery) == 0) {
            ex->bl.weight_err = -1;
        } else if (sBalanceCalibrationState < 3) {
            ex->bl.weight_err = -2;
        } else if (uwp->u.bl.temp == 127 || uwp->u.bl.temp == -128) {
            ex->bl.weight_err = -3;
        } else {
            ex->bl.weight_err = WBCRead(&uwp->u.bl, ex->bl.weight, 4);
            if (ex->bl.weight_err >= 0) {
                for (i = 0; i < 4; i++) {
                    sBalanceSmoothedWeights[i] = (sBalanceAverageWindow * sBalanceSmoothedWeights[i] + ex->bl.weight[i])
                                     / (1.0f + sBalanceAverageWindow);
                }
                if (sBalanceTgcCalibrating) {
                    sBalanceSampleCount++;
                    for (i = 0; i < 4; i++) {
                        sBalanceSampleAverage[i] = ((sBalanceSampleCount - 1) * sBalanceSampleAverage[i] + ex->bl.weight[i])
                                           / sBalanceSampleCount;
                    }
                    if (sBalanceSampleCount == sBalanceAverageWindow) {
                        sBalanceTgcCalibrating = 0;
                        ex->bl.weight_err = WBCGetTGCWeight(sBalanceSampleAverage[0] + sBalanceSampleAverage[1]
                                                                     + sBalanceSampleAverage[2] + sBalanceSampleAverage[3],
                                                                 &sBalanceTgcWeight, &uwp->u.bl);
                        if (sBalanceTgcWeight < -0.5) {
                            ex->bl.weight_err = -4;
                        }
                    }
                }
            }
            for (i = 0; i < 4; i++) {
                ex->bl.weight_ave[i] = sBalanceSmoothedWeights[i];
            }
            ex->bl.tgc_weight = sBalanceTgcWeight;
            ex->bl.tgc_weight_err = sBalanceTgcCalibrating;
        }
    }
}

s32 KPADiRead(s32 chan, KPADStatus samplingBufs[], u32 length, s32* pError, BOOL preserveOutput);

static KPADUnifiedWpadStatus* get_unified_sample(KPADInsideStatus* kp, s32 index) {
    return index >= 16 ? &kp->extraRingBuf[index - 16] : &kp->uniRingBuf[index];
}

#pragma push
#pragma legacy_struct_alignment off

s32 KPADRead(s32 chan, KPADStatus samplingBufs[], u32 length) {
    return KPADiRead(chan, samplingBufs, length, NULL, FALSE);
}

s32 KPADiRead(s32 chan, KPADStatus samplingBufs[], u32 length, s32* pError, BOOL preserveOutput) {
    KPADTmpStatus* tp = (KPADTmpStatus*)samplingBufs;
    KPADTmpStatus* readTp;
    KPADStatus savedStatus;
    KPADInsideStatus* kp = &inside_kpads[chan];
    KPADUnifiedWpadStatus* uwp;
    s32 error = 0;
    s32 index;
    s32 copyCount;
    u32 returnCount = 0;
    u32 bufCount;
    BOOL enabled;
    BOOL corrupted;
    BOOL deviceChanged;
    u32 coreButton;
    u32 fsButton;
    u32 clButton;
    u32 device;

    if (!sKpadInitialized) {
        error = KPAD_READ_NOT_INITIALIZED;
        goto finish;
    }
    if (WPADGetStatus() != WPAD_STATE_SETUP) {
        error = KPAD_READ_NOT_READY;
        goto finish;
    }
    enabled = OSDisableInterrupts();
    if (kp->readLocked) {
        OSRestoreInterrupts(enabled);
        error = KPAD_READ_LOCKED;
        goto finish;
    }
    kp->readLocked = TRUE;
    if (WPADProbe(chan, NULL) == WPAD_ERR_NO_CONTROLLER) {
        reset_kpad(kp);
        if (kp->dpd_ctrl_callback && kp->dpdPreCallbackDone && !kp->dpdPostCallbackDone) {
            KPADiControlDpdCallback(chan, WPAD_ERR_NONE);
        }
        kp->readLocked = FALSE;
        OSRestoreInterrupts(enabled);
        error = KPAD_READ_NO_CONTROLLER;
        goto finish;
    }
    OSRestoreInterrupts(enabled);
    if (kp->resetReq) {
        kp->status.wpad_err = WPAD_ERR_INVALID;
        reset_kpad(kp);
    }
    savedStatus = *samplingBufs;
    if (kp->bufCount <= 1 || !samplingBufs || !length) {
        goto unlock;
    }
    enabled = OSDisableInterrupts();
    bufCount = kp->bufCount;
    if (bufCount > length) {
        bufCount = length;
    }
    copyCount = bufCount;
    corrupted = FALSE;
    deviceChanged = FALSE;
    index = kp->bufIdx - 1;
    if (index < 0) {
        index += 16 + kp->extraRingCount;
    }
    uwp = get_unified_sample(kp, index);
    do {
        if (corrupted) {
            if (uwp->u.core.err == WPAD_ERR_NONE) {
                uwp->u.core.err = WPAD_ERR_CORRUPTED;
                corrupted = FALSE;
            } else if (uwp->u.core.err == WPAD_ERR_BUSY) {
                corrupted = FALSE;
            }
        } else if (uwp->u.core.err == WPAD_ERR_CORRUPTED) {
            corrupted = TRUE;
        }
        if (kp->status.dev_type != uwp->u.core.dev) {
            deviceChanged = TRUE;
        }
        if (deviceChanged) {
            uwp->u.core.err = WPAD_ERR_INVALID;
        }
        if (--index < 0) {
            index += 16 + kp->extraRingCount;
        }
        uwp = get_unified_sample(kp, index);
    } while (--copyCount > 0);
    returnCount = 1;
    kp->bufCount = 1;
    if (bufCount > 1) {
        returnCount = bufCount - 1;
    }
    copyCount = returnCount;
    tp += returnCount;
    index = kp->bufIdx - returnCount - 1;
    if (index < 0) {
        index += 16 + kp->extraRingCount;
    }
    do {
        tp[-1].w = *get_unified_sample(kp, index);
        --tp;
        if (++index >= 16 + kp->extraRingCount) {
            index = 0;
        }
    } while (--copyCount > 0);
    OSRestoreInterrupts(enabled);
    copyCount = returnCount;
    tp = (KPADTmpStatus*)samplingBufs + copyCount;
    readTp = tp;
    if (kp->buttonProcessingMode == 1) {
        do {
            --readTp;
            uwp = &readTp->w;
            if (kp->status.dev_type != uwp->u.core.dev) {
                kp->status.dev_type = uwp->u.core.dev;
                kp->exResetReq = TRUE;
            }
            kp->status.wpad_err = uwp->u.core.err;
            coreButton = fsButton = clButton = KPAD_BUTTON_MASK;
            kp->status.data_format = uwp->fmt;
            device = kp->status.dev_type;
            switch (uwp->u.core.err) {
            case WPAD_ERR_NONE:
                if (device == WPAD_DEV_FREESTYLE) {
                    fsButton = uwp->u.fs.button;
                    clButton = 0;
                } else if (device == WPAD_DEV_CLASSIC || device == WPAD_DEV_TRAIN || device == WPAD_DEV_GUITAR) {
                    fsButton = 0;
                    clButton = uwp->u.cl.clButton;
                } else {
                    fsButton = clButton = 0;
                }
                coreButton = uwp->u.core.button & KPAD_CORE_BUTTON_MASK;
                break;
            case WPAD_ERR_CORRUPTED:
                fsButton = clButton = 0;
            case WPAD_ERR_BUSY:
                coreButton = uwp->u.core.button & KPAD_CORE_BUTTON_MASK;
                break;
            }
            if (coreButton == KPAD_BUTTON_MASK) {
                coreButton = kp->status.hold & KPAD_CORE_BUTTON_MASK;
            }
            if (fsButton == KPAD_BUTTON_MASK) {
                fsButton = kp->status.hold;
            }
            if (clButton == KPAD_BUTTON_MASK) {
                clButton = kp->status.ex_status.cl.hold;
            }
            read_kpad_button(kp, device, 1, coreButton, fsButton, clButton);
            switch (uwp->u.core.err) {
            case WPAD_ERR_NONE:
                read_kpad_stick(kp, uwp);
            case WPAD_ERR_CORRUPTED:
                read_kpad_acc(kp, uwp);
                read_kpad_dpd(kp, uwp);
                break;
            default:
                if (!preserveOutput) {
                    kp->status.dpd_valid_fg = FALSE;
                }
                break;
            }
            readTp->k = kp->status;
        } while (--copyCount > 0);
    } else {
        coreButton = fsButton = clButton = KPAD_BUTTON_MASK;
        do {
            --readTp;
            uwp = &readTp->w;
            if (kp->status.dev_type != uwp->u.core.dev) {
                kp->status.dev_type = uwp->u.core.dev;
                kp->exResetReq = TRUE;
            }
            device = uwp->u.core.dev;
            switch (uwp->u.core.err) {
            case WPAD_ERR_NONE:
                if (device == WPAD_DEV_FREESTYLE) {
                    fsButton = uwp->u.fs.button;
                    clButton = 0;
                } else if (device == WPAD_DEV_CLASSIC || device == WPAD_DEV_TRAIN || device == WPAD_DEV_GUITAR) {
                    fsButton = 0;
                    clButton = uwp->u.cl.clButton;
                } else {
                    fsButton = clButton = 0;
                }
            case WPAD_ERR_CORRUPTED:
            case WPAD_ERR_BUSY:
                coreButton = uwp->u.core.button & KPAD_CORE_BUTTON_MASK;
                break;
            }
        } while (--copyCount > 0);
        if (coreButton == KPAD_BUTTON_MASK) {
            coreButton = kp->status.hold & KPAD_CORE_BUTTON_MASK;
        }
        if (fsButton == KPAD_BUTTON_MASK) {
            fsButton = kp->status.hold;
        }
        if (clButton == KPAD_BUTTON_MASK) {
            clButton = kp->status.ex_status.cl.hold;
        }
        read_kpad_button(kp, device, returnCount, coreButton, fsButton, clButton);
        copyCount = returnCount;
        do {
            --tp;
            uwp = &tp->w;
            kp->status.wpad_err = uwp->u.core.err;
            kp->status.data_format = uwp->fmt;
            switch (uwp->u.core.err) {
            case WPAD_ERR_NONE:
                read_kpad_stick(kp, uwp);
            case WPAD_ERR_CORRUPTED:
                read_kpad_acc(kp, uwp);
                read_kpad_dpd(kp, uwp);
                break;
            default:
                if (!preserveOutput) {
                    kp->status.dpd_valid_fg = FALSE;
                }
                break;
            }
            tp->k = kp->status;
        } while (--copyCount > 0);
    }
unlock:
    kp->readLocked = FALSE;
finish:
    if (!returnCount) {
        if (!error) {
            if (preserveOutput) {
                *samplingBufs = savedStatus;
            }
            error = KPAD_READ_NO_DATA;
        } else if (error == KPAD_READ_NO_CONTROLLER && preserveOutput) {
            samplingBufs->dev_type = WPAD_DEV_NOT_FOUND;
            samplingBufs->data_format = WPAD_FMT_CORE;
            samplingBufs->wpad_err = WPAD_ERR_NO_CONTROLLER;
        }
    }
    if (pError) {
        *pError = error;
    }
    return returnCount;
}

#pragma pop


void KPADSetObjInterval(f32 interval) {
    BOOL enabled = OSDisableInterrupts();
    kp_obj_interval = interval;
    kp_err_dist_min = interval / 0.383864f;
    kp_dist_vv1 = interval / 0.383864f;
    OSRestoreInterrupts(enabled);
}

static void calc_dpd2pos_scale(KPADInsideStatus* kp) {
    f32 scale;
    f32 sx, sy;
    sx = 1.0f;
    sy = (f32)KPAD_DPD_RESO_WY / (f32)KPAD_DPD_RESO_WX;

    scale = sqrt(sx * sx + sy * sy);

    if (kp->center_org.x < 0.0f) {
        sx += kp->center_org.x;
    } else {
        sx -= kp->center_org.x;
    }

    if (kp->center_org.y < 0.0f) {
        sy += kp->center_org.y;
    } else {
        sy -= kp->center_org.y;
    }

    kp->dpd2pos_scale = scale / ((sx < sy) ? sx : sy);
}

void KPADReset(void) {
    s32 chan;
    KPADSetObjInterval(kp_obj_interval);

    chan = WPAD_MAX_CONTROLLERS - 1;
    do {
        if (WPADGetStatus() == WPAD_STATE_SETUP) {
            WPADStopMotor(chan);
        }
        inside_kpads[chan].resetReq = TRUE;
    } while (--chan >= 0);
}

void KPADInit(void) {
    KPADInitEx(NULL, 0);
}

void KPADInitEx(KPADUnifiedWpadStatus* pBuffer, u32 length) {
    s32 chan;
    KPADInsideStatus* kp;
    u32 index;

    if (!sKpadInitialized) {
        WPADInit();
        memset(inside_kpads, 0, sizeof(inside_kpads));
        kp_err_dist_max = 1.0f + (f32)WPADGetDpdSensitivity();
        chan = 0;
        do {
            kp = &inside_kpads[chan];
            kp->appConnectCallback = WPADSetConnectCallback(chan, KPADiConnectCallback);
            kp->appSamplingCallback = WPADSetSamplingCallback(chan, KPADiSamplingCallback);
            kp->dpdHardwareEnabled = FALSE;
            sBalanceCommandPending = FALSE;
            sBalanceControlSucceeded = FALSE;
            sBalanceCalibrationResult = 0;
            sBalanceCalibrationState = 0;
            sBalanceSampleCount = 0;
            sBalanceTgcCalibrating = FALSE;
            sBalanceSampleAverage[0] = 0.0;
            sBalanceSmoothedWeights[0] = 0.0;
            sBalanceSampleAverage[1] = 0.0;
            sBalanceSmoothedWeights[1] = 0.0;
            sBalanceSampleAverage[2] = 0.0;
            sBalanceSmoothedWeights[2] = 0.0;
            sBalanceSampleAverage[3] = 0.0;
            sBalanceSmoothedWeights[3] = 0.0;
            kp->dpdEnabled = TRUE;
            kp->dpdCmd = WPADGetDpdFormat(chan);
            kp->status.dev_type = WPAD_DEV_NOT_FOUND;
            kp->status.data_format = WPAD_FMT_CORE;
            kp->dist_org = idist_org;
            kp->accXY_nrm_hori = iaccXY_nrm_hori;
            kp->sec_nrm_hori = isec_nrm_hori;
            kp->center_org = icenter_org;
            calc_dpd2pos_scale(kp);
            kp->pos_play_radius = kp->hori_play_radius = kp->dist_play_radius = kp->acc_play_radius = 0.0f;
            kp->pos_sensitivity = kp->hori_sensitivity = kp->dist_sensitivity = kp->acc_sensitivity = 1.0f;
            kp->pos_play_mode = kp->hori_play_mode = kp->dist_play_mode = kp->acc_play_mode = KPAD_PLAY_MODE_LOOSE;
            KPADSetBtnRepeat(chan, 0.0f, 0.0f);
            kp->buttonProcessingMode = 0;
            KPADEnableAimingMode(chan);
            kp->accGravityUpdateReq = TRUE;
            kp->fsAccRevise = FALSE;
            if (length && pBuffer) {
                kp->extraRingCount = length / WPAD_MAX_CONTROLLERS;
                kp->extraRingBuf = pBuffer + chan * kp->extraRingCount;
            } else {
                kp->extraRingCount = 0;
                kp->extraRingBuf = NULL;
            }
            for (index = 0; index < 16; ++index) {
                kp->uniRingBuf[index].u.core.err = WPAD_ERR_NO_CONTROLLER;
            }
            for (index = 0; index < kp->extraRingCount; ++index) {
                kp->extraRingBuf[index].u.core.err = WPAD_ERR_NO_CONTROLLER;
            }
        } while (++chan < WPAD_MAX_CONTROLLERS);
        WPADSetCallbackByKPAD(TRUE);
        KPADReset();
        MTXRowCol(kp_fs_rot, 0, 0) = 1;
        MTXRowCol(kp_fs_rot, 0, 1) = 0;
        MTXRowCol(kp_fs_rot, 0, 2) = 0;
        MTXRowCol(kp_fs_rot, 0, 3) = 0;
        MTXRowCol(kp_fs_rot, 1, 0) = 0;
        MTXRowCol(kp_fs_rot, 1, 1) = (f32)cos(MTXDegToRad(kp_fs_revise_deg));
        MTXRowCol(kp_fs_rot, 1, 2) = (f32)-sin(MTXDegToRad(kp_fs_revise_deg));
        MTXRowCol(kp_fs_rot, 1, 3) = 0;
        MTXRowCol(kp_fs_rot, 2, 0) = 0;
        MTXRowCol(kp_fs_rot, 2, 1) = (f32)sin(MTXDegToRad(kp_fs_revise_deg));
        MTXRowCol(kp_fs_rot, 2, 2) = (f32)cos(MTXDegToRad(kp_fs_revise_deg));
        MTXRowCol(kp_fs_rot, 2, 3) = 0;
        sKpadInitialized = TRUE;
        OSRegisterVersion(__KPADVersion);
    }
}

void KPADiConnectCallback(s32 chan, s32 result) {
    KPADInsideStatus* kp = &inside_kpads[chan];
    u32 index;

    if (result == WPAD_ERR_NONE) {
        kp->lastDev = WPAD_DEV_CORE;
        kp->accGravityUpdateReq = TRUE;
        WPADSetCallbackByKPAD(FALSE);
        WPADSetSamplingCallback(chan, KPADiSamplingCallback);
        WPADSetCallbackByKPAD(TRUE);
        kp->dpdHardwareEnabled = FALSE;
        kp->dpdCmd = 0;
        if (chan == 3) {
            sBalanceCommandPending = FALSE;
            sBalanceControlSucceeded = FALSE;
            sBalanceCalibrationResult = 0;
            sBalanceCalibrationState = 0;
            sBalanceSampleCount = 0;
            sBalanceTgcCalibrating = FALSE;
            sBalanceSampleAverage[0] = 0.0;
            sBalanceSmoothedWeights[0] = 0.0;
            sBalanceSampleAverage[1] = 0.0;
            sBalanceSmoothedWeights[1] = 0.0;
            sBalanceSampleAverage[2] = 0.0;
            sBalanceSmoothedWeights[2] = 0.0;
            sBalanceSampleAverage[3] = 0.0;
            sBalanceSmoothedWeights[3] = 0.0;
        }
    } else {
        for (index = 0; index < 16; ++index) {
            kp->uniRingBuf[index].u.core.err = WPAD_ERR_NO_CONTROLLER;
        }
        for (index = 0; index < kp->extraRingCount; ++index) {
            kp->extraRingBuf[index].u.core.err = WPAD_ERR_NO_CONTROLLER;
        }
    }
    if (kp->appConnectCallback) {
        kp->appConnectCallback(chan, result);
    }
}

WPADCallback KPADSetConnectCallback(s32 chan, WPADCallback callback) {
    BOOL enabled = OSDisableInterrupts();
    WPADCallback previous = inside_kpads[chan].appConnectCallback;
    inside_kpads[chan].appConnectCallback = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

void KPADiBalanceControlCallback(s32 chan, s32 result) {
    sBalanceControlSucceeded = result == WPAD_ERR_NONE;
    sBalanceCommandPending = FALSE;
}

void KPADiBalanceCalibrationCallback(s32 chan, s32 result) {
    sBalanceCalibrationState = result == WPAD_ERR_NONE;
    sBalanceCommandPending = FALSE;
}

void KPADDisableDPD(s32 chan) {
    inside_kpads[chan].dpdEnabled = FALSE;
}

void KPADEnableDPD(s32 chan) {
    inside_kpads[chan].dpdEnabled = TRUE;
}

void KPADiControlDpdCallback(s32 chan, s32 result) {
    KPADInsideStatus* kp = &inside_kpads[chan];

    if (result == WPAD_ERR_NONE) {
        if (kp->dpd_ctrl_callback && !kp->dpdPostCallbackDone) {
            kp->dpdPostCallbackDone = TRUE;
            kp->dpd_ctrl_callback(chan, 1);
            kp->dpdPreCallbackDone = FALSE;
        }
    }
    kp->dpdHardwareEnabled = WPADIsDpdEnabled(chan);
    kp->dpdIssued = FALSE;
}

void KPADiSamplingCallback(s32 chan)
{
    KPADInsideStatus* kp = &inside_kpads[chan];
    KPADUnifiedWpadStatus* uwp;
    u32 type;
    WPADAcc acc = { 1, 1, 1 };
    u32 idx;
    s32 mode;
    u32 dpdCmd;
    u8* ent;
    s32 i;

    if (WPADProbe(chan, &type) != -1) {
        idx = kp->bufIdx;
        if (idx >= kp->extraRingCount + 16) {
            idx = 0;
        }
        if (idx >= 16) {
            uwp = &kp->extraRingBuf[idx - 16];
        } else {
            uwp = &kp->uniRingBuf[idx];
        }
        WPADRead(chan, &uwp->u);
        uwp->fmt = WPADGetDataFormat(chan);
        kp->bufIdx = idx + 1;
        if (kp->bufCount < kp->extraRingCount + 16) {
            kp->bufCount++;
        }

        if (type != kp->lastDev && type == WPAD_DEV_FREESTYLE) {
            kp->accGravityUpdateReq = 1;
        }
        if (kp->accGravityUpdateReq) {
            WPADGetAccGravityUnit(chan, 0, &acc);
            if (acc.x * acc.y * acc.z != 0) {
                kp->acc_scale_x = 1.0f / (f32)acc.x;
                kp->acc_scale_y = 1.0f / (f32)acc.y;
                kp->acc_scale_z = 1.0f / (f32)acc.z;
            } else {
                kp->acc_scale_x = 0.01f;
                kp->acc_scale_y = 0.01f;
                kp->acc_scale_z = 0.01f;
            }
            WPADGetAccGravityUnit(chan, 1, &acc);
            if (acc.x * acc.y * acc.z != 0) {
                kp->fs_acc_scale_x = 1.0f / (f32)acc.x;
                kp->fs_acc_scale_y = 1.0f / (f32)acc.y;
                kp->fs_acc_scale_z = 1.0f / (f32)acc.z;
            } else {
                kp->fs_acc_scale_x = 0.005f;
                kp->fs_acc_scale_y = 0.005f;
                kp->fs_acc_scale_z = 0.005f;
            }
            kp->accGravityUpdateReq = 0;
        }

        if (kp->aimReq) {
            f32 ofs;

            if (kp->aimEnabled) {
                if (WPADGetSensorBarPosition() == 1) {
                    ofs = 0.2f;
                } else {
                    ofs = -0.2f;
                }
            } else {
                ofs = 0.0f;
            }
            kp->center_org.x = 0.0f;
            kp->center_org.y = -ofs;
            calc_dpd2pos_scale(kp);
            kp->aimReq = 0;
        }

        do {
            switch (type) {
            case WPAD_DEV_CORE:
            case WPAD_DEV_FUTURE:
            case WPAD_DEV_NOT_SUPPORTED:
            case WPAD_DEV_UNKNOWN:
                mode = 0;
                break;
            case WPAD_DEV_FREESTYLE:
                mode = 2;
                break;
            case WPAD_DEV_CLASSIC:
                mode = 4;
                break;
            case WPAD_DEV_GUITAR:
                mode = 6;
                break;
            case WPAD_DEV_BALANCE_CHECKER:
                mode = 8;
                break;
            case WPAD_DEV_TRAIN:
                mode = 10;
                break;
            default:
                continue;
            }
            if (kp->dpdEnabled) {
                mode++;
            }

            if (WPADIsDpdEnabled(chan)) {
                dpdCmd = kp->dpdCmd;
            } else {
                dpdCmd = 0;
            }
            ent = kp_mode_table[mode];
            if (dpdCmd != ent[0]) {
                if (kp->dpd_ctrl_callback != NULL && !kp->dpdPreCallbackDone) {
                    kp->dpdPreCallbackDone = 1;
                    kp->dpd_ctrl_callback(chan, 0);
                    kp->dpdPostCallbackDone = 0;
                }
                if (!kp->dpdIssued) {
                    kp->dpdIssued = 1;
                    if (WPADControlDpd(chan, ent[0], KPADiControlDpdCallback) == 0) {
                        kp->dpdCmd = ent[0];
                    }
                }
            } else if (uwp->fmt != ent[1]) {
                WPADSetDataFormat(chan, ent[1]);
            }

            if (type == WPAD_DEV_BALANCE_CHECKER) {
                if (!sBalanceCommandPending && !sBalanceControlSucceeded) {
                    if (WPADControlBLC(chan, 0xAA, KPADiBalanceControlCallback) == 0) {
                        sBalanceCommandPending = 1;
                    }
                } else if (sBalanceControlSucceeded && !sBalanceCalibrationResult) {
                    if (!sBalanceCommandPending) {
                        if (WBCSetupCalibration() == 0) {
                            sBalanceCommandPending = 1;
                        }
                    } else {
                        sBalanceCalibrationResult = WBCGetCalibrationStatus();
                        sBalanceCommandPending = !sBalanceCalibrationResult;
                    }
                } else if (!sBalanceCommandPending && sBalanceCalibrationResult && sBalanceCalibrationState < 3) {
                    switch (sBalanceCalibrationState) {
                    case 0:
                        if (WPADControlBLC(chan, 0, KPADiBalanceCalibrationCallback) == 0) {
                            sBalanceCommandPending = 1;
                        }
                        break;
                    case 1:
                        if (uwp->u.bl.temp == 127 || uwp->u.bl.temp == -128) {
                            clear_balance_weights();
                        } else if (++sBalanceSampleCount > sBalanceSettlingSamples) {
                            sBalanceCalibrationState = 2;
                            sBalanceSampleCount = 0;
                        }
                        break;
                    case 2:
                        if (uwp->u.core.err == 0) {
                            sBalanceSampleCount++;
                            for (i = 0; i < 4; i++) {
                                sBalanceSampleAverage[i] = ((sBalanceSampleCount - 1) * sBalanceSampleAverage[i] + uwp->u.bl.press[i])
                                                   / sBalanceSampleCount;
                            }
                            if (sBalanceSampleCount > sBalanceAverageWindow) {
                                sBalanceCalibrationState = 3;
                                WBCSetZEROPoint(sBalanceSampleAverage, 4);
                            }
                        }
                        break;
                    }
                }
            }
        } while (0);
    }
    if (kp->appSamplingCallback != NULL) {
        kp->appSamplingCallback(chan);
    }
}
