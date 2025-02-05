#pragma once

#include <glad/gl.h>

#include "ChunkVertex.hpp"
#include "../game/Chunk.hpp"

// Note: Should only be ran from the main thread!
class ChunkMesh
{
protected:
    bool loaded;
    GLuint vbo;
    //std::vector<uint> vertices; // we will store vertices as a singular integer! (someday)
    std::vector<ChunkVertex> vertices; // for now we must succumb to simplicity :(
    Chunk *chunk;
public:
    ChunkMesh();
    ~ChunkMesh();
    void Update();
    void UploadToGPU();
    void Render();
};