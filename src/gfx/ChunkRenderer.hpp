#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <SDL3/SDL.h>
#include <thread>

#include "../game/GameContext.hpp"
#include "../util/ThreadPool.hpp"
#include "Mesh.hpp"
#include "VertexAttribute.hpp"
#include "ChunkPos.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"
#include <shared_mutex>

class World;

class ChunkRenderer
{   
    GLuint ubo;
    Shader chunkShader, gizmoShader;
    Mesh gizmoMesh;
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