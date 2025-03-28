
#include <glm/glm.hpp>

#include "ChunkRenderer.hpp"
#include "ChunkMesh.hpp"

#include "Plane.hpp"
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

void ChunkRenderer::RenderChunkAt(PrioritizedChunk pChunk)
{
	ChunkPos pos = pChunk.pos;
	if (!this->world->AreAllNeighborsLoaded(pos)) {
		chunkRenderQueue.push(pChunk);
        return;
    }
	
	std::shared_ptr<Chunk> chunk = this->world->chunks.at(pos);

	if (!this->world->ChunkLoaded(pos))
	{
		chunkRenderQueue.push(pChunk);
		return;
	}
	if (!chunk->dirty) return;

	chunk->rendering.store(true);

	std::vector<ChunkVertex> newVerts;

	// then update
	for (int z = 0; z < chunk->size(); z++) {
	for (int y = 0; y < CHUNK_Y_SIZE; y++) {
	for (int x = 0; x < CHUNK_X_SIZE; x++) {
		BLOCK_ID_TYPE blockId = chunk->GetBlockId(x,y,z);
		if (blockId <= 0) continue;
		for (int face = 0; face < 6; face++) {
			int nx = x + nOffsets[face][0];
			int ny = y + nOffsets[face][1];
			int nz = z + nOffsets[face][2];

			if (this->world->GetBlockId(pos, nx, ny, nz) > 0) continue;

			// Add the face to the mesh
			newVerts.push_back({ x, y, z, blockId, face });
		}
	}
	}
	}

	if (!this->chunkMeshes.contains(pos)) {
		this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
	} else if (this->chunkMeshes.at(pos)->pos != pChunk.pos) return;

	this->chunkMeshes.at(pos)->Load(newVerts);
	this->chunkMeshes.at(pos)->pos = pos;
	chunk->rendering.store(false);
}

void ChunkRenderer::RenderChunks(GameContext *c)
{

    // TODO: this thread needs to end when the game exits the playing state
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueRenderMutex);
            queueCV.wait(lock, [this] { return !chunkRenderQueue.empty(); });
        }

        PrioritizedChunk pChunk;
		
		if (!chunkRenderQueue.try_pop(pChunk)) continue;
		
		ChunkPos pos = pChunk.pos;

		if (this->world->ChunkReady(pos))
		{
			if (!chunkMeshes.contains(pos))
			{
				std::shared_ptr<ChunkMesh> mesh;
				/*
				if (!freeMeshes.empty() && freeMeshes.try_pop(mesh))
				{
					if (mesh->IsReusable()) 
					{
						freeMeshes.push(mesh);
						this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
					}
					else
						this->chunkMeshes.emplace(pos, mesh);
				}
				else */
					this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
			}
			this->chunkMeshes.at(pos)->used = true;
			this->chunkMeshes.at(pos)->pos = pos;
			threadPool.enqueueTask([this, c, pChunk]() {
				// it is still possible for the taskid to increment afterwards, but this shouldn't be of much issue
				this->RenderChunkAt(pChunk);
			});
		}
    }
}

ChunkRenderer::ChunkRenderer(World *w) : 
	world(w), 
	chunkShader("assets/shaders/game/chunk.v.glsl", "assets/shaders/game/chunk.f.glsl"), 
	chunkMeshes(), 
	threadPool(16)
{

}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::Init(GameContext *c)
{

    std::thread thr(&ChunkRenderer::RenderChunks, this, c);
    thr.detach();

}

