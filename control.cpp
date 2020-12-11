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

inline static bool isInButton(float x, float y, float r);
static void enterNum(int n);

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



// Digit panel
bool isDigitPanelShown;
bool isDigitPanelActive;
State digitSwitchState = State::ready;
static State digitStates[10];
uint8_t input[4];
uint8_t inputIndex;
static void simulateDate(int month, int day);

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
    vec2 offset = vec2(1.0f, -1.0f);
    vec2 screen = vec2(width, -height) / 2.0f;
    glfwGetCursorPos(window, &lastX, &lastY);
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    if (action == GLFW_PRESS) {
        clicked = true;
        lastTime = glfwGetTime();
        
        // test button click
        int i;
        for (i = 0; i < bars.size(); i++) { 
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
        } else if (isInButton(0.6f, -0.9f, 0.07f)) {            
            if (digitSwitchState == State::ready)
                digitSwitchState = State::holding;
        } else if (isDigitPanelShown) {
            int i;
            for (i = 0; i < 10; i++) {
                if (isInButton(buttons[i].x, buttons[i].y, dButtonRadius)
                    && digitStates[i] == State::ready)
                    digitStates[i] = State::holding;
            }
            if (i == 10)
                clicked = CLICK_GLOBE;
        } else {
            clicked = CLICK_GLOBE;
        }
    } else {
        clicked = CLICK_NONE;
        if (isInButton(0.6f, -0.9f, 0.07f)
            && digitSwitchState == State::holding) {
            digitSwitchState = State::busy;
            isDigitPanelShown = !isDigitPanelShown;
        } else if (digitSwitchState == State::holding) {
            digitSwitchState = State::ready;
        }

        for (int i = 0; i < 10; i++) {
            if (isInButton(buttons[i].x, buttons[i].y, dButtonRadius)
                && digitStates[i] == State::holding) {
                enterNum(i);
                isDigitPanelActive = true;
                digitStates[i] = State::ready;
            } else if (digitStates[i] == State::holding) {
                digitStates[i] = State::ready;
            }
        }
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

        if (clicked == 1)
            isDigitPanelActive = false;

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

inline static bool isInButton(float x, float y, float r) {
    const vec2 offset = vec2(1.0f, -1.0f);
    vec2 screen = vec2(width, -height) / 2.0f;
    vec2 pos = (vec2(x, y) + offset) * screen;
    float r_screen = r * height / 2;
    return distance(pos, vec2(lastX, lastY)) < r_screen;
}

static void enterNum(int n) {
    assert(inputIndex < 4);
    assert(n < 10 && n >= 0);
    input[inputIndex] = n;
    inputIndex = (inputIndex + 1) % 4;

    if (inputIndex == 0) {
        int month = input[0] * 10 + input[1];
        int day = input[2] * 10 + input[3];
        simulateDate(month, day);
    }
}

static void simulateDate(int month, int day) {
    const int days[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    // Validate input.
    if (month < 1 || month > 12 || day < 0 || day > days[month])
        return;

    if (month * 100 + day >= 622 && month * 100 + day <= 1222) {
        // Between June 22 and Dec. 22 (both inclusive).
        int dateDiff = days[6] - 22;
        
        for (int i = 7; i < month; i++)
            dateDiff += days[i];

        dateDiff += month > 6 ? day : 0;
        float prog = (float)dateDiff / 183.0f;
        lightVertAngle = radians(-23.5f * prog + 23.5f * (1.0f - prog));
        updateMatrix();
    } else {
        // Between Dec. 22 and June 22 of the next year (both inclusive).
        int dateDiff = days[12] - 22;
        
        for (int i = 1; i < month; i++)
            dateDiff += days[i];

        dateDiff += month < 12 ? day : 0;
        float prog = (float)dateDiff / 182.0f;
        lightVertAngle = radians(23.5f * prog - 23.5f * (1.0f - prog));
        updateMatrix();
    }
}

SlidingBar::SlidingBar(vec2 _begin, vec2 _end, float _progress)
    : begin(_begin), end(_end), progress(_progress) {}
