#include "revolution/pad.h"
#include "math.h"

static const PADClampRegion ClampRegion = {30, 180, 15, 72, 40, 15, 59, 31, 56, 44};

static const PADClampRegion MarginClampRegion = {30, 180, 15, 72, 47, 15, 59, 37, 62, 50};

static const PADClampRegion NoMarginClampRegion = {0, 180, 0, 87, 62, 0, 74, 52, 80, 68};

static void ClampStick(s8* px, s8* py, s8 max, s8 xy, s8 min) {
    int x = *px;
    int y = *py;
    int signX;
    int signY;
    int d;

    if (0 <= x) {
        signX = 1;
    } else {
        signX = -1;
        x = -x;
    }

    if (0 <= y) {
        signY = 1;
    } else {
        signY = -1;
        y = -y;
    }

    if (x <= min) {
        x = 0;
    } else {
        x -= min;
    }

    if (y <= min) {
        y = 0;
    } else {
        y -= min;
    }

    if (x == 0 && y == 0) {
        *px = *py = 0;
        return;
    }

    if (xy * y <= xy * x) {
        d = xy * x + (max - xy) * y;
        if (xy * max < d) {
            x = (s8)(xy * max * x / d);
            y = (s8)(xy * max * y / d);
        }
    } else {
        d = xy * y + (max - xy) * x;
        if (xy * max < d) {
            x = (s8)(xy * max * x / d);
            y = (s8)(xy * max * y / d);
        }
    }

    *px = (s8)(signX * x);
    *py = (s8)(signY * y);
}

static void ClampCircle(s8* px, s8* py, s8 radius, s8 min) {
    int x = *px;
    int y = *py;
    int squared;
    int length;

    if (-min < x && x < min) {
        x = 0;
    } else if (0 < x) {
        x -= min;
    } else {
        x += min;
    }

    if (-min < y && y < min) {
        y = 0;
    } else if (0 < y) {
        y -= min;
    } else {
        y += min;
    }

    squared = x * x + y * y;
    if (radius * radius < squared) {
        length = sqrtf(squared);
        x = (x * radius) / length;
        y = (y * radius) / length;
    }

    *px = (s8)x;
    *py = (s8)y;
}

static void ClampTrigger(u8* trigger, u8 min, u8 max) {
    if (*trigger <= min) {
        *trigger = 0;
    } else {
        if (max < *trigger) {
            *trigger = max;
        }
        *trigger -= min;
    }
}

void PADClamp(PADStatus* status) {
    int i;

    for (i = 0; i < PAD_MAX_CONTROLLERS; i++, status++) {
        if (status->err == PAD_ERR_NONE) {
            ClampStick(&status->stickX, &status->stickY, ClampRegion.maxStick, ClampRegion.xyStick, ClampRegion.minStick);
            ClampStick(&status->substickX, &status->substickY, ClampRegion.maxSubstick, ClampRegion.xySubstick, ClampRegion.minSubstick);
            ClampTrigger(&status->triggerLeft, ClampRegion.minTrigger, ClampRegion.maxTrigger);
            ClampTrigger(&status->triggerRight, ClampRegion.minTrigger, ClampRegion.maxTrigger);
        }
    }
}

void PADClampCircle(PADStatus* status) {
    int i;

    for (i = 0; i < PAD_MAX_CONTROLLERS; i++, status++) {
        if (status->err == PAD_ERR_NONE) {
            ClampCircle(&status->stickX, &status->stickY, ClampRegion.radStick, ClampRegion.minStick);
            ClampCircle(&status->substickX, &status->substickY, ClampRegion.radSubstick, ClampRegion.minSubstick);
            ClampTrigger(&status->triggerLeft, ClampRegion.minTrigger, ClampRegion.maxTrigger);
            ClampTrigger(&status->triggerRight, ClampRegion.minTrigger, ClampRegion.maxTrigger);
        }
    }
}

void PADClamp2(PADStatus* status, u32 type) {
    int i;
    const PADClampRegion* region;

    for (i = 0; i < PAD_MAX_CONTROLLERS; i++, status++) {
        if (status->err == PAD_ERR_NONE) {
            if (type == PAD_STICK_CLAMP_OCTA_WITH_MARGIN) {
                region = &MarginClampRegion;
            } else {
                region = &NoMarginClampRegion;
            }

            ClampStick(&status->stickX, &status->stickY, region->maxStick, region->xyStick, region->minStick);
            ClampStick(&status->substickX, &status->substickY, region->maxSubstick, region->xySubstick, region->minSubstick);
        }
    }
}

void PADClampCircle2(PADStatus* status, u32 type) {
    int i;
    const PADClampRegion* region;

    for (i = 0; i < PAD_MAX_CONTROLLERS; i++, status++) {
        if (status->err == PAD_ERR_NONE) {
            if (type == PAD_STICK_CLAMP_CIRCLE_WITH_MARGIN) {
                region = &MarginClampRegion;
            } else {
                region = &NoMarginClampRegion;
            }

            ClampCircle(&status->stickX, &status->stickY, region->radStick, region->minStick);
            ClampCircle(&status->substickX, &status->substickY, region->radSubstick, region->minSubstick);
        }
    }
}

void PADClampTrigger(PADStatus* status, u32 type) {
    int i;
    const PADClampRegion* region;

    for (i = 0; i < PAD_MAX_CONTROLLERS; i++, status++) {
        if (status->err == PAD_ERR_NONE) {
            if (type == PAD_TRIGGER_FIXED_BASE) {
                region = &MarginClampRegion;
            } else {
                region = &NoMarginClampRegion;
            }

            ClampTrigger(&status->triggerLeft, region->minTrigger, region->maxTrigger);
            ClampTrigger(&status->triggerRight, region->minTrigger, region->maxTrigger);
        }
    }
}
