#include "ChunkMesh.hpp"

#include "World.hpp"
#include <iostream>
#include "GLHelper.hpp"
#include <glad/gl.h>

// Neighbor offsets as (x, y, z) tuples
constexpr int8_t nOffsets[6][3] = {
	{-1, 0, 0}, // -x
	{1, 0, 0},	// +x
	{0, -1, 0},	// -y
	{0, 1, 0},	// +y
	{0, 0, -1},  // -z
	{0, 0, 1}  // +z
};

ChunkMesh::ChunkMesh() : loaded(false)
{
}

ChunkMesh::~ChunkMesh()
{
}

/**
 * @brief Loads the ChunkMesh in the pipeline.
 * 
 * The hope is for this work to be dispatched to other threads.
 * We also need to let the graphics card know 
 * 
 * 
 */
void ChunkMesh::Update(GameContext *c)
{
	if (this->chunk->dirty || c->plr->chunkPos != c->plr->lastPos) {
		std::vector<ChunkVertex> newVerts;
	
		// then update
		for (int z = 0; z < chunk->blocks.size(); z++) {
			for (int y = 0; y < CHUNK_Y_SIZE; y++) {
				for (int x = 0; x < CHUNK_X_SIZE; x++) {
					BLOCK_ID_TYPE blockId = chunk->blocks.at(z)[x][y];
					if (blockId > 0) {
						for (int face = 0; face < 6; face++) {
							int nx = x + nOffsets[face][0];
							int ny = y + nOffsets[face][1];
							int nz = z + nOffsets[face][2];

							uint16_t neighborBlockId = c->world->GetBlockId(this->chunk->pos, nx, ny, nz);
							if (neighborBlockId <= 0) {
								// Add the face to the mesh
								std::vector<ChunkVertex> blockVerts = {
									{ x, y, z, blockId, face},
									{ x, y, z, blockId, face},
									{ x, y, z, blockId, face},
									{ x, y, z, blockId, face},
									{ x, y, z, blockId, face},
									{ x, y, z, blockId, face},
								};
								newVerts.insert(newVerts.end(), blockVerts.begin(), blockVerts.end());
							}
						}
					}
				}
			}
		}

		
	}
}

/**
 * @brief Loads vertex data to the GPU. We then flag this mesh as ready for rendering once complete
 * 
 * @warning Must be ran from the main window/graphics thread!
 */
void ChunkMesh::UploadToGPU()
{
}

/**
 * @brief Renders the ChunkMesh. Must be loaded prior
 * @warning Must be ran from the main thread!
 */
void ChunkMesh::Render()
{
	if (!this->loaded) return; // TODO: this will need to be thread safe (mutex)
	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));

	glCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
}