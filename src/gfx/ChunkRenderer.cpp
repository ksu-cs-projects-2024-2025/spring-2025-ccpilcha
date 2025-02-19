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

void ChunkRenderer::RenderChunkAt(ChunkPos pos)
{
	if (!this->world->AreAllNeighborsLoaded(pos)) {
		chunkRenderQueue.push(pos);
        return;
    }
	
	Chunk *chunk = this->world->chunks[pos].get();

	if (!this->world->ChunkLoaded(pos))
	{
		chunkRenderQueue.push(pos);
		return;
	}
	if (!chunk->dirty) return;

	std::vector<ChunkVertex> newVerts;

	// then update
	for (int z = 0; z < chunk->blocks.size(); z++) {
	for (int y = 0; y < CHUNK_Y_SIZE; y++) {
	for (int x = 0; x < CHUNK_X_SIZE; x++) {
		BLOCK_ID_TYPE blockId = chunk->blocks[z][y][x];
		if (blockId <= 0) continue;
		for (int face = 0; face < 6; face++) {
			int nx = x + nOffsets[face][0];
			int ny = y + nOffsets[face][1];
			int nz = z + nOffsets[face][2];

			uint16_t neighborBlockId = this->world->GetBlockId(pos, nx, ny, nz);
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

	if (this->chunkMeshes.contains(pos)) {
		this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
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
        }

		if (!chunkRenderQueue.try_pop(pos)) continue;

		if (this->world->ChunkReady(pos))
		{
			threadPool.enqueueTask([this, pos]() {
				if (!chunkMeshes.contains(pos)) {
					chunkMeshes[pos] = std::make_shared<ChunkMesh>();
				}
	
				this->RenderChunkAt(pos);
			});
		}
    }
}

ChunkRenderer::ChunkRenderer(World *w) : world(w), chunkShader("assets/shaders/chunk.v.glsl", "assets/shaders/chunk.f.glsl"), chunkMeshes(), threadPool(8)
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
	c->texture.use(GL_TEXTURE0);
	
	chunkShader.use(); // we are using the same shader each time
	chunkShader.setInt("textureAtlas", 0);
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

	for (auto &meshPair : chunkMeshes) {
		if (c->plr->chunkPos.distanceXY(meshPair.first) > c->renderDistance) {
			continue;
		}
		glm::vec3 chunkPos = glm::vec3(
			meshPair.first.x * CHUNK_X_SIZE, 
			meshPair.first.y * CHUNK_Y_SIZE, 
			meshPair.first.z * CHUNK_Z_SIZE
		);
		glm::vec3 minCorner = chunkPos + camPos;
		glm::vec3 maxCorner = minCorner + glm::vec3(CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE);

		if (isChunkVisible(frustumPlanes, minCorner, maxCorner)) {
			chunkShader.setVec3("chunkPos", chunkPos);
			meshPair.second->Render();
		} else {
			continue;
		}
	}
}
