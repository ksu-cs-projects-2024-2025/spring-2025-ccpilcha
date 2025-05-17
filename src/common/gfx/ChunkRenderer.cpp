
#include <glm/glm.hpp>
#include <imgui.h>

#include "game/World.hpp"
#include "ChunkRenderer.hpp"
#include "Plane.hpp"
#include "ChunkMesh.hpp"

// Neighbor offsets as (x, y, z) tuples
constexpr int8_t nOffsets[6][3] = {
	{-1, 0, 0}, // -x
	{1, 0, 0},	// +x
	{0, -1, 0},	// -y
	{0, 1, 0},	// +y
	{0, 0, -1},  // -z
	{0, 0, 1}  // +z
};

// These are the blocks which must be checked for ambient occlusion lighting
const int aoOffsets[6][4][3][3] = {
    // -X face (left)
	/*
	{ // -X
		{{0,  1,  0}, {0,  0,  1}, {0,  1,  1}},  // v0
		{{0, -1,  0}, {0,  0,  1}, {0, -1,  1}},  // v1
		{{0,  1,  0}, {0,  0, -1}, {0,  1, -1}},  // v2
		{{0, -1,  0}, {0,  0, -1}, {0, -1, -1}},  // v3
	},	*/
	{ // -X
		{{-1,  1,  0}, {-1,  0, -1}, {-1,  1, -1}},
		{{-1, -1,  0}, {-1,  0, -1}, {-1, -1, -1}},
		{{-1,  1,  0}, {-1,  0,  1}, {-1,  1,  1}},
		{{-1, -1,  0}, {-1,  0,  1}, {-1, -1,  1}} 
	},
	{ // +X
		{{ 1, -1,  0}, { 1,  0, -1}, { 1, -1, -1}},
		{{ 1,  1,  0}, { 1,  0, -1}, { 1,  1, -1}},
		{{ 1, -1,  0}, { 1,  0,  1}, { 1, -1,  1}},
		{{ 1,  1,  0}, { 1,  0,  1}, { 1,  1,  1}} 
	},
	{ // -Y
		{{-1, -1,  0}, { 0, -1, -1}, {-1, -1, -1}},
		{{ 1, -1,  0}, { 0, -1, -1}, { 1, -1, -1}},
		{{-1, -1,  0}, { 0, -1,  1}, {-1, -1,  1}},
		{{ 1, -1,  0}, { 0, -1,  1}, { 1, -1,  1}} 
	},	
	{ // +Y
		{{ 1,  1,  0}, { 0,  1, -1}, { 1,  1, -1}},
		{{-1,  1,  0}, { 0,  1, -1}, {-1,  1, -1}},
		{{ 1,  1,  0}, { 0,  1,  1}, { 1,  1,  1}},
		{{-1,  1,  0}, { 0,  1,  1}, {-1,  1,  1}}
	},
	{ // -Z
		{{ 0,  1, -1}, { 1,  0, -1}, { 1,  1, -1}},
		{{ 0, -1, -1}, { 1,  0, -1}, { 1, -1, -1}},
		{{ 0,  1, -1}, {-1,  0, -1}, {-1,  1, -1}},
		{{ 0, -1, -1}, {-1,  0, -1}, {-1, -1, -1}} 
	},	
	{ // +Z
		{{-1,  0,  1}, { 0, -1,  1}, {-1, -1,  1}},
		{{ 1,  0,  1}, { 0, -1,  1}, { 1, -1,  1}},
		{{-1,  0,  1}, { 0,  1,  1}, {-1,  1,  1}},
		{{ 1,  0,  1}, { 0,  1,  1}, { 1,  1,  1}}
	}
};

// A unit‑cube (AABB) as 12 triangles (36 vertices), xyz triplets.
static constexpr float aabbVertices[36 * 3] = {
    // Front  (z = 0)
     0.f, 0.f, 0.f,
     1.f, 0.f, 0.f,
     1.f, 1.f, 0.f,
     1.f, 1.f, 0.f,
     0.f, 1.f, 0.f,
     0.f, 0.f, 0.f,

    // Back   (z = 1)
     1.f, 0.f, 1.f,
     0.f, 0.f, 1.f,
     0.f, 1.f, 1.f,
     0.f, 1.f, 1.f,
     1.f, 1.f, 1.f,
     1.f, 0.f, 1.f,

    // Left   (x = 0)
     0.f, 0.f, 1.f,
     0.f, 0.f, 0.f,
     0.f, 1.f, 0.f,
     0.f, 1.f, 0.f,
     0.f, 1.f, 1.f,
     0.f, 0.f, 1.f,

    // Right  (x = 1)
     1.f, 0.f, 0.f,
     1.f, 0.f, 1.f,
     1.f, 1.f, 1.f,
     1.f, 1.f, 1.f,
     1.f, 1.f, 0.f,
     1.f, 0.f, 0.f,

    // Bottom (y = 0)
     0.f, 0.f, 1.f,
     1.f, 0.f, 1.f,
     1.f, 0.f, 0.f,
     1.f, 0.f, 0.f,
     0.f, 0.f, 0.f,
     0.f, 0.f, 1.f,

    // Top    (y = 1)
     0.f, 1.f, 0.f,
     1.f, 1.f, 0.f,
     1.f, 1.f, 1.f,
     1.f, 1.f, 1.f,
     0.f, 1.f, 1.f,
     0.f, 1.f, 0.f,
};

