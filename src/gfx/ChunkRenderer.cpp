#include "ChunkRenderer.hpp"
#include "ChunkMesh.hpp"

#include "World.hpp"

// Neighbor offsets as (x, y, z) tuples
constexpr int8_t nOffsets[6][3] = {
	{-1, 0, 0}, // -x
	{1, 0, 0},	// +x
	{0, -1, 0},	// -y
	{0, 1, 0},	// +y
	{0, 0, -1},  // -z
	{0, 0, 1}  // +z
};

void ChunkRenderer::RenderChunkAt(ChunkPos pos)
{
	if (!this->chunkMeshes.contains(pos)) {
		this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
	}

	Chunk *chunk = this->world->chunks[pos].get();

	if (!chunk->dirty) return;

	std::vector<ChunkVertex> newVerts;

	// then update
	for (int z = 0; z < chunk->blocks.size(); z++) {
		if (z == CHUNK_Z_SIZE - 1)
		{
			1;
		}
	for (int y = 0; y < CHUNK_Y_SIZE; y++) {
	for (int x = 0; x < CHUNK_X_SIZE; x++) {
		BLOCK_ID_TYPE blockId = chunk->blocks[z][y][x];
		if (blockId <= 0) continue;
		for (int face = 0; face < 6; face++) {
			int nx = x + nOffsets[face][0];
			int ny = y + nOffsets[face][1];
			int nz = z + nOffsets[face][2];

			uint16_t neighborBlockId = this->world->GetBlockId(chunk->pos, nx, ny, nz);
			if (neighborBlockId <= 0) {
				// Add the face to the mesh
				std::vector<ChunkVertex> blockVerts = {
					{ x, y, z, blockId, face },
					{ x, y, z, blockId, face },
					{ x, y, z, blockId, face },
					{ x, y, z, blockId, face },
					{ x, y, z, blockId, face },
					{ x, y, z, blockId, face },
				};
				newVerts.insert(newVerts.end(), blockVerts.begin(), blockVerts.end());
			}
		}
	}
	}
	}

	this->chunkMeshes[pos]->Load(newVerts);
}

void ChunkRenderer::RenderChunks()
{

    // TODO: this thread needs to end when the game exits the playing state
    while (true)
    {
        ChunkPos pos;

        {
            std::unique_lock<std::mutex> lock(queueRenderMutex);
            queueCV.wait(lock, [this] { return !chunkRenderQueue.empty(); });

            pos = chunkRenderQueue.front();
            chunkRenderQueue.pop();
        }
		
        this->RenderChunkAt(pos);

    }
}

ChunkRenderer::ChunkRenderer(World *w) : world(w), chunkShader("assets/shaders/chunk.v.glsl", "assets/shaders/chunk.f.glsl"), chunkMeshes()
{

}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::Init(GameContext *c)
{

    std::thread thr(&ChunkRenderer::RenderChunks, this);
    thr.detach();
}

void ChunkRenderer::Update(GameContext *c, double deltaTime)
{
    // Here is where chunk meshes need to be generated. 
    // This should be done on worker threads which return a completed mesh for the renderer to use
    
    // when chunks are modified, their pointer will be placed in a queue in world called "Dirty" to signal that the chunk has been updated
    // new chunk needs to have the same vao used

	for (auto &meshPair : chunkMeshes) {
		chunkShader.use();
		meshPair.second->Update(c);
		meshPair.second->Swap();
		meshPair.second->UploadToGPU();
	}
}

void ChunkRenderer::Render(GameContext *c)
{
	c->texture.use(GL_TEXTURE0);
	
	chunkShader.use(); // we are using the same shader each time
	chunkShader.setInt("textureAtlas", 0);
	chunkShader.setMat4("projection", c->plr->camera.proj);
	chunkShader.setMat4("view", c->plr->camera.view);
	chunkShader.setFloat("opacity", 1.0f);
	chunkShader.setVec3("hint", glm::vec3(1.0f, 1.0f, 1.0f));
	
	for (auto &meshPair : chunkMeshes) {
		chunkShader.setMat4("model", glm::translate(glm::mat4(1.0f), 
			glm::vec3((meshPair.first.x - c->plr->chunkPos.x) * CHUNK_X_SIZE * 1.0f, 
				(meshPair.first.y - c->plr->chunkPos.y) * CHUNK_Y_SIZE * 1.0f, 
				(meshPair.first.z - c->plr->chunkPos.z) * CHUNK_Z_SIZE * 1.0f)));
		meshPair.second->Render();
	}
}
