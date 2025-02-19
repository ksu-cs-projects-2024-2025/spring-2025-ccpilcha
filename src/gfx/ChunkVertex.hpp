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
    
    // [5 x] [5 y] [5 z] [3 face] [14 id]
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
        data = (x & 0b11111) << 27 
            | (y & 0b11111) << 22 
            | (z & 0b11111) << 17 
            | (faceNum & 0b111) << 14 
            | (blockId & 0x3ff);
    }
};

