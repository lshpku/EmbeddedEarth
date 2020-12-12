#include <iostream>
#include "common.hpp"
using namespace std;
using namespace glm;

#define BUTTON_KBD 10

#define CHAR_MONTH 10
#define CHAR_DAY 11

const vec2 buttons[] = {
    vec2(0.75f, -0.9f),
    vec2(0.60f, -0.3f),
    vec2(0.75f, -0.3f),
    vec2(0.90f, -0.3f),
    vec2(0.60f, -0.5f),
    vec2(0.75f, -0.5f),
    vec2(0.90f, -0.5f),
    vec2(0.60f, -0.7f),
    vec2(0.75f, -0.7f),
    vec2(0.90f, -0.7f),
    vec2(0.60f, -0.9f)
};
vec2 movingButtons[10];
float movingProg;
const float eps = 0.05f;

const float dButtonRadius = 0.07f;
const vec2 characters[] = {
    vec2(0.0f, -0.9f),
    vec2(0.05f, -0.9f),
    vec2(0.21f, -0.9f),
    vec2(0.26f, -0.9f),
    vec2(0.13f, -0.9f),
    vec2(0.34f, -0.9f),
};
const float charRadius = 0.07f;

static GLuint shaderID;
static GLuint vertexBufferID, uvBufferID, indexBufferID;
static GLuint textureID, samplerID;

static inline void drawButtons(vector<vec3> &vertices, vector<vec2> &uvs,
                               vector<uint16_t> &indices);

static void pushButton(vector<vec3> &vertices, vector<vec2> &uvs,
                       vector<uint16_t> &indices, float x, float y, float r,
                       unsigned n);
static void pushChar(vector<vec3> &vertices, vector<vec2> &uvs,
                     vector<uint16_t> &indices, float x, float y, float r,
                     unsigned n, float x_zoomRatio);

void InitDigits()
{
    shaderID = LoadShaders("shaders/panel_vtx.txt", "shaders/panel_frg.txt");

    textureID = LoadPNG("resources/digitpanel.png");

    glGenBuffers(1, &vertexBufferID);
    glGenBuffers(1, &uvBufferID);
    glGenBuffers(1, &indexBufferID);

    samplerID = glGetUniformLocation(shaderID, "sampler");
}

void ShowDigits()
{
    // Positions of the digit buttons.
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<uint16_t> indices;

    drawButtons(vertices, uvs, indices);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
                 &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
                 &uvs[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t),
                 &indices[0], GL_STATIC_DRAW);

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
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisable(GL_BLEND);

    GL_DISABLE_VERTEX_ATRRIB_ARRAYS(2);
}

void DeleteDigits()
{
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &textureID);
}

static inline void drawButtons(vector<vec3> &vertices, vector<vec2> &uvs,
                               vector<uint16_t> &indices) {
    if (digitSwitchState == State::busy && isDigitPanelShown == true
        && movingProg - 1.0f < eps * 0.5f && movingProg - 1.0f > -eps * 0.5f) {
        // moving has finished (from origin to destination)
        digitSwitchState = State::ready;
    } else if (digitSwitchState == State::busy && isDigitPanelShown == false
               && movingProg < eps * 0.5f && movingProg > -eps * 0.5f) {
        // moving has finished (from destination to origin)
        digitSwitchState = State::ready;
    } else if (digitSwitchState == State::busy) {
        movingProg += (isDigitPanelShown ? eps : -eps);
        for (int i = 0; i < 10; i++)
            movingButtons[i] = buttons[i] * movingProg * movingProg
                + buttons[10] * (1.0f - movingProg) * (1.0f - movingProg);
    }

    if (isDigitPanelShown || digitSwitchState == State::busy)
        for (unsigned i = 0; i < 10; i++)
            pushButton(vertices, uvs, indices, movingButtons[i].x,
                       movingButtons[i].y, dButtonRadius, i);

    if (isDigitPanelActive) {
        pushChar(vertices, uvs, indices, characters[4].x, characters[4].y,
                 charRadius, CHAR_MONTH, 0.70f);
        pushChar(vertices, uvs, indices, characters[5].x, characters[5].y,
                 charRadius, CHAR_DAY, 0.60f);
        for (unsigned i = 0; i < (inputIndex ? inputIndex : 4); i++)
            pushChar(vertices, uvs, indices, characters[i].x, characters[i].y,
                     charRadius, input[i], 0.38f);
    }

    pushButton(vertices, uvs, indices, buttons[10].x, buttons[10].y,
               dButtonRadius, BUTTON_KBD);
}

