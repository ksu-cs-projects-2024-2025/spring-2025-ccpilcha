#include "Mesh.hpp"

#include "Shader.hpp"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::Init(GameContext *c)
{
    glCall(glGenVertexArrays(1, &this->vao));
    glCall(glGenBuffers(1, &this->vbo));

    c->glCleanupQueue.emplace([=]() {
        glCall(glDeleteVertexArrays(1, &this->vao));
        glCall(glDeleteBuffers(1, &this->vbo));
    });
}

void Mesh::SetData(const void *data, GLsizei size)
{
    this->data = data;
    this->size = size;
    glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void Mesh::SetAttributes(std::vector<VertexAttribute> attributes)
{
    glCall(glBindVertexArray(this->vao));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
    int i = 0;
    for (auto &attribute : attributes)
    {
        attribute.Enable(i++);
    }
}

void Mesh::Render(GLenum mode) 
{
    glCall(glBindVertexArray(this->vao));
    glCall(glDrawArrays(mode, 0, this->size));
}

void Mesh::RenderInstance(GLenum mode, int count)
{
    glCall(glBindVertexArray(this->vao));
    glCall(glDrawArraysInstanced(mode, 0, this->size, count));
}

void Mesh::RenderInstanceAuto(GLenum mode, int verts, int count)
{
    glCall(glBindVertexArray(this->vao));
    glCall(glDrawArraysInstanced(mode, 0, verts, count));
}
