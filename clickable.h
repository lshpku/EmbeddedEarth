#ifndef _CXX_EMBEDDED_EARTH_CLICKABLE_H_
#define _CXX_EMBEDDED_EARTH_CLICKABLE_H_
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "drawable.h"
#include "graphics.h"

using namespace glm;

class Clickable : public Drawable {

public:
    Clickable() {}
    virtual void onClick(double x, double y) = 0;
};

#endif