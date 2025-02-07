#include "ChunkRenderer.hpp"

std::vector<VertexAttribute> ChunkVertexAttribs = {
	{3, GL_INT, GL_FALSE, sizeof(ChunkVertex), 0},
	{1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(int))},
	{1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(4 * sizeof(int))}
};

ChunkRenderer::ChunkRenderer() : chunkShader("assets/shaders/chunk.v.glsl", "assets/shaders/chunk.f.glsl")
{

}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::Init(GameContext *c)
{
	glCall(glGenVertexArrays(1, &this->vao));
    glCall(glBindVertexArray(this->vao));
	int num = 0;
	for (auto& vAttrib : ChunkVertexAttribs)
	{
		vAttrib.enable(num++);
	}
    glCall(glBindVertexArray(0)); // just for cleanliness we can clean the pipeline
}

void ChunkRenderer::Update(GameContext *c, double deltaTime)
{
    // Here is where chunk meshes need to be generated. 
    // This should be done on worker threads which return a completed mesh for the renderer to use
    
    // when chunks are modified, their pointer will be placed in a queue in world called "Dirty" to signal that the chunk has been updated
    // new chunk needs to have the same vao used
}

void ChunkRenderer::Render(GameContext *c)
{
    chunkShader.use(); // we are using the same shader each time
}
