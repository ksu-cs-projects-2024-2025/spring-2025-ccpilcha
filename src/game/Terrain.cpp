#include "Terrain.hpp"

#include <array>
#include <iostream>

Terrain::Terrain() : seed(0)
{
    noise = FastNoise::New<FastNoise::Perlin>();
}

Terrain::~Terrain()
{
    noise.reset();
}

std::vector<CHUNK_POS_Z_TYPE> Terrain::generateHeightMap(ChunkPos pos)
{
    // Let's make some Perlin Noise!
	std::vector<float> layer1(CHUNK_X_SIZE * CHUNK_Y_SIZE), layer2(CHUNK_X_SIZE * CHUNK_Y_SIZE), layer3(CHUNK_X_SIZE * CHUNK_Y_SIZE), layer4(CHUNK_X_SIZE * CHUNK_Y_SIZE), layer5(CHUNK_X_SIZE * CHUNK_Y_SIZE);

	noise->GenUniformGrid2D(layer1.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, 0.05, seed);
	noise->GenUniformGrid2D(layer2.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, 0.01, seed);
	noise->GenUniformGrid2D(layer3.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, 0.005, seed);
	noise->GenUniformGrid2D(layer4.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, 0.01, seed);
	noise->GenUniformGrid2D(layer5.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, 0.001, seed);

	std::vector<CHUNK_POS_Z_TYPE> height;
	for (int y = 0; y < CHUNK_Y_SIZE; y++)
	{
		for (int x = 0; x < CHUNK_X_SIZE; x++)
		{
			height.push_back((int64_t)floorf(((layer1[x + CHUNK_X_SIZE * y] + 10 * layer2[x + CHUNK_X_SIZE * y] + 30 * layer4[x + CHUNK_X_SIZE * y] * layer3[x + CHUNK_X_SIZE * y] * pow(2*layer5[x + CHUNK_X_SIZE * y],2) + 100 * layer5[x + CHUNK_X_SIZE * y]))));
		}
	}

	return height;
}

uint16_t Terrain::getVisibilityFlags(ChunkPos pos)
{
    std::vector<CHUNK_POS_Z_TYPE> heightMap = generateHeightMap(pos);
    // Get the vertical boundaries for this chunk
    int64_t chunkMinZ = pos.z * CHUNK_Z_SIZE;
    int64_t chunkMaxZ = chunkMinZ + CHUNK_Z_SIZE;
    // TODO: focus on faces of each chunk
    // for now// Check if any of the height values falls within the chunk's vertical range
    
    for (int y = 0; y < CHUNK_Y_SIZE; y++) {
    for (int x = 0; x < CHUNK_X_SIZE; x++) {
        int64_t h = heightMap[x + CHUNK_X_SIZE * y];
        if (h >= chunkMinZ && h < chunkMaxZ)
        {
            return 1; // Visible
        }
    }
    }

    if (pos.z == 0)
    {
        return 0;
    }

    return 0;
}

std::vector<CHUNK_DATA> Terrain::generateChunk(ChunkPos pos)
{
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    std::vector<CHUNK_POS_Z_TYPE> heightMap = generateHeightMap(pos);
    
    // TODO: implement this
    // this needs a Perlin noise generator

    // will return an empty chunk for now with one grass block
    for (int64_t z = 0; z < CHUNK_Z_SIZE; z++)
    {
        int64_t trueHeight = pos.z * CHUNK_Z_SIZE + z;
        int64_t diffHeight;
        CHUNK_DATA layer = CHUNK_DATA();
        for (int y = 0; y < CHUNK_Y_SIZE; y++){
        for (int x = 0; x < CHUNK_X_SIZE; x++){
            if (trueHeight == heightMap[x + CHUNK_X_SIZE * y]) {
                layer[y][x] = trueHeight > 0 ? 1 : 4;
            } else if (trueHeight < heightMap[x + CHUNK_X_SIZE * y])
            {
                layer[y][x] = 2;
            }
        }
        }
        data.push_back(layer);
    }
    return data;
}