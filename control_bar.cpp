#include "control_bar.h"

#include "globe.h"

SlidingBar::SlidingBar(vec2 _begin, vec2 _end, float _progress)
    : begin(_begin), end(_end), progress(_progress) {}

void ControlBar::drawSlidingBar(SlidingBar &bar, int z,
                                std::vector<vec3> &vertices,
                                std::vector<vec2> &uvs) {
  Size size = Graphics::getWindowSize();
  vec2 absolute = vec2(size.width / 2.0f, size.height / 2.0f);
  vec3 relative = vec3(2.0f / size.width, 2.0f / size.height, 1.0f);

  vec2 delta = (bar.end - bar.begin) * absolute;
  float len = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
  if (len == 0) {
    return;
  }
  float cos_k = delta[0] / len;
  float sin_k = delta[1] / len;
  mat3 rotate = mat3(cos_k, sin_k, 0.0f, -sin_k, cos_k, 0.0f, 0.0f, 0.0f, 1.0f);

  float barZ = -0.002f * z;
  float buttonZ = barZ - 0.001f;

  /*  b0__b1____m0____e0__e1    5--4 1
   *  /    |     |     |   \      // |
   *  |  begin  mid    end  |    //  |
   *  \    |     |     |   /    3 2  0
   *  b3--b2----m1----e3--e2 */

  // begin
  vec3 b[] = {vec3(-barRadius, barRadius, 0.0f), vec3(0.0f, barRadius, 0.0f),
              vec3(0.0f, -barRadius, 0.0f), vec3(-barRadius, -barRadius, 0.0f)};
  for (int i = 0; i < 4; i++) {
    b[i] = rotate * b[i] * relative + vec3(bar.begin, barZ);
  }
  pushRectangle(vertices, b[0], b[1], b[2], b[3]);
  pushRectangle(uvs, vec2(0.5f, 1.0f), vec2(0.625f, 1.0f), vec2(0.625f, 0.75f),
                vec2(0.5f, 0.75f));

  // end
  vec3 e[] = {vec3(0.0f, barRadius, 0.0f), vec3(barRadius, barRadius, 0.0f),
              vec3(barRadius, -barRadius, 0.0f), vec3(0.0f, -barRadius, 0.0f)};
  for (int i = 0; i < 4; i++) {
    e[i] = rotate * e[i] * relative + vec3(bar.end, barZ);
  }
  pushRectangle(vertices, e[0], e[1], e[2], e[3]);
  pushRectangle(uvs, vec2(0.875f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.75f),
                vec2(0.875f, 0.75f));

  // filled
  vec3 m0 = b[1] * (1 - bar.progress) + e[0] * bar.progress;
  vec3 m1 = b[2] * (1 - bar.progress) + e[3] * bar.progress;
  pushRectangle(vertices, b[1], m0, m1, b[2]);

  pushRectangle(uvs, vec2(0.625f, 1.0f), vec2(0.75f, 1.0f), vec2(0.75f, 0.75f),
                vec2(0.625f, 0.75f));

  // unfilled
  pushRectangle(vertices, m0, e[0], e[3], m1);
  pushRectangle(uvs, vec2(0.75f, 1.0f), vec2(0.875f, 1.0f), vec2(0.875f, 0.75f),
                vec2(0.75f, 0.75f));

  // button
  vec3 mid =
      vec3(bar.begin * (1 - bar.progress) + bar.end * bar.progress, buttonZ);
  vec3 u[] = {vec3(-buttonRadius, buttonRadius, 0.0f),
              vec3(buttonRadius, buttonRadius, 0.0f),
              vec3(buttonRadius, -buttonRadius, 0.0f),
              vec3(-buttonRadius, -buttonRadius, 0.0f)};
  for (int i = 0; i < 4; i++) {
    u[i] = rotate * u[i] * relative + mid;
  }
  pushRectangle(vertices, u[0], u[1], u[2], u[3]);
  pushRectangle(uvs, vec2(0.0f, 1.0f), vec2(0.5f, 1.0f), vec2(0.5f, 0.5f),
                vec2(0.0f, 0.5f));
}

template <class T>
void ControlBar::pushRectangle(std::vector<T> &vec, T p0, T p1, T p2, T p3) {
  vec.push_back(p2);
  vec.push_back(p1);
  vec.push_back(p3);
  vec.push_back(p3);
  vec.push_back(p1);
  vec.push_back(p0);
}

void ControlBar::notifyCompetitor() {
  if (competitor != nullptr) competitor->onCompete();
}

ControlBar::ControlBar(Globe *globe, std::function<void(float, Globe *)> func,
                       glm::vec2 begin, glm::vec2 end, float progress,
                       Drawable *competitor)
    : globe(globe),
      bar(begin, end, progress),
      func(func),
      competitor(competitor) {
  shaderID =
      Graphics::loadShaders("shaders/panel_vtx.txt", "shaders/panel_frg.txt");

  textureID = loadImage("resources/panel.tga", GL_RGBA);

  glGenBuffers(1, &vertexBufferID);
  glGenBuffers(1, &uvBufferID);

  samplerID = glGetUniformLocation(shaderID, "sampler");
}

ControlBar::~ControlBar() {
  glDeleteBuffers(1, &vertexBufferID);
  glDeleteBuffers(1, &uvBufferID);
  glDeleteProgram(shaderID);
  glDeleteTextures(1, &textureID);
}

void ControlBar::initDrag(double x, double y) {
  vec2 offset = Graphics::getScreenOffset();
  vec2 screen = Graphics::getScreenVec2();
  vec2 begin = (bar.begin + offset) * screen;
  vec2 end = (bar.end + offset) * screen;
  vec2 mid = begin * (1 - bar.progress) + end * bar.progress;
  buttonOffset = mid - vec2(x, y);
}

bool ControlBar::isIn(double x, double y) {
  vec2 offset = Graphics::getScreenOffset();
  vec2 screen = Graphics::getScreenVec2();
  vec2 begin = (bar.begin + offset) * screen;
  vec2 end = (bar.end + offset) * screen;
  vec2 mid = begin * (1 - bar.progress) + end * bar.progress;
  if (distance(mid, vec2(x, y)) < buttonRadius) return true;
  return false;
}

void ControlBar::onDrag(double dx, double dy, double dt, double x, double y) {
  vec2 offset = Graphics::getScreenOffset();
  vec2 screen = Graphics::getScreenVec2();
  vec2 begin = (bar.begin + offset) * screen;
  vec2 end = (bar.end + offset) * screen;

  vec2 o = vec2(x, y) + buttonOffset;
  float be = distance(begin, end);
  float ob = distance(o, begin) / be;
  float oe = distance(o, end) / be;
  float p = (ob * ob - oe * oe + 1) / 2;

  p = clamp(p, 0.0f, 1.0f);
  bar.progress = p;

  func(p, globe);
  globe->updateMatrix();
  notifyCompetitor();
}

void ControlBar::show() {
  std::vector<vec3> vertices;
  std::vector<vec2> uvs;

  drawSlidingBar(bar, 1, vertices, uvs);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0],
               GL_STATIC_DRAW);

  glUseProgram(shaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(samplerID, 0);

  GL_ENABLE_VERTEX_ATTRIB_ARRAYS(2);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Draw call
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());

  glDisable(GL_BLEND);

  GL_DISABLE_VERTEX_ATTRIB_ARRAYS(2);
}