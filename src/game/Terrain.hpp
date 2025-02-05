#include <cstdint>

#include "Chunk.hpp"

struct Terrain {
    uint64_t seed;

    Terrain() : seed(0) {}
    Chunk generateChunk(CHUNK_POS_X_TYPE x, CHUNK_POS_X_TYPE y, CHUNK_POS_X_TYPE z);
};