#ifndef _CXX_EMBEDDED_EARTH_CLICKABLE_H_
#define _CXX_EMBEDDED_EARTH_CLICKABLE_H_

#include "drawable.h"

class Clickable : public Drawable {
 public:
  Clickable() {}
  virtual void onClick(double x, double y) = 0;
};

#endif