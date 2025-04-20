#pragma once

#include <glad/glad.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <thread>
#include <mutex>
#include <vector>

#include "ChunkVertex.hpp"
#include "../game/Chunk.hpp"
#include "../game/GameEngine.hpp"

class ChunkMesh
{
protected:
    bool init = false;
    GLuint vao, vbo;
    GLuint vaoT, vboT;
    std::vector<ChunkVertex> bufferA, bufferB, *currentBuffer, *renderBuffer; 
    std::vector<ChunkVertex> TbufferA, TbufferB, *currentTBuffer, *renderTBuffer;  // translucent blocks
    bool bufferAFlag = false; // when true, the GPU is loading from bufferA. Otherwise we are using bufferB.
    bool facesVisible[6];
    GLsync bufferSync = 0;

    std::mutex meshMutex; // Protects vertex data
    std::atomic<bool> meshSwapping = false, loaded = false; // Indicates if the mesh is ready
public:
    bool used = false;
    ChunkPos pos;
    std::atomic<bool> rendering = false, isUploaded = false, dirty = false;
    ChunkMesh();
    ~ChunkMesh();
    bool IsReusable() const { return !used; }
    bool isInit() const { return init; }
    bool hasTranslucentBlocks() const { return !currentTBuffer->empty(); }
    bool IsLoaded() const { return loaded.load(); }
    void Init(GLuint vao, GLuint vbo, GLuint vaoT, GLuint vboT);
    void Load(std::vector<ChunkVertex> opaque, std::vector<ChunkVertex> translucent);
    void Swap();
    void Update(GameContext *c);
    void UploadToGPU();
    void RenderOpaque();
    void RenderTransparent();
    void Clear();
};