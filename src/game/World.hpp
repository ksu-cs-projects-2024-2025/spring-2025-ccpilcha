#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>

#include "GameContext.hpp"
#include "Terrain.hpp"
#include "../gfx/ChunkRenderer.hpp"
#include "Chunk.hpp"
#include "ChunkPos.hpp"

class World {
    ChunkRenderer renderer;
    Terrain terrain;
    void requestChunkLoad(ChunkPos pos);
    void LoadChunks();
public:

    // TODO: make this part of the GameContext?
    // Shared queue and synchronization tools
    std::queue<ChunkPos> chunkLoadQueue;
    std::mutex queueLoadMutex;
    std::condition_variable queueCV;
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> chunks; // this is a map of chunk positions to chunks loaded in memory
    World();
    ~World();
    void Init(GameContext *c);
    void OnEvent(GameContext *c, SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    bool ChunkLoaded(const ChunkPos &pos);
    BLOCK_ID_TYPE GetBlockId(const ChunkPos &pos, int8_t x, int8_t y, int8_t z);
    void Render(GameContext *c);
};