#ifndef _CXX_EMBEDDED_EARTH_CONTROL_BAR_H_
#define _CXX_EMBEDDED_EARTH_CONTROL_BAR_H_

#include <vector>

#include "draggable.h"
#include "graphics.h"

class Globe;

struct SlidingBar {
  glm::vec2 begin;
  glm::vec2 end;
  float progress;
  SlidingBar(glm::vec2 _begin, glm::vec2 _end, float _progress);
};

class ControlBar : public Draggable {
  GLuint vertexBufferID, uvBufferID;
  GLuint textureID, samplerID;
  Globe *globe;
  SlidingBar bar;
  std::function<void(float, Globe *)> func;
  glm::vec2 buttonOffset;
  Drawable *competitor;

  static void drawSlidingBar(SlidingBar &bar, int z,
                             std::vector<vec3> &vertices,
                             std::vector<vec2> &uvs);

  template <class T>
  static void pushRectangle(std::vector<T> &vec, T p0, T p1, T p2, T p3);

  void notifyCompetitor();

  constexpr static float barRadius = 20.0f;
  constexpr static float buttonRadius = 40.0f;

 public:
  ControlBar(Globe *globe, std::function<void(float, Globe *)> func,
             glm::vec2 begin, glm::vec2 end, float progress,
             Drawable *competitor = nullptr);
  ~ControlBar();
  void initDrag(double x, double y) override;
  bool isIn(double x, double y) override;
  void onDrag(double dx, double dy, double dt, double x, double y) override;
  void show() override;
};

#endif