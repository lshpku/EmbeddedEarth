#include <stdio.h>
#include "common.hpp"
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

static void mouseButtonCallback(GLFWwindow *, int, int, int);
static void cursorPositionCallback(GLFWwindow *, double, double);
static void framebufferSizeCallback(GLFWwindow *, int, int);

static void updateMatrix();

mat4 viewMatrix;
mat4 viewBkgMatrix;
mat4 projectionMatrix;

// Mouse status
static bool clicked = false;
static double lastX, lastY;
static double lastTime;
const static float shiftSpeed = 0.1f;

// Spherical coordinate status
static float horiAngle, vertAngle;
static float radius;
const static float maxVertAngle = pi<float>() * 0.49f;

// View status
static float fieldOfView;
static int width, height;

// Light status
static float lightHoriAngle, lightVertAngle;

void InitControl()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    horiAngle = 0.0f;
    vertAngle = 0.0f;
    radius = 3.0f;
    fieldOfView = 60.0f;
    glfwGetWindowSize(window, &width, &height);

    updateMatrix();
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    if (action == GLFW_PRESS) {
        clicked = true;
        lastTime = glfwGetTime();
        glfwGetCursorPos(window, &lastX, &lastY);
    } else {
        clicked = false;
    }
}

static void cursorPositionCallback(GLFWwindow *window, double x, double y)
{
    if (!clicked) {
        return;
    }

    double curTime = glfwGetTime();
    double deltaTime = curTime - lastTime;
    double deltaX = x - lastX;
    double deltaY = y - lastY;

    horiAngle -= deltaX * deltaTime * shiftSpeed;
    vertAngle += deltaY * deltaTime * shiftSpeed;
    if (vertAngle > maxVertAngle) {
        vertAngle = maxVertAngle;
    } else if (vertAngle < -maxVertAngle) {
        vertAngle = -maxVertAngle;
    }

    updateMatrix();

    lastTime = curTime;
    lastX = x;
    lastY = y;
}

static void framebufferSizeCallback(GLFWwindow *window, int w, int h)
{
    width = w;
    height = h;
    updateMatrix();
}

static void updateMatrix()
{
    const static vec3 origin = vec3(0, 0, 0);
    const static vec3 up = vec3(0, 1, 0);

    projectionMatrix = perspective(
        radians(fieldOfView),
        (float)width / (float)height,
        0.1f, 100.0f);

    vec3 pos = vec3(
        radius * cos(vertAngle) * sin(horiAngle),
        radius * sin(vertAngle),
        radius * cos(vertAngle) * cos(horiAngle));

    viewMatrix = lookAt(pos, origin, up);

    viewBkgMatrix = lookAt(origin, -pos, up);
}
