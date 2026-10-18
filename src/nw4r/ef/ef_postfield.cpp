#include <nw4r/ef/ef_postfield.h>

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

}
}
