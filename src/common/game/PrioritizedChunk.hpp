#pragma once

#include "ChunkPos.hpp"

struct PrioritizedChunk
{
    ChunkPos pos;
    bool checkNeighbors;
    float distance;
    bool operator<(const PrioritizedChunk& other) const {
        return distance > other.distance; // reverse for min-heap
    }
};