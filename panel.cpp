#include <vector>
#include "common.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

static void drawSlidingBar(vec2, vec2, float, vector<vec3> &, vector<vec2> &,
                           float, float);
template <class T>
static void pushRectangle(vector<T> &, T, T, T, T);

static GLuint shaderID;
static GLuint vertexBufferID, uvBufferID;
static GLuint textureID, samplerID;

const static float barRadius = 20.0f;
const static float buttonRadius = 40.0f;

void InitPanel()
{
    shaderID = LoadShaders("shaders/panel_vtx.txt", "shaders/panel_frg.txt");

    textureID = LoadTGA("resources/panel.tga");

    glGenBuffers(1, &vertexBufferID);
    glGenBuffers(1, &uvBufferID);

    samplerID = glGetUniformLocation(shaderID, "sampler");
}

void ShowPanel()
{
    vector<vec3> vertices;
    vector<vec2> uvs;
    drawSlidingBar(vec2(-0.5f, 0.8f), vec2(0.5f, 0.8f), 0.3f,
                   vertices, uvs, -0.90f, -0.91f);
    drawSlidingBar(vec2(-0.8f, -0.5f), vec2(-0.8f, 0.5), 0.97f,
                   vertices, uvs, -0.92f, -0.93f);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
                 &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
                 &uvs[0], GL_STATIC_DRAW);

    glUseProgram(shaderID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(samplerID, 0);

    GL_ENABLE_VERTEX_ATRRIB_ARRAYS(2);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw call
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisable(GL_BLEND);

    GL_DISABLE_VERTEX_ATRRIB_ARRAYS(2);
}

void DeletePanel()
{
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &textureID);
}

static void drawSlidingBar(vec2 begin, vec2 end, float progress,
                           vector<vec3> &vertices, vector<vec2> &uvs,
                           float barZ, float buttonZ)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    vec2 absolute = vec2(width / 2.0f, height / 2.0f);
    vec3 relative = vec3(2.0f / width, 2.0f / height, 1.0f);

    vec2 delta = (end - begin) * absolute;
    float len = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
    if (len == 0) {
        return;
    }
    float cos_k = delta[0] / len;
    float sin_k = delta[1] / len;
    mat3 rotate = mat3(
        cos_k, sin_k, 0.0f,
        -sin_k, cos_k, 0.0f,
        0.0f, 0.0f, 1.0f);

    /*  b0__b1____m0____e0__e1    5--4 1
     *  /    |     |     |   \      // |
     *  |  begin  mid    end  |    //  |
     *  \    |     |     |   /    3 2  0
     *  b3--b2----m1----e3--e2 */

    // begin
    vec3 b[] = {
        vec3(-barRadius, barRadius, 0.0f),
        vec3(0.0f, barRadius, 0.0f),
        vec3(0.0f, -barRadius, 0.0f),
        vec3(-barRadius, -barRadius, 0.0f)};
    for (int i = 0; i < 4; i++) {
        b[i] = rotate * b[i] * relative + vec3(begin, barZ);
    }
    pushRectangle(vertices, b[0], b[1], b[2], b[3]);
    pushRectangle(uvs,
                  vec2(0.5f, 1.0f),
                  vec2(0.625f, 1.0f),
                  vec2(0.625f, 0.75f),
                  vec2(0.5f, 0.75f));

    // end
    vec3 e[] = {
        vec3(0.0f, barRadius, 0.0f),
        vec3(barRadius, barRadius, 0.0f),
        vec3(barRadius, -barRadius, 0.0f),
        vec3(0.0f, -barRadius, 0.0f)};
    for (int i = 0; i < 4; i++) {
        e[i] = rotate * e[i] * relative + vec3(end, barZ);
    }
    pushRectangle(vertices, e[0], e[1], e[2], e[3]);
    pushRectangle(uvs,
                  vec2(0.875f, 1.0f),
                  vec2(1.0f, 1.0f),
                  vec2(1.0f, 0.75f),
                  vec2(0.875f, 0.75f));

    // filled
    vec3 m0 = b[1] * (1 - progress) + e[0] * progress;
    vec3 m1 = b[2] * (1 - progress) + e[3] * progress;
    pushRectangle(vertices, b[1], m0, m1, b[2]);

    pushRectangle(uvs,
                  vec2(0.625f, 1.0f),
                  vec2(0.75f, 1.0f),
                  vec2(0.75f, 0.75f),
                  vec2(0.625f, 0.75f));

    // unfilled
    pushRectangle(vertices, m0, e[0], e[3], m1);
    pushRectangle(uvs,
                  vec2(0.75f, 1.0f),
                  vec2(0.875f, 1.0f),
                  vec2(0.875f, 0.75f),
                  vec2(0.75f, 0.75f));

    // button
    vec3 mid = vec3(begin * (1 - progress) + end * progress, buttonZ);
    vec3 u[] = {
        vec3(-buttonRadius, buttonRadius, 0.0f),
        vec3(buttonRadius, buttonRadius, 0.0f),
        vec3(buttonRadius, -buttonRadius, 0.0f),
        vec3(-buttonRadius, -buttonRadius, 0.0f)};
    for (int i = 0; i < 4; i++) {
        u[i] = rotate * u[i] * relative + mid;
    }
    pushRectangle(vertices, u[0], u[1], u[2], u[3]);
    pushRectangle(uvs,
                  vec2(0.0f, 1.0f),
                  vec2(0.5f, 1.0f),
                  vec2(0.5f, 0.5f),
                  vec2(0.0f, 0.5f));
}

template <class T>
static void pushRectangle(vector<T> &vec, T p0, T p1, T p2, T p3)
{
    vec.push_back(p2);
    vec.push_back(p1);
    vec.push_back(p3);
    vec.push_back(p3);
    vec.push_back(p1);
    vec.push_back(p0);
}
