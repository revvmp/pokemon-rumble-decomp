#include <nw4r/ef/ef_postfield.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>

namespace nw4r {
namespace ef {

int GetTypePositionSphere(const math::VEC3& rPosition, u8 option) {
    math::VEC3 position = rPosition;
    if (position.y < -1.0f || position.y > 1.0f) {
        return 3;
    }
    if ((option & 1) && position.y < 0.0f) {
        return 3;
    }
    if ((option & 2) && position.y > 0.0f) {
        return 3;
    }
    if (position.x < -1.0f || position.x > 1.0f) {
        return 3;
    }
    if (position.z < -1.0f || position.z > 1.0f) {
        return 3;
    }
    f32 length = math::VEC3LenSq(&position);
    if (length > 1.0f) {
        return 3;
    }
    if (1.0f == length) {
        return 1;
    }
    if (option != 0 && 0.0f == position.y) {
        return 1;
    }
    return 2;
}

int GetTypePositionCube(const math::VEC3& rPosition, u8 option) {
    math::VEC3 position = rPosition;
    bool surface = false;
    if (position.x < -1.0f || position.x > 1.0f) {
        return 3;
    }
    if (1.0f == position.x || -1.0f == position.x) {
        surface = true;
    }
    if (position.y < -1.0f || position.y > 1.0f) {
        return 3;
    }
    if (1.0f == position.y || -1.0f == position.y) {
        surface = true;
    }
    if (position.z < -1.0f || position.z > 1.0f) {
        return 3;
    }
    if (surface || 1.0f == position.z || -1.0f == position.z) {
        return 1;
    }
    return 2;
}

int GetTypePositionCylinder(const math::VEC3& rPosition, u8 option) {
    math::VEC3 position = rPosition;
    if (position.y < -1.0f || position.y > 1.0f) {
        return 3;
    }
    if (position.x < -1.0f || position.x > 1.0f) {
        return 3;
    }
    if (position.z < -1.0f || position.z > 1.0f) {
        return 3;
    }
    f32 radius = position.x * position.x + position.z * position.z;
    if (radius > 1.0f) {
        return 3;
    }
    if (1.0f == radius) {
        return 1;
    }
    if (-1.0f == position.y || 1.0f == position.y) {
        return 1;
    }
    return 2;
}

int GetTypePositionCircle(const math::VEC3& rPosition, u8 axis) {
    math::VEC3 position;
    switch (axis) {
    case 0:
        position = rPosition;
        break;
    case 1:
        position.x = rPosition.x;
        position.y = rPosition.z;
        position.z = -rPosition.y;
        break;
    case 2:
        position.x = -rPosition.y;
        position.y = rPosition.x;
        position.z = rPosition.z;
        break;
    }
    if (position.y > 0.0f) {
        return 3;
    }
    if (position.y < 0.0f) {
        return 3;
    }
    if (position.x < -1.0f || position.x > 1.0f) {
        return 4;
    }
    if (position.z < -1.0f || position.z > 1.0f) {
        return 4;
    }
    if (position.x * position.x + position.z * position.z > 1.0f) {
        return 4;
    }
    return 1;
}

int GetTypePositionRectangle(const math::VEC3& rPosition, u8 axis) {
    math::VEC3 position;
    switch (axis) {
    case 0:
        position = rPosition;
        break;
    case 1:
        position.x = rPosition.x;
        position.y = rPosition.z;
        position.z = -rPosition.y;
        break;
    case 2:
        position.x = -rPosition.y;
        position.y = rPosition.x;
        position.z = rPosition.z;
        break;
    }
    if (position.y > 0.0f) {
        return 3;
    }
    if (position.y < 0.0f) {
        return 3;
    }
    if (position.x < -1.0f || position.x > 1.0f) {
        return 4;
    }
    if (position.z < -1.0f || position.z > 1.0f) {
        return 4;
    }
    return 1;
}

int GetTypePositionPlane(const math::VEC3& rPosition, u8 axis) {
    math::VEC3 position;
    switch (axis) {
    case 0:
        position = rPosition;
        break;
    case 1:
        position.x = rPosition.x;
        position.y = rPosition.z;
        position.z = -rPosition.y;
        break;
    case 2:
        position.x = -rPosition.y;
        position.y = rPosition.x;
        position.z = rPosition.z;
        break;
    }
    if (position.y > 0.0f) {
        return 2;
    }
    if (position.y < 0.0f) {
        return 3;
    }
    return 1;
}

f32 crossCheckPlane(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 crossing = 2.0f;
    switch (pInfo->mOption) {
    case 0:
        if (math::FAbs(rPrevious.y) < 1.1920929e-07f &&
            math::FAbs(rCurrent.y) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.y > 0.0f && rCurrent.y < 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->y *= -1.0f;
                pDirection->y *= -1.0f;
                pHit->y += 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        if (rPrevious.y < 0.0f && rCurrent.y > 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->y *= -1.0f;
                pDirection->y *= -1.0f;
                pHit->y -= 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        return 2.0f;
    case 1:
        if (math::FAbs(rPrevious.z) < 1.1920929e-07f &&
            math::FAbs(rCurrent.z) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.z > 0.0f && rCurrent.z < 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->z *= -1.0f;
                pDirection->z *= -1.0f;
                pHit->z += 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        if (rPrevious.z < 0.0f && rCurrent.z > 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->z *= -1.0f;
                pDirection->z *= -1.0f;
                pHit->z -= 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        return 2.0f;
    case 2:
        if (math::FAbs(rPrevious.x) < 1.1920929e-07f &&
            math::FAbs(rCurrent.x) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.x > 0.0f && rCurrent.x < 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->x *= -1.0f;
                pDirection->x *= -1.0f;
                pHit->x += 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        if (rPrevious.x < 0.0f && rCurrent.x > 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pInfo->mFlags & 2) {
                pVelocity->x *= -1.0f;
                pDirection->x *= -1.0f;
                pHit->x -= 1.1920929e-07f;
            }
            math::VEC3Transform(pWorldHit, &rMtx, pHit);
            return crossing;
        }
        return 2.0f;
    }
    return 2.0f;
}

f32 crossCheckRectangle(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 crossing = 2.0f;
    switch (pInfo->mOption) {
    case 0:
        if (math::FAbs(rPrevious.y) < 1.1920929e-07f &&
            math::FAbs(rCurrent.y) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.y > 0.0f && rCurrent.y < 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->x) < 1.0000001f &&
                math::FAbs(pHit->z) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.y < 0.0f && rCurrent.y > 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->x) < 1.0000001f &&
                math::FAbs(pHit->z) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    case 1:
        if (math::FAbs(rPrevious.z) < 1.1920929e-07f &&
            math::FAbs(rCurrent.z) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.z > 0.0f && rCurrent.z < 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->x) < 1.0000001f &&
                math::FAbs(pHit->y) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->z *= -1.0f;
                    pDirection->z *= -1.0f;
                    pHit->z += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.z < 0.0f && rCurrent.z > 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->x) < 1.0000001f &&
                math::FAbs(pHit->y) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->z *= -1.0f;
                    pDirection->z *= -1.0f;
                    pHit->z -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    case 2:
        if (math::FAbs(rPrevious.x) < 1.1920929e-07f &&
            math::FAbs(rCurrent.x) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.x > 0.0f && rCurrent.x < 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->y) < 1.0000001f &&
                math::FAbs(pHit->z) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->x *= -1.0f;
                    pDirection->x *= -1.0f;
                    pHit->x += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.x < 0.0f && rCurrent.x > 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (math::FAbs(pHit->y) < 1.0000001f &&
                math::FAbs(pHit->z) < 1.0000001f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->x *= -1.0f;
                    pDirection->x *= -1.0f;
                    pHit->x -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    }
    return 2.0f;
}

f32 crossCheckCircle(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 crossing = 2.0f;
    switch (pInfo->mOption) {
    case 0:
        if (math::FAbs(rPrevious.y) < 1.1920929e-07f &&
            math::FAbs(rCurrent.y) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.y > 0.0f && rCurrent.y < 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->x * pHit->x + pHit->z * pHit->z < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.y < 0.0f && rCurrent.y > 1.1920929e-07f) {
            crossing = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->x * pHit->x + pHit->z * pHit->z < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    case 1:
        if (math::FAbs(rPrevious.z) < 1.1920929e-07f &&
            math::FAbs(rCurrent.z) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.z > 0.0f && rCurrent.z < 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->x * pHit->x + pHit->y * pHit->y < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->z *= -1.0f;
                    pDirection->z *= -1.0f;
                    pHit->z += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.z < 0.0f && rCurrent.z > 1.1920929e-07f) {
            crossing = -rPrevious.z / pDirection->z;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->x * pHit->x + pHit->y * pHit->y < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->z *= -1.0f;
                    pDirection->z *= -1.0f;
                    pHit->z -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    case 2:
        if (math::FAbs(rPrevious.x) < 1.1920929e-07f &&
            math::FAbs(rCurrent.x) < 1.1920929e-07f) {
            return 2.0f;
        }
        if (rPrevious.x > 0.0f && rCurrent.x < 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->y * pHit->y + pHit->z * pHit->z < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->x *= -1.0f;
                    pDirection->x *= -1.0f;
                    pHit->x += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        if (rPrevious.x < 0.0f && rCurrent.x > 1.1920929e-07f) {
            crossing = -rPrevious.x / pDirection->x;
            math::VEC3Lerp(pHit, &rPrevious, &rCurrent, crossing);
            if (pHit->y * pHit->y + pHit->z * pHit->z < 1.0000002f) {
                if (pInfo->mFlags & 2) {
                    pVelocity->x *= -1.0f;
                    pDirection->x *= -1.0f;
                    pHit->x -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return crossing;
            }
            return 2.0f;
        }
        return 2.0f;
    }
    return 2.0f;
}

f32 crossCheckSphere(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 previousLength = math::VEC3LenSq(&rPrevious);
    f32 directionLength = math::VEC3LenSq(pDirection);
    f32 nearest = 2.0f;
    f32 dot = math::VEC3Dot(&rPrevious, pDirection);
    f32 discriminant = 4.0f * dot * dot - 4.0f * directionLength * (previousLength - 1.0f);
    if (discriminant < 0.0f) {
        return nearest;
    }
    f32 root = math::FSqrt(discriminant);
    f32 nearTime = (-2.0f * dot - root) / (2.0f * directionLength);
    f32 farTime = (-2.0f * dot + root) / (2.0f * directionLength);
    if (nearTime > 0.0f && nearTime < 1.0000001f) {
        math::VEC3Lerp(pHit, &rPrevious, &rCurrent, nearTime);
        if (!(pInfo->mOption == 1 && pHit->y < -1.1920929e-07f) &&
            !(pInfo->mOption == 2 && pHit->y > 1.1920929e-07f)) {
            nearest = nearTime;
        }
    }
    if (farTime > 0.0f && farTime < 1.0000001f && farTime < nearest) {
        math::VEC3 farHit;
        math::VEC3Lerp(&farHit, &rPrevious, &rCurrent, farTime);
        if (!(pInfo->mOption == 1 && farHit.y < -1.1920929e-07f) &&
            !(pInfo->mOption == 2 && farHit.y > 1.1920929e-07f)) {
            *pHit = farHit;
            nearest = farTime;
        }
    }
    if (static_cast<u8>(pInfo->mOption + 255) <= 1 &&
        !(math::FAbs(rPrevious.y) < 1.1920929e-07f &&
          math::FAbs(rCurrent.y) < 1.1920929e-07f)) {
        if (rPrevious.y > 0.0f && rCurrent.y < 1.1920929e-07f) {
            math::VEC3 capHit;
            farTime = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
            if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                *pHit = capHit;
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y += 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return farTime;
            }
        } else if (rPrevious.y < 0.0f && rCurrent.y > 1.1920929e-07f) {
            math::VEC3 capHit;
            farTime = -rPrevious.y / pDirection->y;
            math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
            if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                *pHit = capHit;
                if (pInfo->mFlags & 2) {
                    pVelocity->y *= -1.0f;
                    pDirection->y *= -1.0f;
                    pHit->y -= 1.1920929e-07f;
                }
                math::VEC3Transform(pWorldHit, &rMtx, pHit);
                return farTime;
            }
        }
    }
    if (nearest < 2.0f) {
        if (pInfo->mFlags & 2) {
            math::MTX34 frame;
            math::VEC3 local;
            math::VEC3 normal;
            f32 radius = math::FSqrt(1.0f - pHit->z * pHit->z);
            f32 sine;
            f32 cosine;
            if (radius > 1.1920929e-07f) {
                cosine = pHit->y / radius;
                sine = -pHit->x / radius;
            } else {
                cosine = 1.0f;
                sine = 0.0f;
            }
            frame._00 = cosine;
            frame._01 = pHit->x;
            frame._02 = pHit->z * sine;
            frame._03 = 0.0f;
            frame._10 = sine;
            frame._11 = pHit->y;
            frame._12 = -cosine * pHit->z;
            frame._13 = 0.0f;
            frame._20 = 0.0f;
            frame._21 = pHit->z;
            frame._22 = radius;
            frame._23 = 0.0f;
            math::MTX34Inv(&frame, &frame);
            math::VEC3Transform(&local, &frame, pDirection);
            math::VEC3Scale(&normal, pHit, local.y);
            *pDirection -= normal;
            *pDirection -= normal;
            math::VEC3Transform(&local, &frame, pVelocity);
            math::VEC3Scale(&normal, pHit, local.y);
            *pVelocity -= normal;
            *pVelocity -= normal;
            if (previousLength > 1.0f) {
                math::VEC3Scale(pHit, pHit, 1.00001f);
            } else {
                math::VEC3Scale(pHit, pHit, 0.99999f);
            }
        }
        math::VEC3Transform(pWorldHit, &rMtx, pHit);
    }
    return nearest;
}

f32 crossCheckCylinder(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 nearest = 2.0f;
    f32 previousLength = rPrevious.x * rPrevious.x + rPrevious.z * rPrevious.z;
    f32 directionLength = pDirection->x * pDirection->x + pDirection->z * pDirection->z;
    f32 dot = rPrevious.x * pDirection->x + rPrevious.z * pDirection->z;
    f32 discriminant = 4.0f * dot * dot - 4.0f * directionLength * (previousLength - 1.0f);
    if (discriminant < 0.0f) {
        return nearest;
    }
    f32 root = math::FSqrt(discriminant);
    f32 nearTime = (-2.0f * dot - root) / (2.0f * directionLength);
    f32 farTime = (-2.0f * dot + root) / (2.0f * directionLength);
    if (nearTime > 0.0f && nearTime < 1.0000001f) {
        math::VEC3Lerp(pHit, &rPrevious, &rCurrent, nearTime);
        if (math::FAbs(pHit->y) < 1.0000001f) {
            nearest = nearTime;
        }
    }
    if (farTime > 0.0f && farTime < 1.0000001f && farTime < nearest) {
        math::VEC3 farHit;
        math::VEC3Lerp(&farHit, &rPrevious, &rCurrent, farTime);
        if (math::FAbs(farHit.y) < 1.0000001f) {
            *pHit = farHit;
            nearest = farTime;
        }
    }
    {
        f32 level = 1.0f;
        if (!(math::FAbs(rPrevious.y - level) < 1.1920929e-07f &&
              math::FAbs(rCurrent.y - level) < 1.1920929e-07f)) {
            if (rPrevious.y > level && rCurrent.y < 1.1920929e-07f + level) {
                math::VEC3 capHit;
                farTime = (level - rPrevious.y) / pDirection->y;
                math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
                if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                    *pHit = capHit;
                    if (pInfo->mFlags & 2) {
                        pVelocity->y *= -1.0f;
                        pDirection->y *= -1.0f;
                        pHit->y += 1.1920929e-07f;
                    }
                    math::VEC3Transform(pWorldHit, &rMtx, pHit);
                    return farTime;
                }
            } else if (rPrevious.y < level && rCurrent.y > 1.1920929e-07f + level) {
                math::VEC3 capHit;
                farTime = (level - rPrevious.y) / pDirection->y;
                math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
                if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                    *pHit = capHit;
                    if (pInfo->mFlags & 2) {
                        pVelocity->y *= -1.0f;
                        pDirection->y *= -1.0f;
                        pHit->y -= 1.1920929e-07f;
                    }
                    math::VEC3Transform(pWorldHit, &rMtx, pHit);
                    return farTime;
                }
            }
        }
    }
    {
        f32 level = -1.0f;
        if (!(math::FAbs(rPrevious.y - level) < 1.1920929e-07f &&
              math::FAbs(rCurrent.y - level) < 1.1920929e-07f)) {
            if (rPrevious.y > level && rCurrent.y < 1.1920929e-07f + level) {
                math::VEC3 capHit;
                farTime = (level - rPrevious.y) / pDirection->y;
                math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
                if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                    *pHit = capHit;
                    if (pInfo->mFlags & 2) {
                        pVelocity->y *= -1.0f;
                        pDirection->y *= -1.0f;
                        pHit->y += 1.1920929e-07f;
                    }
                    math::VEC3Transform(pWorldHit, &rMtx, pHit);
                    return farTime;
                }
            } else if (rPrevious.y < level && rCurrent.y > 1.1920929e-07f + level) {
                math::VEC3 capHit;
                farTime = (level - rPrevious.y) / pDirection->y;
                math::VEC3Lerp(&capHit, &rPrevious, &rCurrent, farTime);
                if (farTime < nearest && capHit.x * capHit.x + capHit.z * capHit.z < 1.0000002f) {
                    *pHit = capHit;
                    if (pInfo->mFlags & 2) {
                        pVelocity->y *= -1.0f;
                        pDirection->y *= -1.0f;
                        pHit->y -= 1.1920929e-07f;
                    }
                    math::VEC3Transform(pWorldHit, &rMtx, pHit);
                    return farTime;
                }
            }
        }
    }
    if (nearest < 2.0f) {
        if (pInfo->mFlags & 2) {
            math::VEC3 normal;
            math::FSqrt(pDirection->x * pDirection->x + pDirection->z * pDirection->z);
            f32 dot = pDirection->x * pHit->x + pDirection->z * pHit->z;
            normal.x = pHit->x;
            normal.y = 0.0f;
            normal.z = pHit->z;
            math::VEC3Normalize(&normal, &normal);
            math::VEC3Scale(&normal, &normal, dot);
            *pDirection -= normal;
            *pDirection -= normal;
            math::FSqrt(pVelocity->x * pVelocity->x + pVelocity->z * pVelocity->z);
            dot = pVelocity->x * pHit->x + pVelocity->z * pHit->z;
            normal.x = pHit->x;
            normal.y = 0.0f;
            normal.z = pHit->z;
            math::VEC3Normalize(&normal, &normal);
            math::VEC3Scale(&normal, &normal, dot);
            *pVelocity -= normal;
            *pVelocity -= normal;
            if (previousLength > 1.0f) {
                math::VEC3Scale(pHit, pHit, 1.00001f);
            } else {
                math::VEC3Scale(pHit, pHit, 0.99999f);
            }
        }
        math::VEC3Transform(pWorldHit, &rMtx, pHit);
    }
    return nearest;
}

f32 crossCheckCube(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit) {
    f32 nearest = 2.0f;
    f32 positiveX = nearest;
    f32 negativeX = nearest;
    f32 positiveY = nearest;
    f32 negativeY = nearest;
    f32 positiveZ = nearest;
    f32 negativeZ = nearest;
    math::VEC3 faceHit;
    f32 time;
    time = (1.0f - rPrevious.x) / pDirection->x;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.y) <= 1.0000001f && math::FAbs(faceHit.z) <= 1.0000001f) {
            positiveX = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    time = (-1.0f - rPrevious.x) / pDirection->x;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.y) <= 1.0000001f && math::FAbs(faceHit.z) <= 1.0000001f) {
            negativeX = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    time = (1.0f - rPrevious.y) / pDirection->y;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.x) <= 1.0000001f && math::FAbs(faceHit.z) <= 1.0000001f) {
            positiveY = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    time = (-1.0f - rPrevious.y) / pDirection->y;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.x) <= 1.0000001f && math::FAbs(faceHit.z) <= 1.0000001f) {
            negativeY = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    time = (1.0f - rPrevious.z) / pDirection->z;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.x) <= 1.0000001f && math::FAbs(faceHit.y) <= 1.0000001f) {
            positiveZ = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    time = (-1.0f - rPrevious.z) / pDirection->z;
    if (time > 0.0f && time <= 1.0000001f) {
        math::VEC3Lerp(&faceHit, &rPrevious, &rCurrent, time);
        if (math::FAbs(faceHit.x) <= 1.0000001f && math::FAbs(faceHit.y) <= 1.0000001f) {
            negativeZ = time;
            if (time < nearest) {
                nearest = time;
            }
        }
    }
    if (2.0f == nearest) {
        return 2.0f;
    }
    pHit->x = rPrevious.x + nearest * (rCurrent.x - rPrevious.x);
    pHit->y = rPrevious.y + nearest * (rCurrent.y - rPrevious.y);
    pHit->z = rPrevious.z + nearest * (rCurrent.z - rPrevious.z);
    math::VEC3Transform(pWorldHit, &rMtx, pHit);
    if (pInfo->mFlags & 2) {
        if (math::FAbs(positiveX - nearest) <= 1.1920929e-07f ||
            math::FAbs(negativeX - nearest) <= 1.1920929e-07f) {
            pVelocity->x *= -1.0f;
            pDirection->x *= -1.0f;
        }
        if (math::FAbs(positiveY - nearest) <= 1.1920929e-07f ||
            math::FAbs(negativeY - nearest) <= 1.1920929e-07f) {
            pVelocity->y *= -1.0f;
            pDirection->y *= -1.0f;
        }
        if (math::FAbs(positiveZ - nearest) <= 1.1920929e-07f ||
            math::FAbs(negativeZ - nearest) <= 1.1920929e-07f) {
            pVelocity->z *= -1.0f;
            pDirection->z *= -1.0f;
        }
        if (math::FAbs(positiveX - nearest) <= 1.1920929e-07f) {
            pHit->x = 1.0f;
        }
        if (math::FAbs(negativeX - nearest) <= 1.1920929e-07f) {
            pHit->x = -1.0f;
        }
        if (math::FAbs(positiveY - nearest) <= 1.1920929e-07f) {
            pHit->y = 1.0f;
        }
        if (math::FAbs(negativeY - nearest) <= 1.1920929e-07f) {
            pHit->y = -1.0f;
        }
        if (math::FAbs(positiveZ - nearest) <= 1.1920929e-07f) {
            pHit->z = 1.0f;
        }
        if (math::FAbs(negativeZ - nearest) <= 1.1920929e-07f) {
            pHit->z = -1.0f;
        }
    }
    return nearest;
}

