#include "Terrain.hpp"

#include <array>
#include <iostream>

Terrain::Terrain() : seed(1000)
{
    perlin = FastNoise::New<FastNoise::Simplex>();
    FBm = FastNoise::New<FastNoise::FractalFBm>();
    FBm->SetSource(perlin);
    FBm->SetOctaveCount(5);
    FBm->SetLacunarity(2.6f);
    rgd = FastNoise::New<FastNoise::FractalRidged>();
    rgd->SetSource(perlin);
    rgd->SetGain(0.51f);
    rgd->SetOctaveCount(5);
    rgd->SetLacunarity(2.1f);
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
    float frequency = 0.00006f;
    float frequencyErosion = 0.0002f;
    float frequencyVariation = 0.0004f;
    float frequencyCalm = 0.00001f;

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
        {-0.3f,  3.0f},   // Sea level
        { 0.0f, -2.0f},  // Coastline
        { 0.15f,  32.0f},  // Coastline
        { 0.25f, 40.0f},  // Coastline
        { 0.3f, 36.0f},  // Coastline
        { 0.55f,  100.0f},  // Mountains
        { 0.8f,  150.0f},  // Mountains
        { 1.0f,  250.0f}  // Mountains
    };

    std::vector<std::pair<float, float>> erosionSplines = {
        {-1.0f, 40.0f},
        { 0.0f, -32.0f},
        { 0.4f,-35.0f}, 
        { 0.5f,-30.0f}, 
        { 0.8f,-30.0f}, 
        { 0.9f,-35.0f}, 
        { 1.0f,-40.0f} 
    };

    std::vector<std::pair<float, float>> calmSplines = {
        {-1.0f, 1.0f},
        { 1.0f, 0.0f} 
    };

    // Let's make some Perlin Noise!
	std::vector<float> continental(CHUNK_X_SIZE * CHUNK_Y_SIZE), 
                        erosion(CHUNK_X_SIZE * CHUNK_Y_SIZE), 
                        variation(CHUNK_X_SIZE * CHUNK_Y_SIZE), 
                        calm(CHUNK_X_SIZE * CHUNK_Y_SIZE);

	rgd->GenUniformGrid2D(continental.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency, seed);
	FBm->GenUniformGrid2D(erosion.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyErosion, seed);
	FBm->GenUniformGrid2D(variation.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyVariation, seed);
	FBm->GenUniformGrid2D(calm.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyCalm, seed);

	std::vector<float> height;
	for (int y = 0; y < CHUNK_Y_SIZE; y++)
	{
		for (int x = 0; x < CHUNK_X_SIZE; x++)
		{
            float h = continental[x + CHUNK_X_SIZE * y];
            float h2 = erosion[x + CHUNK_X_SIZE * y];
            float h3 = variation[x + CHUNK_X_SIZE * y];
            float h4 = calm[x + CHUNK_X_SIZE * y];

            float hFinal = linearSpline(continentalSplines, h) + linearSpline(erosionSplines, h2) + 5 * h3;
			height.push_back(hFinal);
		}
	}

	return height;
}

std::vector<float> Terrain::generateStoneHeightMap(ChunkPos pos)
{
    float frequency = 0.02f;

    // These spline arrays represent functions which take in the noise as input and puts out the terrain height.
    // This continental/erosion/peaks&valleys technique is inspired largely by Minecraft
    // I also credit this talk by Henrik Kniberg which does a fantastic explanation of the technique!
    // https://www.youtube.com/watch?v=ob3VwY4JyzE&t=1407s 
    std::vector<std::pair<float, float>> continentalSplines = {
        {-1.0f, -3.0f},  // Inland plateau
        { 1.0f, -6.0f},  // Inland plateau
    };

    // Let's make some Perlin Noise!
	std::vector<float> continental(CHUNK_X_SIZE * CHUNK_Y_SIZE);

	FBm->GenUniformGrid2D(continental.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency, seed);

	std::vector<float> height;
	for (int y = 0; y < CHUNK_Y_SIZE; y++)
	{
		for (int x = 0; x < CHUNK_X_SIZE; x++)
		{
            float h = continental[x + CHUNK_X_SIZE * y];
			height.push_back(linearSpline(continentalSplines, h));
		}
	}

	return height;
}

