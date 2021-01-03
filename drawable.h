#ifndef _CXX_EMBEDDED_EARTH_DRAWABLE_H_
#define _CXX_EMBEDDED_EARTH_DRAWABLE_H_
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "graphics.h"

using namespace glm;

struct PackedVertex {
  vec3 position;
  vec2 uv;
  vec3 normal;
  bool operator<(const PackedVertex that) const {
    return memcmp((void *)this, (void *)&that, sizeof(PackedVertex)) > 0;
  };
};

class Drawable {
 public:
  static void genMipMap();

 protected:
  GLuint programId;

  void indexVbo(std::vector<vec3> &vertices, std::vector<vec2> &uvs,
                std::vector<vec3> &normals, std::vector<uint16_t> &indices);
  GLuint loadImage(const char *path, GLenum format);
  void loadObj(const char *path, std::vector<vec3> &out_vertices,
               std::vector<vec2> &out_uvs, std::vector<vec3> &out_normals);

 public:
  // Drawable(Graphics &graphics) : controller(&graphics) {}
  virtual void show() = 0;
  virtual bool isIn(double x, double y) { return true; }
  virtual void onCompete() {}
  virtual void onResize() {}
};

#endif