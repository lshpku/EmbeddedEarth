#include "background.h"

Background::Background(Globe *globe): globe(globe) {
    programId = Graphics::loadShaders("shaders/background_vtx.txt", "shaders/background_frg.txt");

    mvpMatrixID = glGetUniformLocation(programId, "MVP");
    viewMatrixID = glGetUniformLocation(programId, "V");

    textureID = loadImage("resources/star_map.bmp", GL_RGB);
    samplerID = glGetUniformLocation(programId, "sampler");

    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    loadObj("resources/globe.obj", vertices, uvs, normals);

    std::vector<uint16_t> indices;
    std::printf("indexing\n");
    indexVbo(vertices, uvs, normals, indices);
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] = vertices[i] * 16.0f;
    }

    vertexBufferID = Globe::bindBuffer(GL_ARRAY_BUFFER, vertices);
    uvBufferID = Globe::bindBuffer(GL_ARRAY_BUFFER, uvs);

    elementCount = indices.size();
    elementBufferID = Globe::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
}

void Background::show() {
    glUseProgram(programId);

    assert(globe != nullptr);

    mat4 mvp = globe->projectionMatrix * globe->viewBkgMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(samplerID, 0);

    glEnableVertexAttribArrays(2);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, NULL);

    glDisableVertexAttribArrays(2);
}

Background::~Background() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteProgram(programId);
    glDeleteTextures(1, &textureID);
}