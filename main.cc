#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <OpenGL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics.h"
#include "globe.h"
#include "background.h"
#include <iostream>
using namespace std;
using namespace glm;

int main(int argc, char *argv[])
{   
    try {
        Graphics::init("Embedded Earth", 1024, 768);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    Globe globe;
    Background background(&globe);
    Graphics::addDraggable(&globe);
    
    // InitControl();
    // InitGlobe();
    // InitPanel();
    // InitDigits();
    // InitBackground();
    printf("GL Version: %s\n", glGetString(GL_VERSION));

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        background.show();
        globe.show();

        Graphics::display();
    } // Check if the ESC key was pressed or the window was closed
    while (!Graphics::shouldClose());

    Graphics::dispose();
}
