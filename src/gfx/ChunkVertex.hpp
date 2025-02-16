#pragma once

struct ChunkVertex
{
    //int position[3]; // relative to current chunk position
    //int blockId;
    //int faceNum;
    //	0	FRONT	-X (X = 0)
    //	1	BACK	+X (X = 1)
    //	2	LEFT	-Y (Y = 0)
    //	3	RIGHT	+Y (Y = 1)
    //	4	BOTTOM	-Z (Z = 0)
    //	5	TOP		+Z (Z = 1)
    
    // [4 x] [4 y] [4 z] [3 face] [17 id]
    uint32_t data;

    /**
     * @brief Construct a new Chunk Vertex object
     * Assumes all input are within bounds
     * @param x 
     * @param y 
     * @param z 
     * @param blockId 
     * @param faceNum 
     */
    ChunkVertex(int x, int y, int z, uint blockId, int faceNum)
    {
        data = (x & 0xf) << 28 
            | (y & 0xf) << 24 
            | (z & 0xf) << 20 
            | (faceNum & 0x7) << 17 
            | (blockId & 0x1ffff);
    }
};

