#pragma once

#include <glad/gl.h>
#include <vector>

#include "ChunkVertex.hpp"
#include "../game/Chunk.hpp"
#include "../game/GameEngine.hpp"

class ChunkMesh
{
protected:
    bool loaded;
    GLuint vbo;
    //std::vector<uint> vertices; // we will store vertices as a singular integer! (someday)
    std::vector<ChunkVertex> bufferA, bufferB; // for now we must succumb to simplicity :(
    bool bufferFlag; //this will signify which buffer is being rendered
    bool facesVisible[6];
    Chunk *chunk;
    void addFaceToMesh(uint8_t x, uint8_t y, uint8_t z, uint8_t face, uint16_t blockId);
public:
    ChunkMesh();
    ~ChunkMesh();
    void Update(GameContext *c);
    void UploadToGPU();
    void Render();
};