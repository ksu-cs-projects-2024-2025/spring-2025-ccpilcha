#include "Chunk.hpp"
#include <math.h>
#include <ostream>

Chunk::Chunk() : blocks()
{
}

Chunk::~Chunk()
{
    blocks.clear();
}

void Chunk::Init(GameContext *c)
{

}

bool Chunk::IsEmpty()
{
    return blocks.empty();
}

void Chunk::Load(std::vector<CHUNK_DATA> data)
{
    this->blocks.swap(data);
    this->dirty = true;
    this->loaded = true;
}

int Chunk::size()
{
    return blocks.size();
}

BLOCK_ID_TYPE Chunk::GetBlockId(int x, int y, int z)
{
    if (z + 1 > blocks.size()) return 0;
    BLOCK_ID_TYPE block = blocks[z][y][x];
    return block;
}

void Chunk::SetBlockId(int x, int y, int z, BLOCK_ID_TYPE id)
{
    if (z + 1 > blocks.size()) {
        blocks.resize(z + 1);
    }
    blocks[z][y][x] = id;
    this->dirty = true;
}

void Chunk::Clear()
{
    blocks.clear();
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
