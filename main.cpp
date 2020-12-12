#include <stdio.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include "common.hpp"
using namespace std;
using namespace glm;

GLFWwindow *window;

int main(int argc, char *argv[])
{
    if (!glfwInit()) {
        fprintf(stderr, "cannot init glfw\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1024, 768, "Hello", NULL, NULL);
    if (!window) {
        fprintf(stderr, "cannot create glfw window\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    InitControl();
    InitGlobe();
    InitPanel();
    InitDigits();
    InitBackground();

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShowBackground();
        ShowGlobe();
        ShowPanel();
        ShowDigits();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    glfwTerminate();
}
