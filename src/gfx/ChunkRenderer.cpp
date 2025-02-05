#include "ChunkRenderer.hpp"

ChunkRenderer::ChunkRenderer() : chunkShader("assets/shaders/chunk.v.glsl", "assets/shaders/chunk.f.glsl")
{

}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::Init(GameContext *c)
{
	glCall(glGenVertexArrays(1, &this->vao));
}

void ChunkRenderer::OnEvent(GameContext *c, SDL_Event *event)
{
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