std::vector<float> Terrain::generateRivers(ChunkPos pos)
{
    float frequency = 0.00007;
    float frequency2 = 0.00025;
    float frequency3 = 0.0008;
    float frequencyWorms = 0.0008;

    std::vector<std::pair<float, float>> splines = {
        {-1.0f, 0.0f},  // Inland plateau
        {-0.06f, 0.0f},  // Inland plateau
        {-0.04f,-2.0f},  // Inland plateau
        {-0.02f,-10.0f},  // Inland plateau
        { 0.0f,-12.0f},  // Inland plateau
        { 0.02f,-10.0f},  // Inland plateau
        { 0.04f,-2.0f},  // Inland plateau
        { 0.06f, 0.0f},  // Inland plateau
        { 1.0f, 0.0f},  // Inland plateau
    };

    std::vector<std::pair<float, float>> splines2 = {
        {-1.0f, 1.0f},  // Inland plateau
        {-0.5f, 1.0f},  // Inland plateau
        { 0.0f, 0.0f},  // Inland plateau
        { 1.0f, 0.0f},  // Inland plateau
    };

    std::vector<float> riverNoise (CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE),
                       riverNoise2(CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE),
                       riverNoise3(CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE),
                       caveWorms(CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE);

    int SCALE = 4;
    int xSize = CHUNK_X_SIZE / SCALE + 1;
    int ySize = CHUNK_Y_SIZE / SCALE + 1;
    int zSize = CHUNK_Z_SIZE / SCALE + 1;

    rgd->GenUniformGrid3D(caveWorms.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequencyWorms, seed + 2979);
	FBm->GenUniformGrid2D(riverNoise.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency, seed);
	rgd->GenUniformGrid2D(riverNoise2.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency2, seed+1337);
	rgd->GenUniformGrid2D(riverNoise3.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequency3, seed+1293);
    
	std::vector<float> height;
	for (int y = 0; y < CHUNK_Y_SIZE; y++)
	{
		for (int x = 0; x < CHUNK_X_SIZE; x++)
		{
            float h = riverNoise[x + CHUNK_X_SIZE * y];
            float h2 = riverNoise2[x + CHUNK_X_SIZE * y];
            float h3 = riverNoise3[x + CHUNK_X_SIZE * y];
            height.push_back(linearSpline(splines, (h - h2)/2.0f) * linearSpline(splines2, h3));
		}
	}

	return height;
}

