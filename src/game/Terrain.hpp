#pragma once

#include <cstdint>
#include <vector>

#include "Chunk.hpp"

struct Terrain {
    uint64_t seed;

    Terrain() : seed(0) {}
    std::vector<CHUNK_POS_Z_TYPE> generateHeightMap(ChunkPos pos);
    std::vector<CHUNK_DATA> generateChunk(ChunkPos pos);
};