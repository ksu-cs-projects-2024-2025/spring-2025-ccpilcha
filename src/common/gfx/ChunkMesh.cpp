#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "game/World.hpp"
#include "util/GLHelper.hpp"
#include "ChunkMesh.hpp"

std::vector<VertexAttribute> ChunkVertexAttribs = {
	{1, GL_UNSIGNED_INT, GL_FALSE, sizeof(ChunkVertex), 0, 1},
	{2, GL_UNSIGNED_INT, GL_FALSE, sizeof(ChunkVertex), (GLvoid*)sizeof(uint32_t), 1}
};


ChunkMesh::ChunkMesh() : 
	meshMutex(),
	meshSwapping(true),
	currentBuffer(&this->bufferA),
	currentTBuffer(&this->TbufferA)
{
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::Init(GLuint vao, GLuint vbo, GLuint vaoT, GLuint vboT)
{
	this->init = true;
		// e.g. when you create a new chunk or in your World/Renderer init
	glGenQueries(1, &occlusionQuery);
	// now chunk->occlusionQuery != 0

	this->vao = vao;
	this->vbo = vbo;
	this->vaoT = vaoT;
	this->vboT = vboT;
	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
	glCall(glBindVertexArray(this->vao));
	int num = 0;
	for (auto& vAttrib : ChunkVertexAttribs)
	{
		vAttrib.Enable(num++);
	}
	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vboT));
	glCall(glBindVertexArray(this->vaoT));
	num = 0;
	for (auto& vAttrib : ChunkVertexAttribs)
	{
		vAttrib.Enable(num++);
	}
}

void ChunkMesh::Load(std::vector<ChunkVertex> opaque, std::vector<ChunkVertex> translucent) {
    std::lock_guard<std::mutex> lock(meshMutex);

    if (bufferAFlag) {
		bufferA = std::move(opaque);
		TbufferA = std::move(translucent);

		bufferA.shrink_to_fit();
		TbufferA.shrink_to_fit();
	} else {
		bufferB = std::move(opaque);
		TbufferB = std::move(translucent);

		bufferB.shrink_to_fit();
		TbufferB.shrink_to_fit();
	}

	loaded.store(true);
	dirty.store(false);
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
    currentBuffer  = bufferAFlag ? &bufferA : &bufferB;
    currentTBuffer = bufferAFlag ? &TbufferA : &TbufferB;  // Added swap for translucent buffer
    bufferAFlag = !bufferAFlag;
}

void ChunkMesh::Update(GameContext *c)
{
	if (!this->init) return;
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

	if (currentBuffer->size() > 0) {
		GLsizei wanted = GLsizei(currentBuffer->size() * sizeof(ChunkVertex));
		GLsizei padded = wanted + sizeof(uint32_t);
		glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
		glCall(glBindVertexArray(this->vao));
		glCall(glBufferData(GL_ARRAY_BUFFER, padded, nullptr, GL_STATIC_DRAW));

		glBufferSubData(GL_ARRAY_BUFFER, 0, wanted, currentBuffer->data());
	}

	if (currentTBuffer->size() > 0) {
		GLsizei wanted = GLsizei(currentTBuffer->size() * sizeof(ChunkVertex));
		GLsizei padded = wanted + sizeof(uint32_t);
		glCall(glBindBuffer(GL_ARRAY_BUFFER, this->vboT));
		glCall(glBindVertexArray(this->vaoT));
		glCall(glBufferData(GL_ARRAY_BUFFER, padded, nullptr, GL_STATIC_DRAW));

		glBufferSubData(GL_ARRAY_BUFFER, 0, wanted, currentTBuffer->data());
	}
	meshSwapping.store(false);
	isUploaded = true;
}

/**
 * @brief Renders the ChunkMesh. Must be loaded prior
 * @warning Must be ran from the main thread!
 */
void ChunkMesh::RenderOpaque()
{
	std::lock_guard<std::mutex> lock(this->meshMutex);
	
	if (!isUploaded.load()) return;
	if (this->currentBuffer->empty()) return;

	this->rendering = true;

	glCall(glBindVertexArray(this->vao));
	glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)this->currentBuffer->size()));

	this->rendering = false;
}

void ChunkMesh::RenderTransparent()
{
	std::lock_guard<std::mutex> lock(this->meshMutex);
	
	if (!isUploaded.load()) return;
	if (this->currentTBuffer->empty()) return;

	this->rendering = true;

	glCall(glBindVertexArray(this->vaoT));
	glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)this->currentTBuffer->size()));

	this->rendering = false;
}

void ChunkMesh::Clear()
{
	std::lock_guard<std::mutex> lock(this->meshMutex);
	isUploaded = false;
    this->init = false;
	this->used = false;
	this->loaded.store(false);
}