#ifndef _CXX_EMBEDDED_EARTH_DRAGGABLE_H_
#define _CXX_EMBEDDED_EARTH_DRAGGABLE_H_
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "drawable.h"
#include "graphics.h"

using namespace glm;

class Draggable : public Drawable {

public:
    Draggable() {}
    virtual void onDrag(double dx, double dy, double dt) = 0;
};

#endif