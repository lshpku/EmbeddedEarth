#include "graphics.h"
#include "clickable.h"
#include "draggable.h"

GLFWwindow *Graphics::window;
GLuint Graphics::vertexArrayId;

Drawable *Graphics::clicked;
State Graphics::state;

int Graphics::width, Graphics::height;
double Graphics::lastX, Graphics::lastY;
double Graphics::lastTime;

std::vector<Draggable *> Graphics::draggables;
std::vector<Clickable *> Graphics::clickables;

void Graphics::compileShader(const std::string &code, GLuint shaderID)
{
    GLint result = GL_FALSE;
    int infoLogLength;

    const char *codePointer = code.c_str();
    glShaderSource(shaderID, 1, &codePointer, NULL);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    throwInfoLog(shaderID, infoLogLength);
}

std::string Graphics::readFile(const char *path) {
    std::string code;
    std::ifstream stream(path, std::ios::in);
    if (stream.is_open()) {
        std::stringstream sstr;
        sstr << stream.rdbuf();
        code = sstr.str();
        stream.close();
    } else {
        fprintf(stderr, "file not found: %s\n", path);
        exit(-1);
    }
    return code;
}

void Graphics::throwInfoLog(GLuint program, int infoLogLength)
{
    if (!infoLogLength) {
        return;
    }
    char *infoLogBuffer = new char[infoLogLength];
    glGetShaderInfoLog(program, infoLogLength, NULL, infoLogBuffer);
    fprintf(stderr, "%s\n", infoLogBuffer);
    exit(-1);
}

void Graphics::init(std::string name, int width, int height) {
    if (!glfwInit())
        throw std::runtime_error("cannot init glfw\n");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

    if (!window) {
        throw std::runtime_error("cannot create glfw window\n");
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    Size size = getWindowSize();
    height = size.height;
    width = size.width;
}

GLuint Graphics::loadShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile the shaders
    printf("loading shader %s\n", vertexFilePath);
    std::string vertexShaderCode = readFile(vertexFilePath);
    compileShader(vertexShaderCode.c_str(), vertexShaderID);
    printf("loading shader %s\n", fragmentFilePath);
    std::string fragmentShaderCode = readFile(fragmentFilePath);
    compileShader(fragmentShaderCode, fragmentShaderID);

    // Link the program
    GLuint programID = glCreateProgram();
    printf("linking shaders\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // Check the program
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    throwInfoLog(programID, infoLogLength);

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

void Graphics::addDraggable(Draggable *elem) {
    draggables.push_back(elem);
}

void Graphics::addClickable(Clickable *elem) {
    clickables.push_back(elem);
}

void Graphics::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && state == State::ready) {
        clicked = nullptr;

        for (auto elem : draggables)
            if (elem->isIn(lastX, lastY)) {
                state = State::dragging;
                clicked = elem;
                return;
            }

        for (auto elem : clickables)
            if (elem->isIn(lastX, lastY)) {
                state = State::holding;
                clicked = elem;
                return;
            }
    } else if (action == GLFW_RELEASE && state == State::dragging) {
        state = State::ready;
    } else if (action == GLFW_RELEASE && state == State::holding) {
        state = State::busy;
        static_cast<Clickable *>(clicked)->onClick(lastX, lastY);
    }
}

void Graphics::cursorPositionCallback(GLFWwindow *window, double x, double y) {
    if (state == State::dragging) {
        double curTime = glfwGetTime();
        double dt = curTime - lastTime;
        double dx = x - lastX;
        double dy = y - lastY;

        static_cast<Draggable *>(clicked)->onDrag(dx, dy, dt);

        lastTime = curTime;
        lastX = x;
        lastY = y;
    }
}

void Graphics::framebufferSizeCallback(GLFWwindow *window, int w, int h)
{
    // do not use w & h! they are related to the resolution
    glfwGetWindowSize(window, &width, &height);
    // TODO add onResize call
}