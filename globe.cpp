#include "globe.h"

template <class T>
GLuint Globe::bindBuffer(GLenum type, const std::vector<T> &data) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(type, buffer);
  glBufferData(type, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
  return buffer;
}

Globe::Globe() {
  programId =
      Graphics::loadShaders("shaders/globe_vtx.txt", "shaders/globe_frg.txt");

  mvpMatrixId = glGetUniformLocation(programId, "MVP");
  viewMatrixId = glGetUniformLocation(programId, "V");

  texDayID = loadImage("resources/globe_day.bmp", GL_RGB);
  sampDayID = glGetUniformLocation(programId, "sampler_day");
  texNightID = loadImage("resources/globe_night.bmp", GL_RGB);
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

  horiAngle = 1.1f * pi<float>();  // asia centered
  vertAngle = 0.0f;
  radius = 3.125f;
  fieldOfView = 60.0f;
  lightHoriAngle = pi<float>();
  lightVertAngle = 0.0f;

  updateMatrix();
}

void Globe::onDrag(double dx, double dy, double dt, double x, double y) {
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

  GL_ENABLE_VERTEX_ATTRIB_ARRAYS(3);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
  glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, NULL);

  GL_DISABLE_VERTEX_ATTRIB_ARRAYS(3);
}

void Globe::updateMatrix() {
  const static vec3 origin = vec3(0, 0, 0);
  const static vec3 up = vec3(0, 1, 0);

  Size size = Graphics::getWindowSize();
  projectionMatrix = perspective(
      radians(fieldOfView),
      static_cast<float>(size.width) / static_cast<float>(size.height), 0.1f,
      100.0f);

  vec3 pos =
      vec3(radius * cos(vertAngle) * sin(horiAngle), radius * sin(vertAngle),
           radius * cos(vertAngle) * cos(horiAngle));

  viewMatrix = lookAt(pos, origin, up);
  viewBkgMatrix = lookAt(origin, -pos, up);

  lightDir =
      vec3(cos(lightVertAngle) * sin(lightHoriAngle), sin(lightVertAngle),
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