#pragma once

#include <SDL3/SDL.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <mutex>
#include <oneapi/tbb/concurrent_queue.h>

#include "GameContext.hpp"
#include "Terrain.hpp"
#include "../gfx/ChunkRenderer.hpp"
#include "../util/ThreadPool.hpp"
#include "Chunk.hpp"
#include "ChunkPos.hpp"

class World {
    ChunkRenderer renderer;
    Terrain terrain;
    ThreadPool threadPool;
    void requestChunkLoad(ChunkPos pos);
    void LoadChunks(GameContext *c);
public:

    // TODO: make this part of the GameContext?
    // Shared queue and synchronization tools
    std::mutex queueLoadMutex;
    std::condition_variable queueCV;
    tbb::concurrent_queue<ChunkPos> chunkLoadQueue;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<Chunk>> chunks; // this is a map of chunk positions to chunks loaded in memory
    World();
    ~World();
    bool AreAllNeighborsLoaded(const ChunkPos &pos);
    void Init(GameContext *c);
    void OnEvent(GameContext *c, SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    bool ChunkReady(const ChunkPos &pos);
    bool ChunkLoaded(const ChunkPos &pos);
    BLOCK_ID_TYPE GetBlockId(const ChunkPos &pos, int x, int y, int z);
    void Render(GameContext *c);
};