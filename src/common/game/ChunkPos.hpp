#pragma once

#include <glm/glm.hpp>
#include <string>

#include "ChunkConstants.hpp"

struct ChunkPos
{
    CHUNK_POS_X_TYPE x;
    CHUNK_POS_Y_TYPE y;
    CHUNK_POS_Z_TYPE z;
    
    double distance(const ChunkPos &other) const
    {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
    }

    double distanceXY(const ChunkPos &other) const
    {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }

    bool operator==(const ChunkPos &other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const ChunkPos &other) const
    {
        return x != other.x || y != other.y || z != other.z;
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

    friend std::ostream& operator<<(std::ostream& os, const ChunkPos &pos) {
        os << pos.toString();
        return os;
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

    static ChunkPos adjust(const ChunkPos& pos, int& x, int& y, int& z) 
    {
        int dx = static_cast<int>(std::floor(static_cast<double>(x) / CHUNK_X_SIZE));
        int dy = static_cast<int>(std::floor(static_cast<double>(y) / CHUNK_Y_SIZE));
        int dz = static_cast<int>(std::floor(static_cast<double>(z) / CHUNK_Z_SIZE));

        x -= dx * CHUNK_X_SIZE;
        y -= dy * CHUNK_Y_SIZE;
        z -= dz * CHUNK_Z_SIZE;

        return ChunkPos{pos.x + dx, pos.y + dy, pos.z + dz};
    }

    static ChunkPos adjust(const ChunkPos& cPos, glm::dvec3& pos) 
    {
        int dx = static_cast<int>(std::floor(static_cast<double>(pos.x) / CHUNK_X_SIZE));
        int dy = static_cast<int>(std::floor(static_cast<double>(pos.y) / CHUNK_Y_SIZE));
        int dz = static_cast<int>(std::floor(static_cast<double>(pos.z) / CHUNK_Z_SIZE));

        pos.x -= dx * CHUNK_X_SIZE;
        pos.y -= dy * CHUNK_Y_SIZE;
        pos.z -= dz * CHUNK_Z_SIZE;
        return ChunkPos{cPos.x + dx, cPos.y + dy, cPos.z + dz};
    }

    glm::dvec3 abs(const glm::dvec3& pos)
    {
        return glm::dvec3({CHUNK_X_SIZE * this->x + pos.x,
                           CHUNK_Y_SIZE * this->y + pos.y,
                           CHUNK_Z_SIZE * this->z + pos.z});
    }

    std::string toString() const {
        return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "}";
    }
};


// Hash function for ChunkPos
namespace std {
	template <>
	struct hash<ChunkPos> {
		std::size_t operator()(const ChunkPos& pos) const {
			size_t seed = 1099511628211ULL; // A large prime (FNV offset basis)
			size_t hx = std::hash<size_t>{}(pos.x);
			size_t hy = std::hash<size_t>{}(pos.y);
			size_t hz = std::hash<size_t>{}(pos.z);

			// Mix the hash values with golden ratio
			seed ^= hx + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hy + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hz + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}