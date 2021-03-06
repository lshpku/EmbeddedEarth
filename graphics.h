#ifndef _CXX_EMBEDDED_EARTH_GRAPHICS_H_
#define _CXX_EMBEDDED_EARTH_GRAPHICS_H_

#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Drawable;
class Clickable;
class Draggable;

enum class State { ready, holding, dragging, busy };

struct Size {
  int width;
  int height;
};

class Graphics {
  static GLFWwindow *window;

  static Drawable *clicked;

  static int width, height;
  static double lastX, lastY;
  static double lastTime;

  static std::vector<Draggable *> draggables;
  static std::vector<Clickable *> clickables;
  static std::vector<Drawable *> lazyResizables;

  static void compileShader(const std::string &code, GLuint shaderID);
  static std::string readFile(const char *path);
  static void throwInfoLog(GLuint program, int infoLogLength);

  static void framebufferSizeCallback(GLFWwindow *window, int w, int h);
  static void cursorPositionCallback(GLFWwindow *window, double x, double y);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);

 public:
  static State state;
  static void init(std::string name, int width, int height);

  static void dispose() { glfwTerminate(); }

  static Size getWindowSize() {
    Size s;
    glfwGetWindowSize(window, &(s.width), &(s.height));
    return s;
  }

  static GLFWwindow *getWindow() { return window; }

  static GLuint loadShaders(const char *vertexFilePath,
                            const char *fragmentFilePath);

  static void addDraggable(Draggable *elem);
  static void addClickable(Clickable *elem);
  static void addLazyResizable(Drawable *elem);

  static void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

  static bool shouldClose() {
    return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
           glfwWindowShouldClose(window) == 1;
  }

  static void display() {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  static glm::vec2 getScreenVec2() {
    Size size = getWindowSize();
    return glm::vec2(size.width, -size.height) / 2.0f;
  }

  static glm::vec2 getScreenOffset() { return glm::vec2(1.0f, -1.0f); }
};

#endif