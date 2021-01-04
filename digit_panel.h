#ifndef _CXX_EMBEDDED_EARTH_DIGIT_PANEL_H_
#define _CXX_EMBEDDED_EARTH_DIGIT_PANEL_H_

#include <vector>

#include "clickable.h"
#include "graphics.h"

class Globe;

class DigitPanel : public Clickable {
  const vec2 buttons[11] = {
      vec2(0.75f, -0.9f), vec2(0.60f, -0.3f), vec2(0.75f, -0.3f),
      vec2(0.90f, -0.3f), vec2(0.60f, -0.5f), vec2(0.75f, -0.5f),
      vec2(0.90f, -0.5f), vec2(0.60f, -0.7f), vec2(0.75f, -0.7f),
      vec2(0.90f, -0.7f), vec2(0.60f, -0.9f),
  };
  vec2 movingButtons[10];
  float movingProg;
  static constexpr float eps = 0.05f;

  const float buttonRadius = 0.07f;
  const vec2 characters[6] = {
      vec2(0.0f, -0.9f),  vec2(0.05f, -0.9f), vec2(0.21f, -0.9f),
      vec2(0.26f, -0.9f), vec2(0.13f, -0.9f), vec2(0.34f, -0.9f),
  };
  const float charRadius = 0.07f;
  const unsigned monthChar = 10;
  const unsigned dayChar = 11;
  const unsigned kbdButton = 10;
  uint8_t input[4];
  uint8_t inputIndex;

  GLuint vertexBufferID, uvBufferID, indexBufferID;
  GLuint textureID, samplerID;
  bool isShown;
  bool isActive;

  Globe *globe;

  void drawButtons(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                   std::vector<uint16_t> &indices);

  void pushButton(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                  std::vector<uint16_t> &indices, float x, float y, float r,
                  unsigned n);

  void pushChar(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                std::vector<uint16_t> &indices, float x, float y, float r,
                unsigned n, float x_zoomRatio);

  void enterNum(int n);
  void simulateDate(int month, int day);

  static bool isInButton(float x, float y, float r, float xCursor,
                         float yCursor) {
    const Size size = Graphics::getWindowSize();
    const vec2 offset = Graphics::getScreenOffset();
    vec2 screen = Graphics::getScreenVec2();
    vec2 pos = (vec2(x, y) + offset) * screen;
    float rScreen = r * size.height / 2;
    return distance(pos, vec2(xCursor, yCursor)) < rScreen;
  }

 public:
  DigitPanel(Globe *globe);
  ~DigitPanel();
  bool isIn(double x, double y) override;
  void onClick(double x, double y) override;
  void onCompete() override;
  void show() override;
};

#endif