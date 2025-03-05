#pragma once

#include <vector>

#include "VertexAttribute.hpp"


class Mesh
{
    GLuint vao, vbo;
    const void* data;
    GLsizei size;
public:
    Mesh();
    ~Mesh();
    void SetData(const void* data, GLsizei size);
    void SetAttributes(std::vector<VertexAttribute> attributes);
    void Render(GLenum mode);
    void RenderInstance(GLenum mode, int count);
    void RenderInstanceAuto(GLenum mode, int verts, int count);
};