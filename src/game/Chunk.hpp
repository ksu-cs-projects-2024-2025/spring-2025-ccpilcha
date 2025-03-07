#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>

#include "ChunkPos.hpp"
#include "GameContext.hpp"

// Stores all the block metadata for a chunk
class Chunk
{
    // the idea here is that we can store the block IDs in a 3D array, but condense along the z-axis
    // if a chunk is empty (air) then this will be empty as well
    std::vector<CHUNK_DATA> blocks;
    public:
    std::mutex blockMutex;
    ChunkPos pos;
    bool dirty = true;
    uint16_t visible;
    std::atomic<bool> loaded{false}, inUse{false}, removing{false};

    Chunk(ChunkPos pos);
    ~Chunk();
    bool IsEmpty();
    void Init(GameContext* c);
    void Load(std::vector<CHUNK_DATA> data);
    int size();
    BLOCK_ID_TYPE GetBlockId(int x, int y, int z);
    void SetBlockId(int x, int y, int z, BLOCK_ID_TYPE id);
    static glm::vec3 remainder(glm::vec3 input);
};