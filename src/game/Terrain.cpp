#include "Terrain.hpp"

#include <array>

std::vector<CHUNK_POS_Z_TYPE> Terrain::generateHeightMap(ChunkPos pos)
{
    // Let's make some Perlin Noise!

    

    return std::vector<CHUNK_POS_Z_TYPE>(CHUNK_X_SIZE * CHUNK_Y_SIZE);
}

std::vector<CHUNK_DATA> Terrain::generateChunk(ChunkPos pos)
{
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    
    // TODO: implement this
    // this needs a Perlin noise generator

    // will return an empty chunk for now with one grass block
    for (int z = 0; z < CHUNK_Z_SIZE; z++)
    {
        u_int64_t trueHeight = pos.z * CHUNK_Z_SIZE + z;
        CHUNK_DATA layer = CHUNK_DATA();
        for (int y = 0; y < CHUNK_Y_SIZE; y++){
        for (int x = 0; x < CHUNK_X_SIZE; x++){
            if (pos.z == 0)
            {
                if (z == CHUNK_Z_SIZE - 1)
                {
                    layer[y][x] = 1;
                } else {
                    layer[y][x] = 2;
                }
            } else if (pos.z < 0)
            {
                layer[y][x] = 2;
            }
        }
        }
        data.push_back(layer);
    }
    return data;
}