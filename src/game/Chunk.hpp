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
    std::mutex blockMutex;
public:
    std::vector<CHUNK_DATA> blocks;
    ChunkPos pos;
    bool dirty = true;
    bool loaded;

    Chunk();
    ~Chunk();
    bool IsEmpty();
    void Init(GameContext* c);
    void Load(std::vector<CHUNK_DATA> data);
    BLOCK_ID_TYPE GetBlockId(int x, int y, int z);
    static glm::vec3 remainder(glm::vec3 input);
};