std::pair<std::vector<ChunkVertex>, std::vector<ChunkVertex>> ChunkRenderer::GenerateGreedyMesh(const Chunk &chunk, GameContext *c)
{
    std::vector<ChunkVertex> opaque;
    std::vector<ChunkVertex> translucent;

    // GREEDY MESHING IMPLEMENTATION:
    // - For each axis (e.g., x, y, z):
    //   - For each slice of the chunk in that axis:
    //     - Scan the slice to merge contiguous faces that share the same block type, texture, and AO values.
    //     - For each merged quad, push one ChunkVertex (or quad converted to vertices)
    //       into either 'opaque' or 'translucent' depending on block transparency.
    // TODO: Implement the detailed greedy meshing algorithm.

    return {opaque, translucent};
}

void ChunkRenderer::RenderChunkAt(GameContext *c, PrioritizedChunk pChunk)
{
	int currentGen = chunkGenFrameId.load();
	ChunkPos pos = pChunk.pos;
	if (!this->world->AreAllNeighborsLoaded(pos)) {
		chunkRenderQueue.push(pChunk);
        return;
    }
	
	if (!this->world->ChunkLoaded(pos))
	{
		chunkRenderQueue.push(pChunk);
		return;
	}

	std::shared_ptr<Chunk> chunk = this->world->chunks.at(pos);
    std::shared_lock lock(chunk->chunkMutex);
	
    // Acquire shared (read) lock
	chunk->rendering.store(true);

	std::vector<ChunkVertex> newVerts;
	std::vector<ChunkVertex> newVertsTranslucent;

	// then update
	for (int z = 0; z < chunk->size(); z++) {
	for (int y = 0; y < CHUNK_Y_SIZE; y++) {
	for (int x = 0; x < CHUNK_X_SIZE; x++) {
		if (this->chunkGenFrameId.load() != currentGen) {
			return;
		}
		if (c->isClosing) return;
		BLOCK_ID_TYPE blockId = chunk->GetBlockId(x,y,z);
		BlockInfo blockInfo = c->blockRegistry[blockId];
		if (blockId <= 0) continue;
		for (int face = 0; face < 6; face++) {
			int nx = x + nOffsets[face][0];
			int ny = y + nOffsets[face][1];
			int nz = z + nOffsets[face][2];

			BlockInfo n = c->blockRegistry.at(this->world->GetBlockId(pos, nx, ny, nz));
			// now we have to calculate AO
			int8_t aoByte = 0;
			bool bothWater = blockInfo.blockType == BlockType::Water && n.blockType == BlockType::Water;
			bool waterNextToAir = blockInfo.blockType == BlockType::Water && n.blockType == BlockType::Air;
			
			if (bothWater) continue;
			if (!waterNextToAir && !n.IsTranslucent()) continue;

			for (int corner = 0; corner < 4; ++corner) {
				bool side1 = !c->blockRegistry.at(
									this->world->GetBlockId(pos,
									x + aoOffsets[face][corner][0][0],
									y + aoOffsets[face][corner][0][1],
									z + aoOffsets[face][corner][0][2])).IsTranslucent();

				bool side2 = !c->blockRegistry.at(
									this->world->GetBlockId(pos,
									x + aoOffsets[face][corner][1][0],
									y + aoOffsets[face][corner][1][1],
									z + aoOffsets[face][corner][1][2])).IsTranslucent();

				bool cornerBlock = !c->blockRegistry.at(
									this->world->GetBlockId(pos,
										x + aoOffsets[face][corner][2][0],
										y + aoOffsets[face][corner][2][1],
										z + aoOffsets[face][corner][2][2])).IsTranslucent();

				int8_t ao = (side1 && side2) ? 0 : 3 - (side1 + side2 + cornerBlock);
				aoByte |= (ao << (corner * 2));
			}

			// Add the face to the mesh
			if (blockInfo.IsTranslucent())
				newVertsTranslucent.emplace_back(x, y, z, blockInfo.textureIndices[face], face, aoByte);
			else
				newVerts.emplace_back(x, y, z, blockInfo.textureIndices[face], face, aoByte);
		}
	}
	}
	}

	if (!this->chunkMeshes.contains(pos)) {
		this->chunkMeshes.emplace(pos, std::make_shared<ChunkMesh>());
	} else if (this->chunkMeshes.at(pos)->pos != pChunk.pos) return;

	this->chunkMeshes.at(pos)->Load(newVerts, newVertsTranslucent);
	this->chunkMeshes.at(pos)->pos = pos;
	chunk->rendering.store(false);

	
}

