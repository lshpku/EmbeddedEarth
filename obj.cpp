#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <map>
#include <glm/glm.hpp>
#include "common.hpp"
using namespace std;
using namespace glm;

void LoadOBJ(const char *path,
             vector<vec3> &out_vertices,
             vector<vec2> &out_uvs,
             vector<vec3> &out_normals)
{
    printf("loading obj %s\n", path);

    vector<uint32_t> vertexIndices, uvIndices, normalIndices;
    vector<vec3> temp_vertices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;

    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "file not found: %s\n", path);
        exit(-1);
    }

    char line[1024];
    float x, y, z;
    while (fscanf(file, "%s", line) != EOF) {
        if (strcmp(line, "v") == 0) {
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            temp_vertices.push_back(vec3(x, y, z));
        } else if (strcmp(line, "vt") == 0) {
            fscanf(file, "%f %f\n", &x, &y);
            temp_uvs.push_back(vec2(x, y));
        } else if (strcmp(line, "vn") == 0) {
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            temp_normals.push_back(vec3(x, y, z));
        } else if (strcmp(line, "f") == 0) {
            for (int i = 0; i < 3; i++) {
                uint32_t vi, ui, ni;
                if (fscanf(file, "%d/%d/%d", &vi, &ui, &ni) != 3) {
                    fprintf(stderr, "unsupported format\n");
                    fclose(file);
                    exit(-1);
                }
                vertexIndices.push_back(vi);
                uvIndices.push_back(ui);
                normalIndices.push_back(ni);
            }
        } else { // comment
            fgets(line, sizeof(line), file);
        }
    }

    // convert indices to vectors
    for (int i = 0; i < vertexIndices.size(); i++) {
        uint32_t vertexIndex = vertexIndices[i];
        uint32_t uvIndex = uvIndices[i];
        uint32_t normalIndex = normalIndices[i];

        vec3 vertex = temp_vertices[vertexIndex - 1];
        vec2 uv = temp_uvs[uvIndex - 1];
        vec3 normal = temp_normals[normalIndex - 1];

        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }

    fclose(file);
}

struct PackedVertex {
    vec3 position;
    vec2 uv;
    vec3 normal;
    bool operator<(const PackedVertex that) const
    {
        return memcmp((void *)this, (void *)&that, sizeof(PackedVertex)) > 0;
    };
};

typedef map<PackedVertex, uint16_t>::iterator mpii;

void IndexVBO(vector<vec3> &vertices,
              vector<vec2> &uvs,
              vector<vec3> &normals,
              vector<uint16_t> &indices)
{
    map<PackedVertex, uint16_t> VertexToOutIndex;
    vector<vec3> out_vertices;
    vector<vec2> out_uvs;
    vector<vec3> out_normals;

    for (int i = 0; i < vertices.size(); i++) {
        PackedVertex packed = {vertices[i], uvs[i], normals[i]};
        mpii it = VertexToOutIndex.find(packed);

        if (it != VertexToOutIndex.end()) {
            indices.push_back(it->second);
        } else {
            out_vertices.push_back(vertices[i]);
            out_uvs.push_back(uvs[i]);
            out_normals.push_back(normals[i]);
            uint16_t newindex = (uint16_t)out_vertices.size() - 1;
            indices.push_back(newindex);
            VertexToOutIndex[packed] = newindex;
        }
    }

    vertices = out_vertices;
    uvs = out_uvs;
    normals = out_normals;
    printf("reduce %lu to %lu\n", indices.size(), vertices.size());
}
