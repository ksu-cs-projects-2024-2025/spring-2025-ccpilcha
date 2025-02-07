#include "Terrain.hpp"

#include <array>


std::vector<CHUNK_DATA> Terrain::generateChunk(ChunkPos pos)
{
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    
    // TODO: implement this
    // this needs a Perlin noise generator

    // will return an empty chunk for now
    CHUNK_DATA layer = CHUNK_DATA();
    layer[0][0] = 1;
    data.push_back(layer);
    return data;
}