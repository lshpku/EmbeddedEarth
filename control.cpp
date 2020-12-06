#include <vector>
#include <stdio.h>
#include "common.hpp"
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

static void mouseButtonCallback(GLFWwindow *, int, int, int);
static void cursorPositionCallback(GLFWwindow *, double, double);
static void framebufferSizeCallback(GLFWwindow *, int, int);

static void updateMatrix();

mat4 viewMatrix;
mat4 viewBkgMatrix;
mat4 projectionMatrix;
vector<SlidingBar> bars;

// Mouse status
#define CLICK_NONE -1
#define CLICK_GLOBE -2
static int clicked = CLICK_NONE;
static double lastX, lastY;
static double lastTime;
const static float shiftSpeed = 0.03f;
vec2 buttonOffset;

// Spherical coordinate status
static float horiAngle, vertAngle;
static float radius;
const static float maxVertAngle = pi<float>() * 0.49f;

// View status
static float fieldOfView;
static int width, height;

// Light status
static float lightHoriAngle, lightVertAngle;
vec3 lightDir;

void InitControl()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    horiAngle = 1.1f * pi<float>(); // asia centered
    vertAngle = 0.0f;
    radius = 3.125f;
    fieldOfView = 60.0f;
    glfwGetWindowSize(window, &width, &height);
    lightHoriAngle = pi<float>();
    lightVertAngle = 0.0f; 

    // light horizontal angle
    bars.push_back(SlidingBar(vec2(-0.5, 0.8f), vec2(0.5f, 0.8f), 0.5f));
    // light vertical angle
    bars.push_back(SlidingBar(vec2(-0.8f, -0.5f), vec2(-0.8f, 0.5f), 0.5f));
    // radius
    bars.push_back(SlidingBar(vec2(0.8f, -0.3f), vec2(0.8f, 0.3f), 0.5f));

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

        // test button click
        int i;
        for (i = 0; i < bars.size(); i++) {
            vec2 offset = vec2(1.0f, -1.0f);
            vec2 screen = vec2(width, -height) / 2.0f;
            vec2 begin = (bars[i].begin + offset) * screen;
            vec2 end = (bars[i].end + offset) * screen;
            vec2 mid = begin * (1 - bars[i].progress) + end * bars[i].progress;
            if (distance(mid, vec2(lastX, lastY)) < BUTTON_RADIUS) {
                buttonOffset = mid - vec2(lastX, lastY);
                break;
            }
        }
        if (i < bars.size()) {
            clicked = i;
        } else {
            clicked = CLICK_GLOBE;
        }
    } else {
        clicked = CLICK_NONE;
    }
}

static void cursorPositionCallback(GLFWwindow *window, double x, double y)
{
    if (clicked == CLICK_NONE) {
        return;
    }

    // drag button
    if (clicked != CLICK_GLOBE) {
        vec2 offset = vec2(1.0f, -1.0f);
        vec2 screen = vec2(width, -height) / 2.0f;
        vec2 begin = (bars[clicked].begin + offset) * screen;
        vec2 end = (bars[clicked].end + offset) * screen;

        vec2 o = vec2(x, y) + buttonOffset;
        float be = distance(begin, end);
        float ob = distance(o, begin) / be;
        float oe = distance(o, end) / be;
        float p = (ob * ob - oe * oe + 1) / 2;
        p = clamp(p, 0.0f, 1.0f);
        bars[clicked].progress = p;

        switch (clicked) {
        case 0:
            lightHoriAngle = p * pi<float>() * 2.0f;
            break;
        case 1:
            lightVertAngle = (p - 0.5) * radians(47.0f);
            break;
        case 2:
            radius = 5.0f - p * 3.75f;
            break;
        }
    }

    // turn globe
    else {
        double curTime = glfwGetTime();
        double deltaTime = curTime - lastTime;
        double deltaX = x - lastX;
        double deltaY = y - lastY;
        float synSpeed = deltaTime * shiftSpeed * radius;

        horiAngle -= deltaX * synSpeed;
        vertAngle += deltaY * synSpeed;
        if (vertAngle > maxVertAngle) {
            vertAngle = maxVertAngle;
        } else if (vertAngle < -maxVertAngle) {
            vertAngle = -maxVertAngle;
        }

        lastTime = curTime;
        lastX = x;
        lastY = y;
    }

    updateMatrix();
}

static void framebufferSizeCallback(GLFWwindow *window, int w, int h)
{
    // do not use w & h! they are related to the resolution
    glfwGetWindowSize(window, &width, &height);
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

    lightDir = vec3(
        cos(lightVertAngle) * sin(lightHoriAngle),
        sin(lightVertAngle),
        cos(lightVertAngle) * cos(lightHoriAngle));
}

SlidingBar::SlidingBar(vec2 _begin, vec2 _end, float _progress)
    : begin(_begin), end(_end), progress(_progress) {}