std::vector<bool> Terrain::generateCaves(ChunkPos pos)
{
    float frequency = 0.01f;
    float frequency2 = 0.0025f;
    float frequency3 = 0.0025f;
    float frequency4 = 0.02f;
    float frequencyWidth = 0.01f;
    float frequencyWorms = 0.01f;
    int SCALE = 4;
    int xSize = CHUNK_X_SIZE / SCALE + 1;
    int ySize = CHUNK_Y_SIZE / SCALE + 1;
    int zSize = CHUNK_Z_SIZE / SCALE + 1;
    std::vector<bool> caveMap(CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE, false);
    std::vector<float> caveNoise(xSize * ySize * zSize), 
                caveWidth(xSize * ySize * zSize), 
                caveVar(xSize * ySize * zSize), 
                caveVar2(xSize * ySize * zSize),
                caveVar3(xSize * ySize * zSize),
                caveVar4(xSize * ySize * zSize),
                caveWorms(xSize * ySize * zSize),
                caveWormsY(xSize * ySize * zSize),
                caveWormsZ(xSize * ySize * zSize);

    std::vector<std::pair<float, float>> splines = {
        {-1.0f, 0.0f},  // Inland plateau
        { 0.0f, 2.0f},  // Inland plateau
        { 1.0f, 8.0f},  // Inland plateau
    };

    rgd->GenUniformGrid3D(caveNoise.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequency, seed);
    rgd->GenUniformGrid3D(caveVar.data(), pos.x * (xSize - 1) + 1000, pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequency, seed + 42);
    rgd->GenUniformGrid3D(caveVar2.data(), pos.x * (xSize - 1) + 1001, pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequency2, seed + 13);
    rgd->GenUniformGrid3D(caveVar3.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequency3, seed + 17);
    rgd->GenUniformGrid3D(caveVar4.data(), pos.x * (xSize - 1) + 23, pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequency4, seed + 7757);
    rgd->GenUniformGrid3D(caveWidth.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequencyWidth, seed + 9937);
    rgd->GenUniformGrid3D(caveWorms.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequencyWorms, seed + 279);
    rgd->GenUniformGrid3D(caveWormsY.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequencyWorms, seed + 2979);
    rgd->GenUniformGrid3D(caveWormsZ.data(), pos.x * (xSize - 1), pos.y * (ySize - 1), pos.z * (zSize - 1), xSize, ySize, zSize, frequencyWorms, seed + 4979);

    auto sampleLowRes = [&](const std::vector<float>& noise, int x, int y, int z) -> float {
        x = std::clamp(x, 0, xSize - 1);
        y = std::clamp(y, 0, ySize - 1);
        z = std::clamp(z, 0, zSize - 1);
        return noise[x + xSize * (y + ySize * z)];
    };    

    auto smoothstep = [](float t) {
        return t * t * (3.0f - 2.0f * t);
    };

    auto interp = [&](const std::vector<float>& noise, int x, int y, int z) -> float {

        float fx = x / (float)SCALE;
        float fy = y / (float)SCALE;
        float fz = z / (float)SCALE;

        int x0 = (int)fx;
        int y0 = (int)fy;
        int z0 = (int)fz;

        int x1 = x0 + 1;
        int y1 = y0 + 1;
        int z1 = z0 + 1;

        float tx = fx - x0;
        float ty = fy - y0;
        float tz = fz - z0;

        float txs = smoothstep(tx);
        float tys = smoothstep(ty);
        float tzs = smoothstep(tz);                

        // Trilinear interpolation
        float c000 = sampleLowRes(noise, x0, y0, z0);
        float c100 = sampleLowRes(noise, x1, y0, z0);
        float c010 = sampleLowRes(noise, x0, y1, z0);
        float c110 = sampleLowRes(noise, x1, y1, z0);
        float c001 = sampleLowRes(noise, x0, y0, z1);
        float c101 = sampleLowRes(noise, x1, y0, z1);
        float c011 = sampleLowRes(noise, x0, y1, z1);
        float c111 = sampleLowRes(noise, x1, y1, z1);

        float c00 = c000 * (1 - txs) + c100 * txs;
        float c01 = c001 * (1 - txs) + c101 * txs;
        float c10 = c010 * (1 - txs) + c110 * txs;
        float c11 = c011 * (1 - txs) + c111 * txs;

        float c0 = c00 * (1 - tys) + c10 * tys;
        float c1 = c01 * (1 - tys) + c11 * tys;

        return c0 * (1 - tzs) + c1 * tzs;
    };

    for (int z = 0; z < CHUNK_Z_SIZE; z++) {
        for (int y = 0; y < CHUNK_Y_SIZE; y++) {
            for (int x = 0; x < CHUNK_X_SIZE; x++) {

                float c0 = interp(caveNoise, x,y,z);
                float c1 = interp(caveVar, x,y,z);
                float c2 = interp(caveVar2, x,y,z);
                float c3 = interp(caveVar3, x,y,z);
                float c4 = interp(caveVar4, x,y,z);
                float cW = linearSpline(splines, interp(caveWidth, x,y,z));
                float cworm = interp(caveWorms, x,y,z);

                bool revene = (std::abs(c1) < 0.1) && (std::abs(c4) < 0.1);
                bool noodle = (std::abs(c2 - c3) < 0.009f) && std::abs(cworm) < 0.1f;
                bool cheese = (std::abs(c2) - std::abs(c3) > 0.75f) ;
            
                caveMap[x + CHUNK_X_SIZE * (y + CHUNK_Y_SIZE * z)] = cheese || revene || noodle;
            }
        }
    }    

    // worms for nice cave paths


    return caveMap;
}

