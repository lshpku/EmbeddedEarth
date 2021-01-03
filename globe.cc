#include "globe.h"

GLuint LoadBMP(const char *path)
{
    printf("loading texture %s\n", path);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;

    // Open the file
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("file not found: %s\n", path);
        exit(-1);
    }

    if (fread(header, 1, 54, file) != 54) {
        fprintf(stderr, "Unexpected EOF\n");
        fclose(file);
        exit(-1);
    }
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Bad magic\n");
        fclose(file);
        exit(-1);
    }
    if (*(int *)(header + 0x1E) != 0 || *(int *)(header + 0x1C) != 24) {
        fprintf(stderr, "Not a 24-bit BMP\n");
        fclose(file);
        exit(-1);
    }

    dataPos = *(int *)&(header[0x0A]);
    imageSize = *(int *)&(header[0x22]);
    width = *(int *)&(header[0x12]);
    height = *(int *)&(header[0x16]);

    if (imageSize == 0)
        imageSize = width * height * 3;
    if (dataPos == 0)
        dataPos = 54;

    uint8_t *data = new uint8_t[imageSize];
    fread(data, 1, width * height * 3, file);
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, data);
    Drawable::genMipMap();

    delete[] data;
    return textureID;
}

template <class T>
GLuint Globe::bindBuffer(GLenum type, const std::vector<T> &data)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return buffer;
}

Globe::Globe() {
    programId = Graphics::loadShaders("shaders/globe_vtx.txt", "shaders/globe_frg.txt");

    mvpMatrixId = glGetUniformLocation(programId, "MVP");
    viewMatrixId = glGetUniformLocation(programId, "V");

    texDayID = LoadBMP("resources/globe_day.bmp");
    sampDayID = glGetUniformLocation(programId, "sampler_day");
    texNightID = LoadBMP("resources/globe_night.bmp");
    sampNightID = glGetUniformLocation(programId, "sampler_night");

    lightID = glGetUniformLocation(programId, "light_mod");

    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    loadObj("resources/globe.obj", vertices, uvs, normals);

    std::vector<uint16_t> indices;
    printf("indexing\n");
    indexVbo(vertices, uvs, normals, indices);

    vertexBufferID = bindBuffer(GL_ARRAY_BUFFER, vertices);
    uvBufferID = bindBuffer(GL_ARRAY_BUFFER, uvs);
    normalBufferID = bindBuffer(GL_ARRAY_BUFFER, normals);

    elementCount = indices.size();
    elementBufferID = bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

    horiAngle = 1.1f * pi<float>(); // asia centered
    vertAngle = 0.0f;
    radius = 3.125f;
    fieldOfView = 60.0f;
    lightHoriAngle = pi<float>();
    lightVertAngle = 0.0f;

    updateMatrix();
}

void Globe::onDrag(double dx, double dy, double dt) {
    printf("dx: %lf, dy: %lf, dt: %lf\n", dx, dy, dt);
    float synSpeed = dt * shiftSpeed * radius;

    horiAngle -= dx * synSpeed;
    vertAngle += dy * synSpeed;
    if (vertAngle > maxVertAngle) {
        vertAngle = maxVertAngle;
    } else if (vertAngle < -maxVertAngle) {
        vertAngle = -maxVertAngle;
    }

    updateMatrix();
}

void Globe::show() {
    glUseProgram(programId);

    mat4 mvp = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

    glUniform3f(lightID, lightDir[0], lightDir[1], lightDir[2]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDayID);
    glUniform1i(sampDayID, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texNightID);
    glUniform1i(sampNightID, 1);

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

void Globe::updateMatrix()
{
    const static vec3 origin = vec3(0, 0, 0);
    const static vec3 up = vec3(0, 1, 0);

    Size size = Graphics::getWindowSize();
    projectionMatrix = perspective(
        radians(fieldOfView),
        static_cast<float>(size.width) / static_cast<float>(size.height),
        0.1f, 100.0f);

    vec3 pos = vec3(
        radius * cos(vertAngle) * sin(horiAngle),
        radius * sin(vertAngle),
        radius * cos(vertAngle) * cos(horiAngle));

    viewMatrix = lookAt(pos, origin, up);
    viewBkgMatrix = lookAt(origin, -pos, up);

    lightDir = vec3(
        cos(lightVertAngle) * sin(lightHoriAngle),
        sin(lightVertAngle),
        cos(lightVertAngle) * cos(lightHoriAngle));
}

Globe::~Globe() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteProgram(programId);
    glDeleteTextures(1, &texDayID);
    glDeleteTextures(1, &sampDayID);
}