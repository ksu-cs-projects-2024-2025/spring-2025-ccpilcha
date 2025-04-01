/**
 * @file ChunkRenderer.hpp
 * @author Cameron Pilchard
 * @brief 
 * @version 0.1
 * @date 2025-03-07
 * 
 * @copyright Copyright (c) 2025
 * 
 * This is the manager for the rendering of chunks 
 */

#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <oneapi/tbb/concurrent_priority_queue.h>
#include <SDL3/SDL.h>
#include <thread>
#include <shared_mutex>

#include "game/GameContext.hpp"
#include "game/PrioritizedChunk.hpp"
#include "game/ChunkPos.hpp"
#include "util/ThreadPool.hpp"
#include "Mesh.hpp"
#include "VertexAttribute.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"

class World;

class ChunkRenderer
{   
    // TODO: i am considering if I want to switch to using a UBO instead of changing uniform variables per chunk draw call
    GLuint ubo;
    // this is the shader object for all chunks
    Shader chunkShader;
    // this is the thread worker pool which will process/render chunks
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<ThreadPool> threadPoolP;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>> chunkMeshes;

    std::thread loadThread;

    World* world;
    void RenderChunkAt(PrioritizedChunk pos);
    void RenderChunks(GameContext *c);
public:
    std::atomic<int> chunkGenFrameId = 0;
    // the render queue is for chunks which are ready to be rendered
    // the remove queue is for chunks which are ready to be deleted
    tbb::concurrent_priority_queue<PrioritizedChunk> chunkRenderQueue;
    tbb::concurrent_queue<ChunkPos> chunkRemoveQueue;
    // instead of creating more mesh instances, we can just use one from here
    tbb::concurrent_queue<std::shared_ptr<ChunkMesh>> freeMeshes;
    std::mutex queueRenderMutex;
    std::condition_variable queueCV;
    ChunkRenderer(World*);
    ~ChunkRenderer();
    bool IsLoaded(ChunkPos pos);
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};