#ifndef _CXX_EMBEDDED_EARTH_GLOBE_H_
#define _CXX_EMBEDDED_EARTH_GLOBE_H_

#include "draggable.h"
#include "graphics.h"
#include <vector>

class Globe : public Draggable {
    GLuint mvpMatrixId;
    GLuint viewMatrixId;
    mat4 viewMatrix;
    mat4 viewBkgMatrix;
    mat4 projectionMatrix;

    float horiAngle, vertAngle;
    float radius;
    const float maxVertAngle = pi<float>() * 0.49f;  

    float fieldOfView;

    float lightHoriAngle, lightVertAngle;

    vec3 lightDir;
    GLuint texDayID, texNightID, sampDayID, sampNightID;
    GLuint lightID;
    GLuint vertexBufferID, uvBufferID, normalBufferID;
    GLuint elementBufferID;
    size_t elementCount;
    const float shiftSpeed = 0.03f;

    template <class T>
    static GLuint bindBuffer(GLenum type, const std::vector<T> &data);
    
    void updateMatrix();
    friend class Background;

public:
    Globe();
    ~Globe();
    void onDrag(double dx, double dy, double dt) override;
    void show() override;
};

#define GL_ENABLE_VERTEX_ATRRIB_ARRAYS(n) \
    for (int i = 0; i < n; i++)           \
    glEnableVertexAttribArray(i)

#define GL_DISABLE_VERTEX_ATRRIB_ARRAYS(n) \
    for (int i = 0; i < n; i++)            \
    glDisableVertexAttribArray(i)

#endif