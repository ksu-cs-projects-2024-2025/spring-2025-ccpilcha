#pragma once

#include "ChunkPos.hpp"

struct PrioritizedChunk
{
    ChunkPos pos;
    float distance;
    bool operator<(const PrioritizedChunk& other) const {
        return distance > other.distance; // reverse for min-heap
    }
};