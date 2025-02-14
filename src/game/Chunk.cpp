#include "Chunk.hpp"
#include <math.h>

Chunk::Chunk() : pos(), blocks()
{
}

Chunk::~Chunk()
{
    1;
}

void Chunk::Init(GameContext *c)
{

}

void Chunk::Load(std::vector<CHUNK_DATA> data)
{
    // TODO: mutex?
    this->blocks = data;
    this->dirty = true;
    this->loaded = true;
}

BLOCK_ID_TYPE Chunk::GetBlockId(int x, int y, int z)
{
    if (this == NULL)
    {
        return 0;
    }
    if (z + 1 > blocks.size()) return 0;
    BLOCK_ID_TYPE block = blocks[z][y][x];
    return block;
}

/**
 * @brief This is a quick way to see if a position is outside of the boundaries for a chunk.
 * 
 * @param input 
 * @return std::pair<ChunkPos, glm::vec3> 
 */
glm::vec3 Chunk::remainder(glm::vec3 input)
{
    ChunkPos q{(int64_t)floor(input.x / CHUNK_X_SIZE), (int64_t)floor(input.y / CHUNK_Y_SIZE), (int64_t)floor(input.z / CHUNK_Z_SIZE)};
    glm::vec3 r(input.x - q.x * CHUNK_X_SIZE, input.y - q.y * CHUNK_Y_SIZE, input.z - q.z * CHUNK_Z_SIZE);
    return r;
}
