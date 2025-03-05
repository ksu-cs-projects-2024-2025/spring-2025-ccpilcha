#pragma once

#include <cstdint>
#include <vector>
#include <FastNoise/FastNoise.h>

#include "Chunk.hpp"

struct Terrain {
    uint64_t seed;

    FastNoise::SmartNode<FastNoise::Perlin> noise;
    Terrain();
    std::vector<CHUNK_POS_Z_TYPE> generateHeightMap(ChunkPos pos);
    uint16_t getVisibilityFlags(ChunkPos pos);
    std::vector<CHUNK_DATA> generateChunk(ChunkPos pos);
};