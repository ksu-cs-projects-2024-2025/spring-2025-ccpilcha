#pragma once

#include <glm/glm.hpp>

#define BLOCK_ID_TYPE uint8_t
#define CHUNK_X_SIZE 32
#define CHUNK_Y_SIZE 32
#define CHUNK_Z_SIZE 32
#define CHUNK_SIZE (CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE)
#define CHUNK_POS_X_TYPE int64_t
#define CHUNK_POS_Y_TYPE int64_t
#define CHUNK_POS_Z_TYPE int64_t

#define CHUNK_DATA std::array<std::array<BLOCK_ID_TYPE, CHUNK_X_SIZE>, CHUNK_Y_SIZE>

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

    double distanceXY(const ChunkPos &other) const
    {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
    

    double distance(const ChunkPos &other) const
    {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
    }
    
    /**
     * @brief Helpful way to find the chunk position relative to one point
     * This will be a helpful conversion
     * We should be okay with integer casting; the distance shouldn't ever need to 
     * exceed double precision since the point is that the input is relative
     * 
     * @param input 
     * @return ChunkPos 
     */
    ChunkPos operator+(const glm::vec3 &input) const {
        return ChunkPos{x + (int64_t)floor(input.x / CHUNK_X_SIZE), y + (int64_t)floor(input.y / CHUNK_Y_SIZE), z + (int64_t)floor(input.z / CHUNK_Z_SIZE)};
    }

    ChunkPos operator+(const glm::dvec3 &input) const {
        return ChunkPos{x + (int64_t)floor(input.x / CHUNK_X_SIZE), y + (int64_t)floor(input.y / CHUNK_Y_SIZE), z + (int64_t)floor(input.z / CHUNK_Z_SIZE)};
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