#include <glad/gl.h>
#include <iostream>

#include "ChunkMesh.hpp"
#include "World.hpp"
#include "GLHelper.hpp"

std::vector<VertexAttribute> ChunkVertexAttribs = {
	{3, GL_INT, GL_FALSE, sizeof(ChunkVertex), 0},
	{1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(int))},
	{1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(4 * sizeof(int))}
};

ChunkMesh::ChunkMesh() : 
	meshMutex(std::make_unique<std::mutex>()),
	meshSwapping(std::make_unique<std::atomic<bool>>(false)),
	currentBuffer(&this->bufferA)
{
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::Load(std::vector<ChunkVertex> data) {
    std::lock_guard<std::mutex> lock(*meshMutex);
	(*meshSwapping).store(true);
    if (bufferAFlag) {
        bufferA = std::move(data);
        currentBuffer = &bufferA;
    } else {
        bufferB = std::move(data);
        currentBuffer = &bufferB;
    }
}

/**
 * @brief Swaps the vertex buffer in the pipeline.
 * 
 * The hope is for this work to be dispatched to other threads.
 * We also need to let the graphics card know 
 * 
 * 
 */
void ChunkMesh::Swap() {
    std::lock_guard<std::mutex> lock(*meshMutex);
    if ((*meshSwapping).load()) {
        currentBuffer = bufferAFlag ? &bufferA : &bufferB;
		bufferAFlag = !bufferAFlag;
    }
}

void ChunkMesh::Update(GameContext *c)
{
	if (!this->init)
	{
		this->init = true;
		glCall(glGenBuffers(1, &this->vbo));
		glCall(glGenVertexArrays(1, &this->vao));
		glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
		glCall(glBindVertexArray(this->vao));
		int num = 0;
		for (auto& vAttrib : ChunkVertexAttribs)
		{
			vAttrib.enable(num++);
		}
	}
}

/**
 * @brief Loads vertex data to the GPU. We then flag this mesh as ready for rendering once complete
 * 
 * @warning Must be ran from the main window/graphics thread!
 */
void ChunkMesh::UploadToGPU() {
    if (!(*meshSwapping).load()) return;

    std::lock_guard<std::mutex> lock(*meshMutex);

    if (!init) {
        glGenBuffers(1, &vbo);
		glCall(glGenVertexArrays(1, &this->vao));
        init = true;
    }

	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
	glCall(glBindVertexArray(this->vao));
    glBufferData(GL_ARRAY_BUFFER, currentBuffer->size() * sizeof(ChunkVertex),  currentBuffer->data(), GL_DYNAMIC_DRAW);
	(*meshSwapping).store(false);
}

/**
 * @brief Renders the ChunkMesh. Must be loaded prior
 * @warning Must be ran from the main thread!
 */
void ChunkMesh::Render()
{
	if ((*meshSwapping).load() || this->currentBuffer->empty()) return;

	std::lock_guard<std::mutex> lock(*this->meshMutex);

	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
	glCall(glBindVertexArray(this->vao));
	glCall(glDrawArrays(GL_TRIANGLES, 0, this->currentBuffer->size()));
}