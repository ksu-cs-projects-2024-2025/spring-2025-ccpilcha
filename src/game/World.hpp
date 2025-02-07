#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>

#include "GameContext.hpp"
#include "Terrain.hpp"
#include "../gfx/ChunkRenderer.hpp"
#include "Chunk.hpp"

class World {
    ChunkRenderer renderer;
    Terrain terrain;
    std::unordered_map<ChunkPos, Chunk> chunks; // this is a map of chunk positions to chunks loaded in memory
    void LoadChunks();
public:
    World();
    ~World();
    void Init(GameContext *c);
    void OnEvent(GameContext *c, SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    BLOCK_ID_TYPE GetBlockId(const ChunkPos &pos, int8_t x, int8_t y, int8_t z);
    void Render(GameContext *c);
};