void ChunkRenderer::Update(GameContext *c, double deltaTime)
{
    // Here is where chunk meshes need to be generated. 
    // This should be done on worker threads which return a completed mesh for the renderer to use
    
    // when chunks are modified, their pointer will be placed in a queue in world called "Dirty" to signal that the chunk has been updated
    // new chunk needs to have the same vao used


	std::vector<std::shared_ptr<ChunkMesh>> meshes;
	int init = 0;
	chunkShader.use();
	for (auto &meshPair : chunkMeshes) {
		if (meshPair.second->isInit())
			meshPair.second->Update(c);
		else
			meshes.push_back(meshPair.second);
	}

	GLuint* vao = new GLuint[meshes.size()];
	GLuint* vbo = new GLuint[meshes.size()];
	glCall(glGenBuffers(meshes.size(), vbo));
	glCall(glGenVertexArrays(meshes.size(), vao));
	int i = 0;
	for(auto mesh : meshes) {
		mesh->Init(vao[i], vbo[i]);
		i++;
	};


    for (int i = 0; i < std::min((size_t)10, chunkRemoveQueue.unsafe_size()); i++)
    {
        ChunkPos pos;
        if (!chunkRemoveQueue.try_pop(pos)) return;
		if (!chunkMeshes.contains(pos)) continue;
        auto &cptr = chunkMeshes.at(pos);
        // If it hasn’t finished loading, skip removal (it’ll be retried later)
        if (!cptr) continue;
        // Mark for removal and remove the chunk
        //chunkMeshes.at(pos)->Clear();
		//freeMeshes.push(chunkMeshes.at(pos));
    }
}

// Check if an AABB is inside the frustum
bool isChunkVisible(const std::array<Plane, 6>& frustum, const glm::vec3& minCorner, const glm::vec3& maxCorner) {
    for (const auto& plane : frustum) {
        int inCount = 0;
        glm::vec3 corners[8] = {
            {minCorner.x, minCorner.y, minCorner.z},
            {maxCorner.x, minCorner.y, minCorner.z},
            {minCorner.x, maxCorner.y, minCorner.z},
            {maxCorner.x, maxCorner.y, minCorner.z},
            {minCorner.x, minCorner.y, maxCorner.z},
            {maxCorner.x, minCorner.y, maxCorner.z},
            {minCorner.x, maxCorner.y, maxCorner.z},
            {maxCorner.x, maxCorner.y, maxCorner.z},
        };

        // If all 8 points of the AABB are outside a single frustum plane, cull it
        for (const auto& corner : corners) {
            if (glm::dot(plane.normal, corner) + plane.d > 0) {
                inCount++;
            }
        }

        if (inCount == 0) {
            return false;  // Fully outside the frustum
        }
    }
    return true;  // At least partially inside the frustum
}


void ChunkRenderer::Render(GameContext *c)
{
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	c->texture.use(GL_TEXTURE1);
	
	chunkShader.use(); // we are using the same shader each time
	chunkShader.setInt("textureAtlas", 1);
	chunkShader.setMat4("projection", c->plr->camera.proj);
	chunkShader.setMat4("view", c->plr->camera.view);
	chunkShader.setFloat("opacity", 1.0f);
	chunkShader.setVec3("hint", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 camPos = glm::vec3(
		-1 * c->plr->chunkPos.x * CHUNK_X_SIZE, 
		-1 * c->plr->chunkPos.y * CHUNK_Y_SIZE, 
		-1 * c->plr->chunkPos.z * CHUNK_Z_SIZE
	);
	glm::mat4 VP = c->plr->camera.proj * c->plr->camera.view;
	std::array<Plane, 6> frustumPlanes = extractFrustumPlanes(VP);
	chunkShader.setMat4("model", glm::translate(glm::mat4(1.0f), camPos));

	for (int dz = -c->renderDistance; dz < c->renderDistance; dz++){
	for (int dy = -c->renderDistance; dy < c->renderDistance; dy++){
	for (int dx = -c->renderDistance; dx < c->renderDistance; dx++){
		ChunkPos pos = c->plr->chunkPos + ChunkPos{dx,dy,dz};
		if (!chunkMeshes.contains(pos)) continue;
		std::shared_ptr<ChunkMesh> mesh = chunkMeshes.at(pos);

		chunkShader.setInt("LOD", 2);
		
		glm::vec3 chunkPos = glm::vec3(
			pos.x * CHUNK_X_SIZE, 
			pos.y * CHUNK_Y_SIZE, 
			pos.z * CHUNK_Z_SIZE
		);
		glm::vec3 minCorner = chunkPos + camPos;
		glm::vec3 maxCorner = minCorner + glm::vec3(CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE);

		if (isChunkVisible(frustumPlanes, minCorner, maxCorner)) {
			chunkShader.setVec3("chunkPos", chunkPos);
			mesh->Render();
		} else {
			continue;
		}
	}
	}
	}
}
