#ifndef _CXX_EMBEDDED_EARTH_DRAGGABLE_H_
#define _CXX_EMBEDDED_EARTH_DRAGGABLE_H_

#include "drawable.h"

using namespace glm;

class Draggable : public Drawable {
 public:
  Draggable() {}
  virtual void initDrag(double x, double y) {}
  virtual void onDrag(double dx, double dy, double dt, double x, double y) = 0;
};

#endif