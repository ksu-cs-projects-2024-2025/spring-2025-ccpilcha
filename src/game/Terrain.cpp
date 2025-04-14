#include "Terrain.hpp"

#include <array>
#include <iostream>

Terrain::Terrain() : seed(0)
{
    perlin = FastNoise::New<FastNoise::Simplex>();
    FBm = FastNoise::New<FastNoise::FractalFBm>();
    FBm->SetSource(perlin);
    FBm->SetGain(2.0f);
    FBm->SetOctaveCount(5);
    FBm->SetLacunarity(2.6f);
    rgd = FastNoise::New<FastNoise::FractalRidged>();
    rgd->SetSource(perlin);
    rgd->SetGain(0.51f);
    rgd->SetOctaveCount(5);
    rgd->SetLacunarity(2.0f);
}

Terrain::~Terrain()
{
    rgd.reset();
}

float linearSpline(const std::vector<std::pair<float, float>>& points, float x) {
    for (size_t i = 1; i < points.size(); ++i) {
        if (x <= points[i].first) {
            float x0 = points[i - 1].first;
            float y0 = points[i - 1].second;
            float x1 = points[i].first;
            float y1 = points[i].second;
            float t = (x - x0) / (x1 - x0);
            return std::lerp(y0, y1, t);
        }
    }
    return points.back().second;
}

std::vector<float> Terrain::generateHeightMap(ChunkPos pos)
{
    float frequency = 0.00004f;
    float frequencyErosion = 0.00004f;
    float frequencyVariation = 0.0003f;

    // These spline arrays represent functions which take in the noise as input and puts out the terrain height.
    // This continental/erosion/peaks&valleys technique is inspired largely by Minecraft
    // I also credit this talk by Henrik Kniberg which does a fantastic explanation of the technique!
    // https://www.youtube.com/watch?v=ob3VwY4JyzE&t=1407s 
    std::vector<std::pair<float, float>> continentalSplines = {
        {-1.0f, -500.0f},  // Inland plateau
        {-0.8f, -475.0f},  // Deep ocean
        {-0.6f, -370.0f},  // Deep ocean
        {-0.55f,-160.0f},   // Sea level
        {-0.45f, -12.0f},   // Sea level
        {-0.4f, -10.0f},   // Sea level
        {-0.3f,  0.0f},   // Sea level
        { 0.0f,  5.0f},  // Coastline
        { 0.15f,  32.0f},  // Coastline
        { 0.25f, 40.0f},  // Coastline
        { 0.3f, 36.0f},  // Coastline
        { 0.6f,  200.0f},  // Mountains
        { 1.0f,  450.0f}  // Mountains
    };

    std::vector<std::pair<float, float>> erosionSplines = {
        {-1.0f, 40.0f},
        {-0.75f, 0.0f}, 
        {-0.4f,  1.0f},
        { 0.0f, -32.0f},
        { 0.4f,-35.0f}, 
        { 0.5f,-30.0f}, 
        { 0.8f,-30.0f}, 
        { 0.9f,-35.0f}, 
        { 1.0f,-40.0f} 
    };

    // Let's make some Perlin Noise!
	std::vector<float> continental(CHUNK_X_SIZE * CHUNK_Y_SIZE), erosion(CHUNK_X_SIZE * CHUNK_Y_SIZE), octave3(CHUNK_X_SIZE * CHUNK_Y_SIZE), variation(CHUNK_X_SIZE * CHUNK_Y_SIZE), variation2(CHUNK_X_SIZE * CHUNK_Y_SIZE);

	rgd->GenUniformGrid2D(continental.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency, seed);
	FBm->GenUniformGrid2D(erosion.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyErosion, seed + 1);
	FBm->GenUniformGrid2D(variation.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyVariation, seed);

	std::vector<float> height;
	for (int y = 0; y < CHUNK_Y_SIZE; y++)
	{
		for (int x = 0; x < CHUNK_X_SIZE; x++)
		{
            float h = continental[x + CHUNK_X_SIZE * y];
            float h2 = erosion[x + CHUNK_X_SIZE * y];
            float h4 = variation[x + CHUNK_X_SIZE * y];
			height.push_back(linearSpline(continentalSplines, h) + linearSpline(erosionSplines, h2) * 2.0f + 10 * h4);
		}
	}

	return height;
}

uint16_t Terrain::getVisibilityFlags(ChunkPos pos)
{
    std::vector<float> heightMap = generateHeightMap(pos);
    // Get the vertical boundaries for this chunk
    int64_t chunkMinZ = pos.z * CHUNK_Z_SIZE;
    int64_t chunkMaxZ = chunkMinZ + CHUNK_Z_SIZE;
    // TODO: focus on faces of each chunk
    // for now// Check if any of the height values falls within the chunk's vertical range
    
    for (int y = 0; y < CHUNK_Y_SIZE; y++) {
    for (int x = 0; x < CHUNK_X_SIZE; x++) {
        int64_t h = floorf(heightMap[x + CHUNK_X_SIZE * y]);
        if (h >= chunkMinZ && h < chunkMaxZ || h<=0 && pos.z == 0)
        {
            return 1; // Visible
        }
    }
    }

    return 0;
}

std::vector<CHUNK_DATA> Terrain::generateChunk(ChunkPos pos)
{
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    std::vector<float> heightMap = generateHeightMap(pos);
    
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
            if (trueHeight == (int64_t)floorf(heightMap[x + CHUNK_X_SIZE * y])) {
                layer[y][x] = heightMap[x + CHUNK_X_SIZE * y] > 3.5f ? 1 : 4;
            } else if (trueHeight < (int64_t)floorf(heightMap[x + CHUNK_X_SIZE * y]))
            {
                layer[y][x] = 2;
            } 
            else if (trueHeight > (int64_t)floorf(heightMap[x + CHUNK_X_SIZE * y]) && trueHeight <= 0)
            {
                layer[y][x] = 7;
            }
        }
        }
        data.push_back(layer);
    }
    return data;
}