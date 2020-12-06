#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <vector>
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// shader.cpp
GLuint LoadShaders(const char *vertexFilePath, const char *fragmentFilePath);

// texture.cpp
GLuint LoadBMP(const char *path);
GLuint LoadTGA(const char *path);

// control.cpp
#define BAR_RADIUS 20.0f
#define BUTTON_RADIUS 40.0f

struct SlidingBar {
    glm::vec2 begin;
    glm::vec2 end;
    float progress;
    SlidingBar(glm::vec2 _begin, glm::vec2 _end, float _progress);
};


void InitControl();
extern glm::mat4 viewMatrix;
extern glm::mat4 viewBkgMatrix;
extern glm::mat4 projectionMatrix;
extern std::vector<SlidingBar> bars;
extern glm::vec3 lightDir;

// obj.cpp
void LoadOBJ(const char *path,
             std::vector<glm::vec3> &vertices,
             std::vector<glm::vec2> &uvs,
             std::vector<glm::vec3> &normals);
void IndexVBO(std::vector<glm::vec3> &vertices,
              std::vector<glm::vec2> &uvs,
              std::vector<glm::vec3> &normals,
              std::vector<uint16_t> &indices);

// globe.cpp
void InitGlobe();
void ShowGlobe();
void DeleteGlobe();

// panel.cpp
void InitPanel();
void ShowPanel();
void DeletePanel();

// backgound.cpp
void InitBackground();
void ShowBackground();
void DeleteBackground();

// main.cpp
extern GLFWwindow *window;

// for show
#define GL_ENABLE_VERTEX_ATRRIB_ARRAYS(n) \
    for (int i = 0; i < n; i++)           \
    glEnableVertexAttribArray(i)

#define GL_DISABLE_VERTEX_ATRRIB_ARRAYS(n) \
    for (int i = 0; i < n; i++)            \
    glDisableVertexAttribArray(i)

#endif