static inline int GetTypePosition(u8 shape, const math::VEC3& rPosition, u8 option) {
    switch (shape) {
    case 0:
        return GetTypePositionPlane(rPosition, option);
    case 1:
        return GetTypePositionRectangle(rPosition, option);
    case 2:
        return GetTypePositionCircle(rPosition, option);
    case 3:
        return GetTypePositionCube(rPosition, option);
    case 4:
        return GetTypePositionSphere(rPosition, option);
    case 5:
        return GetTypePositionCylinder(rPosition, option);
    }
    return 1;
}

int crossCheck(const math::MTX34& rMtx, const math::MTX34& rInverseMtx,
               const PostFieldInfo* pInfo, EmitterResource* pResource,
               Particle* pParticle, const math::VEC3& rPrevious,
               const math::VEC3& rCurrent, math::VEC3 velocity,
               bool* pKilled, math::VEC3* pHit, math::VEC3* pWorldHit,
               math::VEC3* pPosition, math::VEC3* pVelocity) {
    GetTypePosition(pInfo->mShape, rCurrent, pInfo->mOption);
    math::VEC3 direction;
    math::VEC3Sub(&direction, &rCurrent, &rPrevious);
    f32 crossing = 2.0f;
    switch (pInfo->mShape) {
    case 0:
        crossing = crossCheckPlane(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    case 1:
        crossing = crossCheckRectangle(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    case 2:
        crossing = crossCheckCircle(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    case 3:
        crossing = crossCheckCube(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    case 4:
        crossing = crossCheckSphere(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    case 5:
        crossing = crossCheckCylinder(rMtx, pInfo, rCurrent, rPrevious,
                                   &direction, &velocity, pHit, pWorldHit);
        break;
    }
    if (crossing > 1.5f) {
        return 0;
    }
    if ((pInfo->mFlags & 2) || (pInfo->mFlags & 8)) {
        math::VEC3Transform(pVelocity, &rInverseMtx, &velocity);
        pVelocity->x *= pInfo->mCollisionScale.x;
        pVelocity->y *= pInfo->mCollisionScale.y;
        pVelocity->z *= pInfo->mCollisionScale.z;
        math::VEC3 remaining;
        math::VEC3Scale(&remaining, &direction, 1.0f - crossing);
        math::VEC3Transform(&remaining, &rInverseMtx, &remaining);
        remaining.x *= pInfo->mCollisionScale.x;
        remaining.y *= pInfo->mCollisionScale.y;
        remaining.z *= pInfo->mCollisionScale.z;
        math::VEC3Add(pPosition, pWorldHit, &remaining);
    } else {
        math::VEC3Transform(pVelocity, &rInverseMtx, &velocity);
        math::VEC3Transform(pPosition, &rMtx, &rCurrent);
    }
    if (pInfo->mFlags & 0x40) {
        *pKilled = true;
        *pPosition = *pWorldHit;
    }
    if ((pInfo->mFlags & 0x10) && pResource != nullptr) {
        pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES->mCreationQueue.AddParticleCreation(
            &pInfo->mInheritSetting, pParticle, pResource, pParticle->mCalcRemain,
            pWorldHit, nullptr);
    }
    if ((pInfo->mFlags & 0x20) && pResource != nullptr) {
        pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES->mCreationQueue.AddEmitterCreation(
            &pInfo->mInheritSetting, pParticle, pResource, pParticle->mCalcRemain,
            pWorldHit, nullptr);
    }
    return 1;
}

}
}
