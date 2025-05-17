/**
 * @file ChunkRenderer.hpp
 * @author Cameron Pilchard
 * @brief 
 * @version 0.1
 * @date 2025-03-07
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
#include "game/BlockInfo.hpp"
#include "Shader.hpp"

class World;

class ChunkRenderer
{   
    // TODO: i am considering if I want to switch to using a UBO instead of changing uniform variables per chunk draw call
    GLuint ubo;
    // this is the shader object for all chunks
    Shader chunkShader, queryShader;
    Mesh queryMesh;
    // this is the thread worker pool which will process/render chunks
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<ThreadPool> threadPoolP;

    std::thread loadThread;
    
    World* world;
     std::atomic<size_t> meshCount = 0;
    

    std::pair<std::vector<ChunkVertex>, std::vector<ChunkVertex>> GenerateGreedyMesh(const Chunk &chunk, GameContext *c);
    void RenderChunkAt(GameContext *c, PrioritizedChunk pos);
    void RenderChunks(GameContext *c);
    void drawChunkAABB(GameContext *c, const ChunkPos& pos);
public:
    std::atomic<int> chunkGenFrameId = 0;
    // the render queue is for chunks which are ready to be rendered
    // the remove queue is for chunks which are ready to be deleted
    tbb::concurrent_priority_queue<PrioritizedChunk> chunkRenderQueue;
    tbb::concurrent_queue<ChunkPos> chunkRemoveQueue;
    // instead of creating more mesh instances, we can just use one from here
    tbb::concurrent_queue<std::shared_ptr<ChunkMesh>> freeMeshes;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>> chunkMeshes;
    std::mutex queueRenderMutex;
    std::condition_variable queueCV;
    bool loadSignal = false;
    ChunkRenderer(World*);
    ~ChunkRenderer();
    bool IsLoaded(ChunkPos pos);
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};