/// Push a button centered at (x, y), with the radius [r], showing the number
/// [n], unless [n] is not a valid digit.
static void pushButton(vector<vec3> &vertices, vector<vec2> &uvs,
                       vector<uint16_t> &indices, float x, float y, float r,
                       unsigned n) {
    int nVertices = vertices.size();

    int windowWidth, windowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    float r_x = r * windowHeight / windowWidth;
    float z = -0.02f * n;

    vertices.push_back(vec3(x - r_x, y - r, z));
    vertices.push_back(vec3(x + r_x, y - r, z + 0.01f));
    vertices.push_back(vec3(x + r_x, y + r, z + 0.01f));
    vertices.push_back(vec3(x - r_x, y + r, z));

    if (n == BUTTON_KBD) {
        uvs.push_back(vec2(0.0f, 0.25f));
        uvs.push_back(vec2(0.1f, 0.25f));
        uvs.push_back(vec2(0.1f, 0.5f));
        uvs.push_back(vec2(0.0f, 0.5f));
    } else {
        uvs.push_back(vec2(0.1f * n, 0.0f));
        uvs.push_back(vec2(0.1f * n + 0.1f, 0.0f));
        uvs.push_back(vec2(0.1f * n + 0.1f, 0.25f));
        uvs.push_back(vec2(0.1f * n, 0.25f));
    }

    indices.push_back(nVertices);
    indices.push_back(nVertices + 1);
    indices.push_back(nVertices + 2);
    indices.push_back(nVertices + 2);
    indices.push_back(nVertices + 3);
    indices.push_back(nVertices);
}

/// Push a character centered at (x, y), with the radius [r], showing the number
/// [n], unless [n] is not a valid digit.
static void pushChar(vector<vec3> &vertices, vector<vec2> &uvs,
                     vector<uint16_t> &indices, float x, float y, float r,
                     unsigned n, float x_zoomRatio) {
    const int nVertices = vertices.size();

    int windowWidth, windowHeight;
 
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    const float r_x = r * windowHeight / windowWidth * x_zoomRatio;
    const float padding = (1.0f - x_zoomRatio) * 0.5f;
    float z = -0.1f;

    if (n == CHAR_MONTH)
        z += 0.04f;
    else if (n == CHAR_DAY)
        z += 0.02f;
        

    vertices.push_back(vec3(x - r_x, y - r, z));
    vertices.push_back(vec3(x + r_x, y - r, z + 0.01f));
    vertices.push_back(vec3(x + r_x, y + r, z + 0.01f));
    vertices.push_back(vec3(x - r_x, y + r, z));

    if (n == CHAR_MONTH) {
        uvs.push_back(vec2(0.0f + 0.1f * padding, 0.75f));
        uvs.push_back(vec2(0.1f - 0.1f * padding, 0.75f));
        uvs.push_back(vec2(0.1f - 0.1f * padding, 1.0f));
        uvs.push_back(vec2(0.0f + 0.1f * padding, 1.0f));
    } else if (n == CHAR_DAY) {
        uvs.push_back(vec2(0.1f + 0.1f * padding, 0.75f));
        uvs.push_back(vec2(0.2f - 0.1f * padding, 0.75f));
        uvs.push_back(vec2(0.2f - 0.1f * padding, 1.0f));
        uvs.push_back(vec2(0.1f + 0.1f * padding, 1.0f));
    } else {
        uvs.push_back(vec2(0.1f * (n + padding), 0.5f));
        uvs.push_back(vec2(0.1f * (n + 1.0f - padding), 0.5f));
        uvs.push_back(vec2(0.1f * (n + 1.0f - padding), 0.75f));
        uvs.push_back(vec2(0.1f * (n + padding), 0.75f));
    }

    indices.push_back(nVertices);
    indices.push_back(nVertices + 1);
    indices.push_back(nVertices + 2);
    indices.push_back(nVertices + 2);
    indices.push_back(nVertices + 3);
    indices.push_back(nVertices);
}