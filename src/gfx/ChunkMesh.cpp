#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "ChunkMesh.hpp"
#include "World.hpp"
#include "GLHelper.hpp"

std::vector<VertexAttribute> ChunkVertexAttribs = {
	{1, GL_UNSIGNED_INT, GL_FALSE, sizeof(ChunkVertex), 0, 1}
};


ChunkMesh::ChunkMesh() : 
	meshMutex(std::mutex()),
	meshSwapping(true),
	currentBuffer(&this->bufferA)
{
}

ChunkMesh::~ChunkMesh()
{
    std::lock_guard<std::mutex> lock(meshMutex);

    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void ChunkMesh::Load(std::vector<ChunkVertex> data) {
    std::lock_guard<std::mutex> lock(meshMutex);

    if (bufferAFlag) 
		bufferA = std::move(data);
	else 
		bufferB = std::move(data);

	meshSwapping.store(true);
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
    std::lock_guard<std::mutex> lock(meshMutex);
    if (!meshSwapping.load()) return;
	currentBuffer = bufferAFlag ? &bufferA : &bufferB;
	bufferAFlag = !bufferAFlag;
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
			vAttrib.enable(num++);
	}
    Swap();
    UploadToGPU();
}

/**
 * @brief Loads vertex data to the GPU. We then flag this mesh as ready for rendering once complete
 * 
 * @warning Must be ran from the main window/graphics thread!
 */
void ChunkMesh::UploadToGPU() {
    if (!meshSwapping.load() || !init) return;

    std::lock_guard<std::mutex> lock(meshMutex);

	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
	glCall(glBindVertexArray(this->vao));
    glBufferData(GL_ARRAY_BUFFER, currentBuffer->size() * sizeof(ChunkVertex),  currentBuffer->data(), GL_DYNAMIC_DRAW);
	meshSwapping.store(false);
	isValid = true;
}

/**
 * @brief Renders the ChunkMesh. Must be loaded prior
 * @warning Must be ran from the main thread!
 */
void ChunkMesh::Render()
{
	std::lock_guard<std::mutex> lock(this->meshMutex);
	
	if (!isValid.load()) return;
	if (this->currentBuffer->empty()) return;

	this->rendering = true;

	glCall(glBindVertexArray(this->vao));
	glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, this->currentBuffer->size()));

	this->rendering = false;
}

void ChunkMesh::Clear()
{
	isValid = false;
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    this->init = false;
}