#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/concurrent_queue.h>

#include <SDL3/SDL.h>
#include <unordered_map>
#include <queue>

#include <thread>

#include "../game/GameContext.hpp"
#include "VertexAttribute.hpp"
#include "ChunkPos.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"
#include <shared_mutex>

#include "../util/ThreadPool.hpp"

class World;

class ChunkRenderer
{   
    GLuint ssbo;
    Shader chunkShader;
    ThreadPool threadPool;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>> chunkMeshes;
    World* world;
    void RenderChunkAt(ChunkPos pos);
    void RenderChunks();

public:
    tbb::concurrent_queue<ChunkPos> chunkRenderQueue;
    std::mutex queueRenderMutex;
    std::condition_variable queueCV;
    ChunkRenderer(World*);
    ~ChunkRenderer();
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};