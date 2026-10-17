#include <nw4r/ef/emform/ef_cube.h>
#include <nw4r/ef/emform/ef_cylinder.h>
#include <nw4r/ef/emform/ef_disc.h>
#include <nw4r/ef/emform/ef_emform.h>
#include <nw4r/ef/emform/ef_line.h>
#include <nw4r/ef/emform/ef_point.h>
#include <nw4r/ef/emform/ef_sphere.h>
#include <nw4r/ef/emform/ef_torus.h>

namespace nw4r {
namespace ef {

static EmitterFormDisc gEmitterFormDisc;
static EmitterFormLine gEmitterFormLine;
static EmitterFormCylinder gEmitterFormCylinder;
static EmitterFormSphere gEmitterFormSphere;
static EmitterFormTorus gEmitterFormTorus;
static EmitterFormCube gEmitterFormCube;
static EmitterFormPoint gEmitterFormPoint;

EmitterForm* EmitFormBuilder::Create(EmitFormType type) {
    switch (type) {
    case EMITFORMTYPE_DISC: {
        return &gEmitterFormDisc;
    }

    case EMITFORMTYPE_LINE: {
        return &gEmitterFormLine;
    }

    case EMITFORMTYPE_CYLINDER: {
        return &gEmitterFormCylinder;
    }

    case EMITFORMTYPE_SPHERE: {
        return &gEmitterFormSphere;
    }

    case EMITFORMTYPE_TORUS: {
        return &gEmitterFormTorus;
    }

    case EMITFORMTYPE_CUBE: {
        return &gEmitterFormCube;
    }

    case EMITFORMTYPE_POINT: {
        return &gEmitterFormPoint;
    }

    default: {
        return nullptr;
    }
    }
}

}
}
