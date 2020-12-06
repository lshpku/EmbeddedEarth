#include <stdio.h>
#include <vector>
#include "common.hpp"
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

template <class T>
static GLuint bindBuffer(GLenum, const vector<T> &);

static GLuint shaderID;
static GLuint mvpMatrixID, viewMatrixID;
static Texture texture;
static GLuint vertexBufferID, uvBufferID;
static GLuint elementBufferID;
static size_t elementCount;

void InitBackground()
{
    shaderID = LoadShaders("shaders/background_vtx.txt", "shaders/background_frg.txt");

    mvpMatrixID = glGetUniformLocation(shaderID, "MVP");
    viewMatrixID = glGetUniformLocation(shaderID, "V");

    texture.texture = LoadBMP("resources/star_map.bmp");
    texture.sampler = glGetUniformLocation(shaderID, "sampler");

    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;
    LoadOBJ("resources/globe.obj", vertices, uvs, normals);

    vector<uint16_t> indices;
    printf("indexing\n");
    IndexVBO(vertices, uvs, normals, indices);
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] = vertices[i] * 16.0f;
    }

    vertexBufferID = bindBuffer(GL_ARRAY_BUFFER, vertices);
    uvBufferID = bindBuffer(GL_ARRAY_BUFFER, uvs);

    elementCount = indices.size();
    elementBufferID = bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
}

void ShowBackground()
{
    glUseProgram(shaderID);

    mat4 mvp = projectionMatrix * viewBkgMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.texture);
    glUniform1i(texture.sampler, 0);

    GL_ENABLE_VERTEX_ATRRIB_ARRAYS(2);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, NULL);

    GL_DISABLE_VERTEX_ATRRIB_ARRAYS(2);
}

void DeleteBackground()
{
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &texture.texture);
}

template <class T>
static GLuint bindBuffer(GLenum type, const vector<T> &data)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return buffer;
}
