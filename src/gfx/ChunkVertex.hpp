#pragma once

struct ChunkVertex
{
    //int blockId;
    //int faceNum;
    //	0	FRONT	-X (X = 0)
    //	1	BACK	+X (X = 1)
    //	2	LEFT	-Y (Y = 0)
    //	3	RIGHT	+Y (Y = 1)
    //	4	BOTTOM	-Z (Z = 0)
    //	5	TOP		+Z (Z = 1)
    
    // [5 x] [5 y] [5 z] [3 face] [14 id]
    // [2 id] [8 AO] [5 width1] [5 width2] [12 extra]
    // TODO: how should we use the extra space?
    uint32_t packed0, packed1;

    /**
     * @brief Construct a new Chunk Vertex object
     * Assumes all input are within bounds
     * @param x 
     * @param y 
     * @param z 
     * @param textureId the offset within the texture to use 
     * @param faceNum 
     */
    ChunkVertex(int x, int y, int z, int textureId, int faceNum, int AO)
    {
        packed0 = (x & 0b11111) << 27 
            | (y & 0b11111) << 22 
            | (z & 0b11111) << 17 
            | (faceNum & 0b111) << 14 
            | ((textureId >> 2) & 0x3fff);

        packed1 = (textureId & 0b11) << 30
            | (AO & 0xFF) << 22; 
    }
};

