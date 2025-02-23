#pragma once

#include <glad/glad.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <thread>
#include <mutex>

#include "ChunkVertex.hpp"
#include "../game/Chunk.hpp"
#include "../game/GameEngine.hpp"

class ChunkMesh
{
protected:
    bool init, loaded;
    GLuint vao, vbo;
    //std::vector<uint> vertices; // we will store vertices as a singular integer! (someday)
    std::vector<ChunkVertex> bufferA, bufferB, *currentBuffer, *renderBuffer; // for now we must succumb to simplicity :(
    bool bufferAFlag = false; // when true, the GPU is loading from bufferA. Otherwise we are using bufferB.
    bool facesVisible[6];
    GLsync bufferSync = 0;

    std::unique_ptr<std::mutex> meshMutex; // Protects vertex data
    std::unique_ptr<std::atomic<bool>> meshSwapping; // Indicates if the mesh is ready
public:
    ChunkMesh();
    ~ChunkMesh();
    void Load(std::vector<ChunkVertex> data);
    void Swap();
    void Update(GameContext *c);
    void UploadToGPU();
    void Render();
};