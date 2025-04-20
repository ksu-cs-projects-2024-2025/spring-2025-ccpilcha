#include "Chunk.hpp"
#include <math.h>
#include <ostream>
#include <array>

Chunk::Chunk() : blocks()
{
}

Chunk::~Chunk()
{
    blocks.clear();
}

void Chunk::Init(GameContext *c)
{
    this->removing.store(false);
}

bool Chunk::IsEmpty()
{
    return blocks.size() == 0;
}

void Chunk::Load(std::vector<CHUNK_DATA> data)
{
    this->blocks.swap(data);
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
}

void Chunk::Clear()
{
    blocks.clear();
    this->loaded = false;
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

nlohmann::json Chunk::To_RLE()
{
    // TODO: I might consider other ways to further compress the data like switching the axis and comparing size
    // Maybe even use a palette system
    //std::ofstream outFile(".data", std::ios::binary | std::ios::app);

    /*  Formatting:
     * - chunkPos
     * - z layer count
     * - block stuff
     * - (OPTIONAL) pad with date?
     */
    nlohmann::json json; // for now I will do json
    json["x"] = this->pos.x;
    json["y"] = this->pos.y;
    json["z"] = this->pos.z;
    json["z-layers"] = blocks.size();

    BLOCK_ID_TYPE blockId;
    uint16_t count = 0;
    if (blocks.size() > 0) blockId = blocks[0][0][0];
    for (int z = 0; z < blocks.size(); z++) {
    for (int y = 0; y < CHUNK_Y_SIZE; y++) {
    for (int x = 0; x < CHUNK_X_SIZE; x++)
    {
        BLOCK_ID_TYPE newId = blocks[z][y][x];
        if (blockId != newId) {
            json["data"].push_back({count, blockId});
            count = 1;
            blockId = newId;
        } else {
            count++;
        }
    }}}
    json["data"].push_back({count, blockId});

    return json;
}

void Chunk::Load_RLE(nlohmann::json rle)
{
    blocks.clear();
    blocks.resize(rle["z-layers"]);

    this->pos = ChunkPos({rle["x"], rle["y"], rle["z"]});

    int index = 0;
    for (const auto& pair : rle["data"]) {
        int count = pair[0];
        int blockId = pair[1];

        for (int i = 0; i < count; i++) {
            int z = index / (CHUNK_Y_SIZE * CHUNK_X_SIZE);
            int y = (index / CHUNK_X_SIZE) % CHUNK_Y_SIZE;
            int x = index % CHUNK_X_SIZE;

            if (z >= CHUNK_Z_SIZE) {
                throw std::runtime_error("RLE overflow: too many blocks!");
            }

            blocks[z][y][x] = blockId;
            index++;
        }
    }
    this->loaded = true;
}