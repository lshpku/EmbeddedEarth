#ifndef _CXX_EMBEDDED_EARTH_GLOBE_H_
#define _CXX_EMBEDDED_EARTH_GLOBE_H_

#include <vector>

#include "draggable.h"
#include "graphics.h"

class Globe : public Draggable {
  GLuint mvpMatrixId;
  GLuint viewMatrixId;
  mat4 viewMatrix;
  mat4 viewBkgMatrix;
  mat4 projectionMatrix;

  float horiAngle, vertAngle;
  const float maxVertAngle = pi<float>() * 0.49f;

  float fieldOfView;

  vec3 lightDir;
  GLuint texDayID, texNightID, sampDayID, sampNightID;
  GLuint lightID;
  GLuint vertexBufferID, uvBufferID, normalBufferID;
  GLuint elementBufferID;
  size_t elementCount;
  const float shiftSpeed = 0.03f;

  template <class T>
  static GLuint bindBuffer(GLenum type, const std::vector<T> &data);

  void updateMatrix();
  friend class Background;
  friend class ControlBar;
  friend class DigitPanel;

 public:
  float lightHoriAngle, lightVertAngle;
  float radius;

  Globe();
  ~Globe();
  void onDrag(double dx, double dy, double dt, double x, double y) override;
  void onResize() override { updateMatrix(); }
  void show() override;
};

#define GL_ENABLE_VERTEX_ATTRIB_ARRAYS(n) \
  for (int i = 0; i < n; i++) glEnableVertexAttribArray(i)

#define GL_DISABLE_VERTEX_ATTRIB_ARRAYS(n) \
  for (int i = 0; i < n; i++) glDisableVertexAttribArray(i)

#endif