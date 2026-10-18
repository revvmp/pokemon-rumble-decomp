#ifndef GAME_CSHADOWCAMERA_H
#define GAME_CSHADOWCAMERA_H

#include <game/BWorldCamera.h>

class CShadowCamera : public BWorldCamera {
public:
    enum { TYPE = TYPE_SHADOW_CAMERA };
};

#endif
