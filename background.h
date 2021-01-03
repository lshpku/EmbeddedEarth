#ifndef _CXX_EMBEDDED_EARTH_BACKGROUND_H_
#define _CXX_EMBEDDED_EARTH_BACKGROUND_H_

#include "drawable.h"
#include "globe.h"
#include "graphics.h"
#include <vector>

class Background : public Drawable {
    Globe *globe;
    GLuint mvpMatrixID, viewMatrixID;
    GLuint textureID, samplerID;
    GLuint vertexBufferID, uvBufferID;
    GLuint elementBufferID;
    size_t elementCount;
public:
    Background(Globe *globe);
    ~Background();
    void show() override;
};

#endif