bool Terrain::getVisibilityFlags(ChunkPos pos)
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
        if (h >= chunkMinZ || h<=0 && pos.z == 0)
        {
            return true; // Visible
        }
    }
    }

    return false;
}

std::pair<bool, std::vector<CHUNK_DATA>> Terrain::generateChunk(ChunkPos pos)
{
    float frequencyVariation = 0.003f;
    float frequency3D = 0.003f;
    std::vector<CHUNK_DATA> data = std::vector<CHUNK_DATA>();
    std::vector<float> heightMap = generateHeightMap(pos);
    std::vector<float> stoneMap = generateStoneHeightMap(pos);
    
    std::vector<bool> caveMap = generateCaves(pos);
    std::vector<float> riverMap = generateRivers(pos);
    
	std::vector<float> variation(CHUNK_X_SIZE * CHUNK_Y_SIZE);
	std::vector<float> stuff(CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE);

	FBm->GenUniformGrid2D(variation.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, frequencyVariation, seed);
	FBm->GenUniformGrid3D(stuff.data(), pos.x * CHUNK_X_SIZE, pos.y * CHUNK_Y_SIZE, pos.z * CHUNK_Z_SIZE, CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE, frequencyVariation, seed + 42);
    
    bool hasAir = false;
    for (int64_t z = 0; z < CHUNK_Z_SIZE; z++)
    {
        int64_t trueHeight = pos.z * CHUNK_Z_SIZE + z;
        int64_t diffHeight;
        CHUNK_DATA layer = CHUNK_DATA();
        for (int y = 0; y < CHUNK_Y_SIZE; y++){
        for (int x = 0; x < CHUNK_X_SIZE; x++){
            int64_t calcH = floorf(heightMap[x + CHUNK_X_SIZE * y]) + floorf(riverMap[x + CHUNK_X_SIZE * y]) * std::clamp(1.0f - (heightMap[x + CHUNK_X_SIZE * y]/40.f), 0.0f, 1.0f);
            
            if (trueHeight == calcH) {
                BLOCK_ID_TYPE block = 4;
                if (heightMap[x + CHUNK_X_SIZE * y] > 250.5f + 30.0f * variation[x + CHUNK_X_SIZE * y]) block = 8;
                else if (heightMap[x + CHUNK_X_SIZE * y] > 3.5f) block = 1;
                layer[y][x] = block;
            } else if (trueHeight < calcH)
            {
                layer[y][x] = 2;
            } 
            else if (trueHeight > calcH && trueHeight <= 0)
            {
                layer[y][x] = 7;
            } else {
                hasAir = true;
            }

            if (stuff[x + CHUNK_X_SIZE * (y + CHUNK_Y_SIZE * z)] >= std::clamp(calcH/100.f, 0.0f, 1.0f)) layer[y][x] = 0;

            if (trueHeight <= calcH + (int64_t)floorf(stoneMap[x + CHUNK_X_SIZE * y]))
            {
                layer[y][x] = 3;
            }

            if (trueHeight <= calcH && caveMap[x + CHUNK_X_SIZE * (y + CHUNK_Y_SIZE * z)])
            {
                layer[y][x] = 0;
            }
        }
        }
        data.push_back(layer);
    }
    return {hasAir, data};
}