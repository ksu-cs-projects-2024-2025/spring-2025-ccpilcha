#pragma once

#include <cstdint>
#include <vector>
#include <FastNoise/FastNoise.h>

#include "Chunk.hpp"

struct Terrain {
    uint64_t seed;

    FastNoise::SmartNode<FastNoise::FractalRidged> rgd;
    FastNoise::SmartNode<FastNoise::FractalFBm> FBm;
    FastNoise::SmartNode<FastNoise::Simplex> perlin;
    Terrain();
    ~Terrain();
    std::vector<float> generateHeightMap(ChunkPos pos);
    uint16_t getVisibilityFlags(ChunkPos pos);
    std::vector<CHUNK_DATA> generateChunk(ChunkPos pos);
};