void ChunkRenderer::RenderChunks(GameContext *c)
{

	int currentGen = chunkGenFrameId.load();
    // TODO: this thread needs to end when the game exits the playing state
    while (!c->isClosing)
    {
        {
            std::unique_lock<std::mutex> lock(queueRenderMutex);
            queueCV.wait(lock, [this, c] { return c->isClosing || !chunkRenderQueue.empty(); });

			if (c->isClosing) return;
			currentGen = chunkGenFrameId.load();
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
			auto task = [this, c, pChunk, currentGen]() {
                if (this->chunkGenFrameId.load() != currentGen) {
					return;
				}
				// it is still possible for the taskid to increment afterwards, but this shouldn't be of much issue
				this->RenderChunkAt(c, pChunk);
			};
			if (pChunk.distance < 0)
				threadPoolP->enqueueTask(task);
			else
				threadPool->enqueueTask(task);
		}
    }
}

ChunkRenderer::ChunkRenderer(World *w) : 
	world(w), 
	chunkShader("assets/shaders/game/chunk.v.glsl", "assets/shaders/game/chunk.f.glsl"), 
	chunkMeshes(), 
	queryShader("assets/shaders/game/query.v.glsl", "assets/shaders/game/query.f.glsl"),
	threadPool(std::make_unique<ThreadPool>(16)),
	threadPoolP(std::make_unique<ThreadPool>(4))
{

}

ChunkRenderer::~ChunkRenderer()
{
	std::cout << "deleting chunk renderer\n";
	queueCV.notify_all();  // 🚨 Wake up any threads blocked on queueCV
	this->loadThread.join();
    threadPool.reset(); // safely joins threads in ~ThreadPool()
	threadPoolP.reset();
	std::cout << "done";
}

bool ChunkRenderer::IsLoaded(ChunkPos pos)
{
	return this->chunkMeshes.contains(pos) && this->chunkMeshes.at(pos)->IsLoaded();
}

void ChunkRenderer::Init(GameContext *c)
{
	queryShader.Init(c);
	queryMesh.Init(c);
	queryMesh.SetAttributes({{3, GL_FLOAT, GL_FALSE, (GLsizei)(3 * sizeof(float)), (GLvoid*)(0), 1}});
	queryMesh.SetData(aabbVertices, sizeof(aabbVertices));

	glBindVertexArray(0);

	chunkShader.Init(c);
    this->loadThread = std::thread(&ChunkRenderer::RenderChunks, this, c);

}

