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
static Texture texDay, texNight;
static GLuint lightID;
static GLuint vertexBufferID, uvBufferID, normalBufferID;
static GLuint elementBufferID;
static size_t elementCount;

void InitGlobe()
{
    shaderID = LoadShaders("shaders/globe_vtx.txt", "shaders/globe_frg.txt");

    mvpMatrixID = glGetUniformLocation(shaderID, "MVP");
    viewMatrixID = glGetUniformLocation(shaderID, "V");

    texDay.texture = LoadBMP("resources/globe_day.bmp");
    texDay.sampler = glGetUniformLocation(shaderID, "sampler_day");
    texNight.texture = LoadBMP("resources/globe_night.bmp");
    texNight.sampler = glGetUniformLocation(shaderID, "sampler_night");

    lightID = glGetUniformLocation(shaderID, "light_mod");

    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;

    LoadOBJ("resources/globe.obj", vertices, uvs, normals);

    vector<uint16_t> indices;
    printf("indexing\n");
    IndexVBO(vertices, uvs, normals, indices);

    vertexBufferID = bindBuffer(GL_ARRAY_BUFFER, vertices);
    uvBufferID = bindBuffer(GL_ARRAY_BUFFER, uvs);
    normalBufferID = bindBuffer(GL_ARRAY_BUFFER, normals);

    elementCount = indices.size();
    elementBufferID = bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
}

void ShowGlobe()
{
    glUseProgram(shaderID);

    mat4 mvp = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

    glUniform3f(lightID, 4.0f, 4.0f, 4.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDay.texture);
    glUniform1i(texDay.sampler, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texNight.texture);
    glUniform1i(texNight.sampler, 1);

    GL_ENABLE_VERTEX_ATRRIB_ARRAYS(3);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, NULL);

    GL_DISABLE_VERTEX_ATRRIB_ARRAYS(3);
}

void DeleteGlobe()
{
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &texDay.texture);
    glDeleteTextures(1, &texNight.texture);
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
