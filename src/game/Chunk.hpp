/**
 * @file Chunk.hpp
 * @author Cameron Pilchard
 * @brief stores chunk metadata
 * @version 0.1
 * @date 2025-03-07
 * 
 * 
 * The way I am currently working this is through a vector which will store 2D arrays of blocks along z-layer.
 * This will save memory when there are air blocks.
 * 
 * I have considered switching to either RLE or Chunk palettes, and the latter seems favorable for constant lookup/write time.
 */

#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <nlohmann/json.hpp>

#include "ChunkPos.hpp"
#include "GameContext.hpp"

// Stores all the block metadata for a chunk
class Chunk
{
    // the idea here is that we can store the block IDs in a 3D array, but condense along the z-axis
    // if a chunk is empty (air) then this will be empty as well
    std::vector<CHUNK_DATA> blocks;
    
public:
    ChunkPos pos;
    // eventually this will store all 15 possible combinations of paths through a chunk.
    // inspiration: https://tomcc.github.io/2014/08/31/visibility-1.html 
    uint16_t visible;
    std::atomic<bool> loaded{false}, rendering{false}, removing{false};

    Chunk();
    ~Chunk();
    bool IsEmpty();
    void Init(GameContext* c);
    void Load(std::vector<CHUNK_DATA> data);
    int size();
    BLOCK_ID_TYPE GetBlockId(int x, int y, int z);
    void SetBlockId(int x, int y, int z, BLOCK_ID_TYPE id);
    void Clear();
    static glm::vec3 remainder(glm::vec3 input);
    nlohmann::json To_RLE();
    void Load_RLE(nlohmann::json);
};