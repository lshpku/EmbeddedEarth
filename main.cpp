#include <iostream>

#include "background.h"
#include "control_bar.h"
#include "digit_panel.h"
#include "globe.h"
#include "graphics.h"

using namespace glm;

int main(int argc, char *argv[]) {
  try {
    Graphics::init("Embedded Earth", 1024, 768);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  Globe globe;
  Background background(&globe);
  DigitPanel digitPanel(&globe);

  ControlBar bars[] = {
      ControlBar(
          &globe,
          [](float p, Globe *globe) {
            globe->lightHoriAngle = p * pi<float>() * 2.0f;
          },
          vec2(-0.5, 0.8f), vec2(0.5f, 0.8f), 0.5f),
      ControlBar(
          &globe,
          [](float p, Globe *globe) {
            globe->lightVertAngle = (p - 0.5) * radians(47.0f);
          },
          vec2(-0.8f, -0.5f), vec2(-0.8f, 0.5f), 0.5f, &digitPanel),
      ControlBar(
          &globe,
          [](float p, Globe *globe) { globe->radius = 5.0f - p * 3.75f; },
          vec2(0.8f, -0.3f), vec2(0.8f, 0.3f), 0.5f),
  };

  Graphics::addClickable(&digitPanel);

  Graphics::addDraggable(&bars[0]);
  Graphics::addDraggable(&bars[1]);
  Graphics::addDraggable(&bars[2]);

  Graphics::addDraggable(&globe);
  Graphics::addLazyResizable(&globe);

  do {
    Graphics::clear();

    background.show();
    globe.show();

    bars[0].show();
    bars[1].show();
    bars[2].show();

    digitPanel.show();

    Graphics::display();
  }  // Check if the ESC key was pressed or the window was closed
  while (!Graphics::shouldClose());

  Graphics::dispose();
}
