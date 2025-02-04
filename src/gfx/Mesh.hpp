#pragma once

#include "Shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

// Note: Should only be ran from the main thread!
class Mesh
{
protected:
    bool loaded;
    VAO vao;
    VBO vbo;
    Shader* shader;
public:
    Mesh();
    ~Mesh();
    void Load();
    void Render();
};