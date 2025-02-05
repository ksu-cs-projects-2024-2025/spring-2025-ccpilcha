#pragma once

#include <vector>

#include "GameContext.hpp"

#define BLOCK_ID_TYPE uint8_t
#define CHUNK_X_SIZE 16
#define CHUNK_Y_SIZE 16
#define CHUNK_Z_SIZE 16
#define CHUNK_SIZE (CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE)
#define CHUNK_POS_X_TYPE int64_t
#define CHUNK_POS_Y_TYPE int64_t
#define CHUNK_POS_Z_TYPE int64_t

struct ChunkPos
{
    CHUNK_POS_X_TYPE x;
    CHUNK_POS_Y_TYPE y;
    CHUNK_POS_Z_TYPE z;

    bool operator==(const ChunkPos &other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const ChunkPos &other) const
    {
        return x != other.x || y != other.y || z != other.z;
    }
    
    ChunkPos operator+(const ChunkPos &other) const {
        return ChunkPos{x + other.x, y + other.y, z + other.z};
    }

    ChunkPos& operator+=(const ChunkPos &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    ChunkPos operator-(const ChunkPos &other) const {
        return ChunkPos{x - other.x, y - other.y, z - other.z};
    }

    ChunkPos& operator-=(const ChunkPos &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

};

// Stores all the block metadata for a chunk
class Chunk
{
    // the idea here is that we can store the block IDs in a 3D array, but condense along the z-axis
    // if a chunk is empty (air) then this will be empty as well
    std::vector<BLOCK_ID_TYPE[CHUNK_X_SIZE][CHUNK_Y_SIZE]> blocks;
public:
    ChunkPos pos;
    bool dirty;
    bool changed;

    Chunk();
    ~Chunk();
    void Init(GameContext* c);
    void Update(GameContext* c, double deltaTime);
    void Render(GameContext* c);
};


// Hash function for ChunkPos
namespace std {
	template <>
	struct hash<ChunkPos> {
		std::size_t operator()(const ChunkPos& pos) const {
			size_t seed = 1099511628211ULL; // A large prime (FNV offset basis)
			size_t hx = std::hash<int32_t>{}(pos.x);
			size_t hy = std::hash<int32_t>{}(pos.y);
			size_t hz = std::hash<int32_t>{}(pos.z);

			// Mix the hash values with golden ratio
			seed ^= hx + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hy + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hz + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}