#include "ChunkMesh.hpp"

#include <iostream>
#include "GLHelper.hpp"
#include <glad/gl.h>

ChunkMesh::ChunkMesh() : loaded(false)
{
}

ChunkMesh::~ChunkMesh()
{
}

/**
 * @brief Loads the ChunkMesh in the pipeline.
 * @warning Must be ran from the main thread!
 */
void ChunkMesh::Update()
{
	if (this->chunk->dirty) {
		// then update
	}
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