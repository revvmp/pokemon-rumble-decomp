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

KPADInsideStatus inside_kpads[4];
static Vec2 Vec2_0 = {0.0f, 0.0f};
static Mtx kp_fs_rot;
static f32 kp_fs_revise_deg = 24.0f;
static f64 sBalanceSampleAverage[4];
static f64 sBalanceSmoothedWeights[4];
static u16 sBalanceSampleCount;
static u8 sBalanceCalibrationState;
static u8 sBalanceTgcCalibrating;
static u8 sBalanceCalibrationResult;
static u8 sBalanceControlSucceeded;
static u8 sBalanceCommandPending;

void KPADiSamplingCallback(s32 chan);

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