void ChunkRenderer::Update(GameContext *c, double deltaTime)
{
    // Here is where chunk meshes need to be generated. 
    // This should be done on worker threads which return a completed mesh for the renderer to use
    
    // when chunks are modified, their pointer will be placed in a queue in world called "Dirty" to signal that the chunk has been updated
    // new chunk needs to have the same vao used

    for (int i = 0; i < std::min((size_t)10, chunkRemoveQueue.unsafe_size()); i++)
    {
        ChunkPos pos;
        if (!chunkRemoveQueue.try_pop(pos)) return;
		if (!chunkMeshes.contains(pos)) continue;
        auto &cptr = chunkMeshes.at(pos);
        // If it hasn’t finished loading, skip removal (it’ll be retried later)
        if (!cptr) continue;
        // Mark for removal and remove the chunk
        chunkMeshes.at(pos)->Clear();
		freeMeshes.push(chunkMeshes.at(pos));
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


void ChunkRenderer::drawChunkAABB(GameContext *c, const ChunkPos &pos)
{
	queryShader.use();
	glm::vec3 camPos = glm::vec3(
		-c->plr->chunkPos.x * CHUNK_X_SIZE,
		-c->plr->chunkPos.y * CHUNK_Y_SIZE,
		-c->plr->chunkPos.z * CHUNK_Z_SIZE
	);
	
	glm::vec3 chunkOffset = glm::vec3(
		pos.x * CHUNK_X_SIZE,
		pos.y * CHUNK_Y_SIZE,
		pos.z * CHUNK_Z_SIZE
	);
	
	// Combine them:
	glm::mat4 model = 
		glm::translate(glm::mat4(1.0f), camPos + chunkOffset);

	queryShader.setMat4("uMVP", c->plr->camera.proj * c->plr->camera.view * model);

	queryMesh.Render(GL_TRIANGLES);

}

void ChunkRenderer::Render(GameContext *c)
{
	glCall(glEnable(GL_DEPTH_TEST));
	glDepthMask(GL_TRUE);
    glCall(glEnable(GL_CULL_FACE));
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
	//glm::mat4 VP = glm::perspective(glm::radians(70.f), 1.0f, 0.1f, 2000.0f) * c->plr->camera.view;
	std::array<Plane, 6> frustumPlanes = extractFrustumPlanes(VP);
	chunkShader.setMat4("model", glm::translate(glm::mat4(1.0f), camPos));
	chunkShader.setVec3("plrPos", (glm::vec3)c->plr->pos - camPos);


	std::vector<std::shared_ptr<ChunkMesh>> meshes;
	std::vector<ChunkPos> transparent;
	for (int dz = -c->renderDistance; dz < c->renderDistance; dz++){
	for (int dy = -c->renderDistance; dy < c->renderDistance; dy++){
	for (int dx = -c->renderDistance; dx < c->renderDistance; dx++){
		ChunkPos pos = c->plr->chunkPos + ChunkPos{dx,dy,dz};
		if (!chunkMeshes.contains(pos)) continue;
		std::shared_ptr<ChunkMesh> mesh = chunkMeshes.at(pos);

		if (mesh->isInit())
			mesh->Update(c);
		else
			meshes.push_back(mesh);
		
		if (mesh->hasTranslucentBlocks()) transparent.push_back(pos);
		
		glm::vec3 chunkPos = glm::vec3(
			pos.x * CHUNK_X_SIZE, 
			pos.y * CHUNK_Y_SIZE, 
			pos.z * CHUNK_Z_SIZE
		);
		glm::vec3 minCorner = chunkPos + camPos;
		glm::vec3 maxCorner = minCorner + glm::vec3(CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE);

		if (isChunkVisible(frustumPlanes, minCorner, maxCorner)) {
			chunkShader.setVec3("chunkPos", chunkPos);
			mesh->RenderOpaque();
		} else {
			continue;
		}
	}
	}
	}

	if (!meshes.empty())
	{
		GLuint* vao = new GLuint[meshes.size()];
		GLuint* vbo = new GLuint[meshes.size()];
		glCall(glGenBuffers(meshes.size(), vbo));
		glCall(glGenVertexArrays(meshes.size(), vao));
	
		GLuint* vaoT = new GLuint[meshes.size()];
		GLuint* vboT = new GLuint[meshes.size()];
		glCall(glGenBuffers(meshes.size(), vboT));
		glCall(glGenVertexArrays(meshes.size(), vaoT));
		int i = 0;
		for(auto mesh : meshes) {
			mesh->Init(vao[i], vbo[i], vaoT[i], vboT[i]);
			i++;
		};

		c->glCleanupQueue.emplace([=]() {
			glCall(glDeleteBuffers(meshes.size(), vbo));
			glCall(glDeleteVertexArrays(meshes.size(), vao));
			glCall(glDeleteBuffers(meshes.size(), vboT));
			glCall(glDeleteVertexArrays(meshes.size(), vaoT));
		});
	}
	
	glDepthMask(GL_FALSE);
    glCall(glDisable(GL_CULL_FACE));
	for (auto& pos : transparent){
		if (!chunkMeshes.contains(pos)) continue;
		std::shared_ptr<ChunkMesh> mesh = chunkMeshes.at(pos);
		
		glm::vec3 chunkPos = glm::vec3(
			pos.x * CHUNK_X_SIZE, 
			pos.y * CHUNK_Y_SIZE, 
			pos.z * CHUNK_Z_SIZE
		);
		glm::vec3 minCorner = chunkPos + camPos;
		glm::vec3 maxCorner = minCorner + glm::vec3(CHUNK_X_SIZE, CHUNK_Y_SIZE, CHUNK_Z_SIZE);

		if (isChunkVisible(frustumPlanes, minCorner, maxCorner)) {
			chunkShader.setVec3("chunkPos", chunkPos);
			mesh->RenderTransparent();
		} else {
			continue;
		}
	}

}
