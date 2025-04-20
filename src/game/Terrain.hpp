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
    std::vector<float> generateStoneHeightMap(ChunkPos pos);
    std::vector<bool> generateCaves(ChunkPos pos);
    bool getVisibilityFlags(ChunkPos pos);
    std::pair<bool, std::vector<CHUNK_DATA>> generateChunk(ChunkPos pos);
};