#include "digit_panel.h"

#include "globe.h"

void DigitPanel::drawButtons(std::vector<vec3> &vertices,
                             std::vector<vec2> &uvs,
                             std::vector<uint16_t> &indices) {
  if (Graphics::state == State::busy && isShown == true &&
      movingProg - 1.0f < eps * 0.5f && movingProg - 1.0f > -eps * 0.5f) {
    // moving has finished (from origin to destination)
    Graphics::state = State::ready;
  } else if (Graphics::state == State::busy && isShown == false &&
             movingProg < eps * 0.5f && movingProg > -eps * 0.5f) {
    // moving has finished (from destination to origin)
    Graphics::state = State::ready;
  } else if (Graphics::state == State::busy) {
    movingProg += (isShown ? eps : -eps);
    for (int i = 0; i < 10; i++)
      movingButtons[i] =
          buttons[i] * movingProg * movingProg +
          buttons[10] * (1.0f - movingProg) * (1.0f - movingProg);
  }

  if (isShown || Graphics::state == State::busy)
    for (unsigned i = 0; i < 10; i++)
      pushButton(vertices, uvs, indices, movingButtons[i].x, movingButtons[i].y,
                 buttonRadius, i);

  if (isActive) {
    pushChar(vertices, uvs, indices, characters[4].x, characters[4].y,
             charRadius, monthChar, 0.70f);
    pushChar(vertices, uvs, indices, characters[5].x, characters[5].y,
             charRadius, dayChar, 0.60f);
    for (unsigned i = 0; i < (inputIndex ? inputIndex : 4); i++)
      pushChar(vertices, uvs, indices, characters[i].x, characters[i].y,
               charRadius, input[i], 0.38f);
  }

  pushButton(vertices, uvs, indices, buttons[10].x, buttons[10].y, buttonRadius,
             kbdButton);
}

/// Push a button centered at (x, y), with the radius [r], showing the number
/// [n], unless [n] is not a valid digit.
void DigitPanel::pushButton(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                            std::vector<uint16_t> &indices, float x, float y,
                            float r, unsigned n) {
  int nVertices = vertices.size();

  Size size = Graphics::getWindowSize();

  float r_x = r * size.height / size.width;
  float z = -0.02f * n;

  vertices.push_back(vec3(x - r_x, y - r, z));
  vertices.push_back(vec3(x + r_x, y - r, z + 0.01f));
  vertices.push_back(vec3(x + r_x, y + r, z + 0.01f));
  vertices.push_back(vec3(x - r_x, y + r, z));

  if (n == kbdButton) {
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
void DigitPanel::pushChar(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                          std::vector<uint16_t> &indices, float x, float y,
                          float r, unsigned n, float x_zoomRatio) {
  const int nVertices = vertices.size();

  Size size = Graphics::getWindowSize();

  const float r_x = r * size.height / size.width * x_zoomRatio;
  const float padding = (1.0f - x_zoomRatio) * 0.5f;
  float z = -0.1f;

  if (n == monthChar)
    z += 0.04f;
  else if (n == dayChar)
    z += 0.02f;

  vertices.push_back(vec3(x - r_x, y - r, z));
  vertices.push_back(vec3(x + r_x, y - r, z + 0.01f));
  vertices.push_back(vec3(x + r_x, y + r, z + 0.01f));
  vertices.push_back(vec3(x - r_x, y + r, z));

  if (n == monthChar) {
    uvs.push_back(vec2(0.0f + 0.1f * padding, 0.75f));
    uvs.push_back(vec2(0.1f - 0.1f * padding, 0.75f));
    uvs.push_back(vec2(0.1f - 0.1f * padding, 1.0f));
    uvs.push_back(vec2(0.0f + 0.1f * padding, 1.0f));
  } else if (n == dayChar) {
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

void DigitPanel::enterNum(int n) {
  assert(inputIndex < 4);
  assert(n < 10 && n >= 0);
  input[inputIndex] = n;
  inputIndex = (inputIndex + 1) % 4;

  if (inputIndex == 0) {
    int month = input[0] * 10 + input[1];
    int day = input[2] * 10 + input[3];
    simulateDate(month, day);
  }
}

void DigitPanel::simulateDate(int month, int day) {
  const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Validate input.
  if (month < 1 || month > 12 || day < 0 || day > days[month]) return;

  if (month * 100 + day >= 622 && month * 100 + day <= 1222) {
    // Between June 22 and Dec. 22 (both inclusive).
    int dateDiff = days[6] - 22;

    for (int i = 7; i < month; i++) dateDiff += days[i];

    dateDiff += month > 6 ? day : 0;
    float prog = (float)dateDiff / 183.0f;
    globe->lightVertAngle = radians(-23.5f * prog + 23.5f * (1.0f - prog));
    globe->updateMatrix();
  } else {
    // Between Dec. 22 and June 22 of the next year (both exclusive).
    int dateDiff = days[12] - 22;

    for (int i = 1; i < month; i++) dateDiff += days[i];

    dateDiff += month < 12 ? day : 0;
    float prog = (float)dateDiff / 182.0f;
    globe->lightVertAngle = radians(23.5f * prog - 23.5f * (1.0f - prog));
    globe->updateMatrix();
  }
}

DigitPanel::DigitPanel(Globe *globe)
    : movingProg(0.0f), inputIndex(0), isShown(false), isActive(false), globe(globe) {
  programId =
      Graphics::loadShaders("shaders/panel_vtx.txt", "shaders/panel_frg.txt");

  textureID = loadImage("resources/digitpanel.png", GL_RGBA);

  glGenBuffers(1, &vertexBufferID);
  glGenBuffers(1, &uvBufferID);
  glGenBuffers(1, &indexBufferID);

  samplerID = glGetUniformLocation(programId, "sampler");
}

DigitPanel::~DigitPanel() {
  glDeleteBuffers(1, &vertexBufferID);
  glDeleteBuffers(1, &uvBufferID);
  glDeleteBuffers(1, &indexBufferID);
  glDeleteProgram(programId);
  glDeleteTextures(1, &textureID);
}

bool DigitPanel::isIn(double x, double y) {
  if (isInButton(0.6f, -0.9f, 0.07f, x, y)) return true;

  if (isShown)
    for (int i = 0; i < 10; i++) {
      if (isInButton(buttons[i].x, buttons[i].y, buttonRadius, x, y))
        return true;
    }

  return false;
}

void DigitPanel::onClick(double x, double y) {
  if (isInButton(0.6f, -0.9f, 0.07f, x, y)) {
    isShown = !isShown;
    return;
  }

  for (int i = 0; i < 10; i++) {
    if (isInButton(buttons[i].x, buttons[i].y, buttonRadius, x, y)) {
      enterNum(i);
      isActive = true;
      // TODO Use a method to restore isActive.
      break;
    }
  }

  Graphics::state = State::ready;
}

void DigitPanel::onCompete() { isActive = false; }

void DigitPanel::show() {
  // Positions of the digit buttons.
  std::vector<vec3> vertices;
  std::vector<vec2> uvs;
  std::vector<uint16_t> indices;

  drawButtons(vertices, uvs, indices);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t),
               &indices[0], GL_STATIC_DRAW);

  glUseProgram(programId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(samplerID, 0);

  glEnableVertexAttribArrays(2);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Draw call
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

  glDisable(GL_BLEND);

  glEnableVertexAttribArrays(2);
}