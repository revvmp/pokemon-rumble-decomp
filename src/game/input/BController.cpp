#include <game/input/BController.h>

#include <game/core/MathUtil.h>
#include <nw4r/math/math_triangular.h>

static const s32 LOST_FRAMES = 10;
static const s32 REPEAT_DELAY = 20;
static const s32 REPEAT_INTERVAL = 4;

BController::BController() :
    mChannel(0),
    mConnected(false),
    mLostCount(0),
    mStickLength(0.0f),
    mStickAngle(0.0f),
    mHold(0),
    mTrig(0),
    mRelease(0),
    mRepeat(0),
    mRepeatButton(0),
    mRepeatWait(0) {}

bool BController::IsLost(bool detected) {
    if (detected) {
        mLostCount = 0;
        return false;
    }
    if (!mConnected) {
        return true;
    }
    mLostCount++;
    return mLostCount >= LOST_FRAMES;
}

void BController::SetButton(u32 button) {
    mConnected = true;
    UpdateButton(button);
}

void BController::SetDisconnected() {
    mConnected = false;
    mLostCount = 0;
    mStickLength = 0.0f;
    mStickAngle = 0.0f;
    if (++mDisconnectSerial <= 1) {
        mDisconnectSerial = 1;
    }
    UpdateButton(0);
}

void BController::SetStickFromCross() {
    switch (mHold & CROSS_MASK) {
    case CROSS_RIGHT:
        mStickLength = 1.0f;
        mStickAngle = 0.0f;
        break;
    case CROSS_UP | CROSS_RIGHT:
        mStickLength = 1.0f;
        mStickAngle = 45.0f;
        break;
    case CROSS_UP:
        mStickLength = 1.0f;
        mStickAngle = 90.0f;
        break;
    case CROSS_UP | CROSS_LEFT:
        mStickLength = 1.0f;
        mStickAngle = 135.0f;
        break;
    case CROSS_LEFT:
        mStickLength = 1.0f;
        mStickAngle = 180.0f;
        break;
    case CROSS_DOWN | CROSS_LEFT:
        mStickLength = 1.0f;
        mStickAngle = 225.0f;
        break;
    case CROSS_DOWN:
        mStickLength = 1.0f;
        mStickAngle = 270.0f;
        break;
    case CROSS_DOWN | CROSS_RIGHT:
        mStickLength = 1.0f;
        mStickAngle = 315.0f;
        break;
    default:
        mStickLength = 0.0f;
        mStickAngle = 0.0f;
        break;
    }
}

void BController::SetStick(const nw4r::math::VEC2& rStick) {
    mStickAngle = nw4r::math::Atan2Deg(rStick.y, rStick.x);
    mStickLength = Min(Max(0.0f, nw4r::math::VEC2Len(&rStick)), 1.0f);
}

void BController::UpdateButton(u32 button) {
    u32 prev = mHold;
    mHold = button;
    mTrig = button & (prev ^ button);
    mRelease = prev & (prev ^ button);

    if (mTrig != 0) {
        mRepeatButton = mTrig;
        mRepeat = 0;
        mRepeatWait = REPEAT_DELAY;
        return;
    }

    if (mRepeatButton == 0) {
        return;
    }

    if (button & mRepeatButton) {
        if (--mRepeatWait == 0) {
            mRepeatWait = REPEAT_INTERVAL;
            mRepeat = mRepeatButton;
        } else {
            mRepeat = 0;
        }
    } else {
        mRepeatButton = 0;
        mRepeat = 0;
    }
}

