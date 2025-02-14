#include "Terrain.hpp"

#include <array>


std::vector<CHUNK_DATA> Terrain::generateChunk(ChunkPos pos)
{
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    
    // TODO: implement this
    // this needs a Perlin noise generator

    // will return an empty chunk for now with one grass block
    CHUNK_DATA layer = CHUNK_DATA();
    for (int x = 0; x < CHUNK_X_SIZE; x++){
    for (int y = 0; y < CHUNK_Y_SIZE; y++){
        layer[y][x] = 0;
    }
    }
    layer[5][0] = 1;
    layer[2][2] = 1;
    data.push_back(layer);
    return data;
}