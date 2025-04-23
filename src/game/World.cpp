/**
 * @file World.cpp
 * @author Cameron Pilchard (you@domain.com)
 * @brief 
 * @date 2025-03-07
 * 
 */

#include "World.hpp"
#include <unordered_set>
#include <imgui.h>
#include <cmath>

#include <thread>

/**
 * @brief Checks if all neighboring chunks exist and are loaded (have data)
 * This is a prerequisite to rendering a chunk since we want to know what faces we can cull
 * 
 * @param pos the position in question
 * @return true If all neighbors exist
 * @return false Otherwise
 */
bool World::AreAllNeighborsLoaded(const ChunkPos &pos)
{
    for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
    for (int dz = -1; dz <= 1; ++dz) {
        if (dx == 0 && dy == 0 && dz == 0) continue;
        ChunkPos neighbor = pos + ChunkPos{dx, dy, dz};
        if (!this->ChunkLoaded(neighbor))
            return false;
    }}}
    return true;
}

/**
 * @brief This will allow for checking air blocks, even in neighboring chunks (calculates offset as needed)
 * 
 * @param pos The position of the chunk
 * @param x relative block position coord X
 * @param y relative block position coord Y
 * @param z relative block position coord Z
 * @return the block present
 */
BLOCK_ID_TYPE World::GetBlockId(const ChunkPos &pos, int x, int y, int z)
{
    // make sure this is a proper bound for a chunk
    ChunkPos adjusted = ChunkPos::adjust(pos, x, y, z);

    if (!ChunkLoaded(adjusted)) 
        return 0;  // Ensure chunk is valid

    BLOCK_ID_TYPE block = chunks.at(adjusted)->GetBlockId(x,y,z);

    return block;
}

/**
 * @brief Changes a block within a chunk
 * 
 * @param pos The position of the chunk
 * @param x relative block position coord X
 * @param y relative block position coord Y
 * @param z relative block position coord Z
 * @return the block present
 */
void World::SetBlockId(const ChunkPos &pos, int x, int y, int z, BLOCK_ID_TYPE id)
{
    // make sure this is a proper bound for a chunk
    ChunkPos adjusted = ChunkPos::adjust(pos, x, y, z);

    glm::ivec3 blockPos{x,y,z};

    if (!ChunkLoaded(adjusted)) return;  // Ensure chunk is valid

    chunks.at(adjusted)->SetBlockId(x,y,z,id);

    if (this->modified.find(adjusted) == this->modified.end())
    {
        this->modified.insert(adjusted);
    }

    // we also have to push the surrounding border chunks for ambient occlusion updates
    if ((x == 0 || x == CHUNK_X_SIZE - 1) ||
        (y == 0 || y == CHUNK_Y_SIZE - 1) ||
        (z == 0 || z == CHUNK_Z_SIZE - 1))
    {
        // Loop through 3×3×3 region to determine affected chunks
        int nx,ny,nz;
        ChunkPos neighborChunk;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dz = -1; dz <= 1; dz++) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;
                    nx = x + dx, ny = y + dy, nz = z + dz;
                    neighborChunk = ChunkPos::adjust(adjusted, nx, ny, nz);
                    if ((nx > 0 && nx < CHUNK_X_SIZE - 1) &&
                        (ny > 0 && ny < CHUNK_Y_SIZE - 1) &&
                        (nz > 0 && nz < CHUNK_Z_SIZE - 1)) continue;
                    
                    renderer->chunkRenderQueue.push({neighborChunk, false, id != 0 ? -1 : std::numeric_limits<float>::lowest()});
                }
            }
        }
    }

    renderer->chunkRenderQueue.push({adjusted, id == 0, id == 0 ? -1 : std::numeric_limits<float>::lowest()});
    
    
    {
        std::lock_guard<std::mutex> lock(renderer->queueRenderMutex);
        renderer->loadSignal = true;
    }
    renderer->queueCV.notify_one();
}

/**
 * @brief Implements the J. Amanatides and A. Woo ray tracing algorithm
 * 
 * http://www.cs.yorku.ca/~amana/research/grid.pdf
 * @param c 
 */
void World::TraverseRays(GameContext *c)
{
    while (!rays.empty())
    {
        Ray ray;
        if (!rays.try_pop(ray)) continue;
        BlockFace block = RayTraversal(c, ray, 0, c->maxBlockDistance);
        if (block.face == -1) continue;
        glm::ivec3 neighbor = block.getNeighbor();
        if (ray.dig)
            this->SetBlockId(ray.originC, block.pos.x, block.pos.y, block.pos.z, 0);
        else {

            // Player's bounding box
            glm::dvec3 minBound = c->plr->pos - c->plr->radius;
            glm::dvec3 maxBound = c->plr->pos + c->plr->radius;

            // Block's position
            glm::dvec3 blockMin = glm::dvec3(neighbor.x, neighbor.y, neighbor.z);
            glm::dvec3 blockMax = glm::dvec3(neighbor.x + 1, neighbor.y + 1, neighbor.z + 1);

            // Check for overlap between the player's bounding box and the block's bounding box
            if ((blockMax.x < minBound.x || blockMin.x > maxBound.x || // No overlap on X-axis
                blockMax.y < minBound.y || blockMin.y > maxBound.y || // No overlap on Y-axis
                blockMax.z < minBound.z || blockMin.z > maxBound.z))  // No overlap on Z-axis
            {
                this->SetBlockId(ray.originC, neighbor.x, neighbor.y, neighbor.z, c->plr->cursor);
            }
        }
    }
}

BlockFace World::RayTraversal(GameContext *c, Ray ray, double tMin, double tMax)
{
    BlockFace block;
    glm::vec3 ray_start = ray.at(tMin), ray_end = ray.at(tMax);
    glm::ivec3 curPos, endPos, step;
    glm::vec3 v_tDelta, v_tMax;
    // initialization phase
    for (int i = 0; i < 3; i++)
    {
        curPos[i] = floor(ray_start[i]);
        endPos[i] = floor(ray_end[i]);
        if (ray.direction[i] > 0) {
            step[i] = 1;
            v_tDelta[i] = 1.0 / ray.direction[i];
            v_tMax[i] = (1 + curPos[i] - ray_start[i]) / ray.direction[i];
        } 
        else if (ray.direction[i] < 0) {
            step[i] = -1;
            v_tDelta[i] = -1.0 / ray.direction[i];
            v_tMax[i] = (curPos[i] - ray_start[i]) / ray.direction[i];
        } 
        else {
            step[i] = 0;
            v_tDelta[i] = tMax;
            v_tMax[i] = tMax;
        }
    }
    
    // traversal phase
    while (curPos.x != endPos.x || curPos.y != endPos.y || curPos.z != endPos.z)
    {
        
        if (v_tMax.x < v_tMax.y && v_tMax.x < v_tMax.z) {
            // X-axis traversal.
            curPos.x += step.x;
            v_tMax.x += v_tDelta.x;

            if (c->blockRegistry[this->GetBlockId(ray.originC, curPos.x, curPos.y, curPos.z)].isSelectable) {
                block = BlockFace({curPos, v_tMax.x, (step.x == 1) ? 0 : 1});
                break;
            }
        } else if (v_tMax.y < v_tMax.z) {
            // Y-axis traversal.
            curPos.y += step.y;
            v_tMax.y += v_tDelta.y;

            if (c->blockRegistry[this->GetBlockId(ray.originC, curPos.x, curPos.y, curPos.z)].isSelectable)
            {
                block = BlockFace({curPos, v_tMax.y, (step.y == 1) ? 2 : 3});
                break;
            }
        } else {
            // Z-axis traversal.
            curPos.z += step.z;
            v_tMax.z += v_tDelta.z;

            if (c->blockRegistry[this->GetBlockId(ray.originC, curPos.x, curPos.y, curPos.z)].isSelectable)
            {
                block = BlockFace({curPos, v_tMax.z, (step.z == 1) ? 4 : 5});
                break;
            }
        }
    }
    return block;
}

/**
 * @brief Responsible for generating each chunk
 * This function will run on its own thread
 *
 */
void World::LoadChunks(GameContext *c)
{
    // Here we need to load the modified chunks
    ChunkPos pPos = c->plr->chunkPos;
    std::ifstream in("chunk.json");
    if (in.is_open()) {

        nlohmann::json chunkList;
        in >> chunkList;
        // for now just save
        for (const auto& rle : chunkList["chunks"]) {
            int x = rle["x"];
            int y = rle["y"];
            int z = rle["z"];
            ChunkPos pos({x,y,z});
            if (!this->chunks.contains(pos)) // not sure how this would be false but whatever
                this->chunks[pos] = std::make_shared<Chunk>();
            this->chunks.at(pos)->Load_RLE(rle);
            this->modified.insert(pos);
            
            {
                std::lock_guard<std::mutex> lock(renderer->queueRenderMutex);
                renderer->loadSignal = true;
            }
            renderer->chunkRenderQueue.push({pos, 0});
            renderer->queueCV.notify_one();
        }
        
    }

    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueLoadMutex);
            queueCV.wait(lock, [this, c] { return c->isClosing || loadSignal; });

            if (c->isClosing) return;  // Exit thread when game ends

            loadSignal = false;
        }

        pPos = c->plr->chunkPos;

        for (const auto &[pos, cptr]: chunks)
        {
            ChunkPos rel = (pos - pPos);
            int x = abs(rel.x);
            int y = abs(rel.y);
            int z = abs(rel.z);
            if (x >= c->renderDistance * 1.5 || y >= c->renderDistance * 1.5 || z >= c->renderDistance * 1.5) {
                cptr->removing = true;
                chunkRemoveQueue.push(pos);
                this->renderer->chunkRemoveQueue.push(pos);
            }
        }

        // This looping in a radius extending outwards from the centermost chunk.
        for (const auto dPos : loadOrder) {
            int x = pPos.x + dPos.x, y = pPos.y + dPos.y, z = pPos.z + dPos.z;
            ChunkPos nPos({x,y,z});
            // if the chunk is not loaded (not in the hashtable) then we need to create the instance
            if (ChunkReady(nPos)) continue;

            if (!this->chunks.contains(nPos))
                this->chunks[nPos] = std::make_shared<Chunk>();
            this->chunks.at(nPos)->pos = nPos;
            this->chunks.at(nPos)->Init(c);
            Task task;
            task.priority = (float)nPos.distance(pPos);
            task.func = [this, c, pPos, nPos]() {
                if (ChunkReady(nPos)) {
                    if (!this->chunks.at(nPos)->loaded.load())
                    {
                        auto [visible, data] = this->terrain->generateChunk(nPos);            
                        this->chunks.at(nPos)->visible = this->terrain->getVisibilityFlags(nPos);
                        this->chunks.at(nPos)->Load(data);
                        this->chunks.at(nPos)->loaded.store(true); 
                    }
                    if (this->chunks.at(nPos)->visible == 0) return;
                    if (!this->chunks.at(nPos)->IsEmpty())
                    {
                        {
                            std::lock_guard<std::mutex> lock(renderer->queueRenderMutex);
                            renderer->loadSignal = true;
                        }
                        renderer->chunkRenderQueue.push({nPos, false, (float)nPos.distance(pPos)});
                        renderer->queueCV.notify_one();
                        // we should then queue another thread responsible for loading chunk vertex data
                    }
                    if (this->chunks.at(nPos)->pos != nPos) {
                        std::cerr << "[WARNING] Chunk position mismatch: rendered pos != memory pos!\n";
                        return;
                    }
                }
            };
            threadPool->enqueueTask(task);
        }
    }
}

World::World() : 
    terrain(std::make_unique<Terrain>()), 
    chunks(), 
    renderer(std::make_unique<ChunkRenderer>(this)),
    threadPool(std::make_unique<PriorityThreadPool>(16)),
    gizmoShader("assets/shaders/game/gizmo.v.glsl", "assets/shaders/game/gizmo.g.glsl", "assets/shaders/game/gizmo.f.glsl"),
	skyShader("assets/shaders/game/sky.v.glsl", "assets/shaders/game/sky.f.glsl"),
	highlightShader("assets/shaders/game/highlight.v.glsl", "assets/shaders/game/highlight.f.glsl"),
    postShader("assets/shaders/game/post.v.glsl","assets/shaders/game/post.f.glsl")
{

}
GLuint quadVAO, quadVBO;

/**
 * @brief Initializes the world.
 * 
 * @param c 
 */
void World::Init(GameContext *c)
{
    c->texture.Init(c);
    gizmoShader.Init(c);
    skyShader.Init(c);
    highlightShader.Init(c);
    postShader.Init(c);
    skyMesh.Init(c);
    gizmoMesh.Init(c);
    highlightMesh.Init(c);

    BlockInfo::LoadBlockRegistry(c->blockRegistry, "assets/textures/texturepack-simple.json");


    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
    
        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);    

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Generate and bind color texture
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, c->width, c->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Attach texture as color output
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    
    glGenTextures(1, &fboDepthTexture);
    glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, c->width, c->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture, 0);
    
    // Validate FBO
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete! Status: 0x" << std::hex << status << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    c->glCleanupQueue.emplace([=]() {
        glCall(glDeleteVertexArrays(1, &quadVAO));
        glCall(glDeleteBuffers(1, &quadVBO));
        glCall(glDeleteFramebuffers(1, &fbo));
        glCall(glDeleteTextures(1, &fboTexture));
        glCall(glDeleteTextures(1, &fboDepthTexture));
    });
    
    // Run this algorithm once
    std::unordered_set<ChunkPos> checkPos;
    int r = c->renderDistance;
    for (int dr = 0; dr < r; dr++) {
    for (int dz = -dr; dz < dr; dz++) {
    for (int dy = -dr; dy < dr; dy++) {
    for (int dx = -dr; dx < dr; dx++) {
        ChunkPos nPos({dx,dy,dz});
        if (checkPos.contains(nPos)) continue;
        loadOrder.push_back(nPos);
        checkPos.insert(nPos);
    }
    }
    }
    }

    terrain->seed = c->seed;
    renderer->Init(c);
    this->chunks.reserve(10000);
    // TODO: create thread which works on generating the world
    this->loadThread = std::thread(&World::LoadChunks, this, c);
}

void World::OnEvent(GameContext *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, c->width, c->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Attach texture as color output
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

        glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, c->width, c->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture, 0);

    }
}

/**
 * @brief This handles the loading and unloading of chunks based upon a player's position and render distance.
 * 
 * First, we need to handle all rays created by the player (just stores their position and previous camera.forward vector)
 * This is for creating/destroying blocks.
 * 
 * Then, if the player's position has changed since the last update cycle, we need to notify the Load thread to process all chunks which need to be rendered.alignas
 * @param c 
 * @param deltaTime This is in seconds!
 */
void World::Update(GameContext *c, double deltaTime)
{
    if (c->isClosing) return; // Exit if game is closing

    ImGui::Text("block: (%d, %d, %d)", (int)floor(c->plr->pos.x),(int) floor(c->plr->pos.y), (int)floor(c->plr->pos.z));
    ImGui::Text("chunk: (%lld, %lld, %lld)", c->plr->chunkPos.x, c->plr->chunkPos.y, c->plr->chunkPos.z); 
    ImGui::Text("pos:   (%lld, %lld, %lld)", (int)floor(c->plr->pos.x) + CHUNK_X_SIZE * c->plr->chunkPos.x, (int) floor(c->plr->pos.y) + CHUNK_Y_SIZE * c->plr->chunkPos.y, (int)floor(c->plr->pos.z) + CHUNK_Z_SIZE * c->plr->chunkPos.z);
    ImGui::Text("pos:   (%f, %f, %f)", c->plr->pos.x + CHUNK_X_SIZE * c->plr->chunkPos.x, c->plr->pos.y + CHUNK_Y_SIZE * c->plr->chunkPos.y, c->plr->pos.z + CHUNK_Z_SIZE * c->plr->chunkPos.z);
    phase += deltaTime / 2.0f;
    if (phase >= 1.0f) phase -= 1.0f;
    if (phase2 >= 1.0f) phase2 -= 1.0f;

    TraverseRays(c);

    // we need to loop through chunks in a radius around the camera.
    // this needs to work from the center outward
    // check which chunks need to be loaded
    if (c->plr->chunkPos != c->plr->lastPos)
    {
        renderer->chunkGenFrameId.fetch_add(1, std::memory_order_relaxed);
        {
            std::lock_guard<std::mutex> lock(queueLoadMutex);
            loadSignal = true;
        }
        queueCV.notify_all();
        std::cout << "\r" << c->plr->chunkPos;
        std::cout.flush();
    }

    for (int i = 0; i < chunkRemoveQueue.unsafe_size(); i++)
    {
        ChunkPos pos;
        if (!chunkRemoveQueue.try_pop(pos)) return;
        if (pos.distance(c->plr->chunkPos) <= c->renderDistance * 1.5) continue;
        auto &cptr = chunks.at(pos);
        // If it hasn’t finished loading, skip removal (it’ll be retried later)
        if (!cptr || !cptr->loaded.load() || !cptr->removing.load())
            continue;
        // Mark for removal and remove the chunk
        //cptr->Clear();
        //cptr->removing.store(false);
    }

    // needs to be aware of the chunks which are queued for render
    renderer->Update(c, deltaTime);
}

bool World::ChunkReady(const ChunkPos &pos)
{
    return this->chunks.contains(pos) && this->chunks.at(pos) != nullptr;
}

bool World::ChunkLoaded(const ChunkPos &pos)
{
    return this->ChunkReady(pos) && this->chunks.at(pos)->loaded;
}

void World::Render(GameContext *c)
{
	glCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear both

    GLenum status = glCallR(glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete! Status: " << std::hex << status << std::endl;
    }

    // RENDER SKY
	glDepthMask(GL_FALSE);
    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glDisable(GL_CULL_FACE));
    glm::mat4 skyview = glm::lookAt(glm::vec3(0.f), c->plr->camera.forward, c->plr->camera.up);
    skyShader.use();
	skyShader.setMat4("projection", c->plr->camera.proj);
	skyShader.setMat4("view", skyview);
	skyShader.setMat4("u_invProj", glm::inverse(c->plr->camera.proj));
	skyShader.setMat4("u_invView", glm::inverse(skyview));
	skyShader.setVec2("u_resolution", glm::vec2(c->width, c->height));
	skyMesh.RenderInstanceAuto(GL_TRIANGLE_STRIP, 4, 6);

    
    // RENDER WORLD
    renderer->Render(c);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);

    // Use post shader
    postShader.use();
    if (c->blockRegistry[this->GetBlockId(c->plr->chunkPos, c->plr->pos.x, c->plr->pos.y, c->plr->pos.z)].blockType == BlockType::Water)
    {
        postShader.setVec3("hint", glm::vec3(0.3f, 0.7f, 1.0f));
        postShader.setFloat("rippleStrength", 0.002f);
    } else {
        postShader.setVec3("hint", glm::vec3(1.0f));
        postShader.setFloat("rippleStrength", 0.0f);
    }
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    postShader.setInt("screenTexture", 0);
    postShader.setFloat("phase", phase * 2.0f * SDL_PI_F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboDepthTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // RENDER HIGHLIGHT
    BlockFace selected = RayTraversal(c, Ray(c->plr->camPos, c->plr->camera.forward, c->plr->chunkPos, false), 0, c->maxBlockDistance);
    if (selected.face != -1)
    {  
        highlightShader.use();
        highlightShader.setMat4("model", glm::mat4(1.0f));
        highlightShader.setMat4("projection", c->plr->camera.proj);
        highlightShader.setMat4("view", c->plr->camera.view);
        highlightShader.setInt("x", selected.pos.x);
        highlightShader.setInt("y", selected.pos.y);
        highlightShader.setInt("z", selected.pos.z);
        highlightShader.setInt("face", selected.face);
        highlightShader.setInt("screenTexture", 0);
        highlightShader.setInt("depthTexture", 1);
        highlightShader.setVec2("screenSize", glm::vec2(c->width, c->height));
        highlightShader.setFloat("phase", sqrt(1-pow(2*phase - 1,2)));
        highlightMesh.RenderInstanceAuto(GL_TRIANGLE_STRIP, 4, 1);
        
        ImGui::Text("block id: %d", this->GetBlockId(c->plr->chunkPos, selected.pos.x, selected.pos.y, selected.pos.z)); 
        ImGui::Text("block n:  %s", c->blockRegistry[this->GetBlockId(c->plr->chunkPos, selected.pos.x, selected.pos.y, selected.pos.z)].name.c_str()); 
    }

    // RENDER GIZMO
    glClear(GL_DEPTH_BUFFER_BIT); // clear both
    glEnable(GL_DEPTH_TEST);
	gizmoShader.use();
	gizmoShader.setMat4("projection", c->plr->camera.proj);
	gizmoShader.setMat4("view", glm::lookAt(-10.0f * c->plr->camera.forward, glm::vec3(0.f), c->plr->camera.up));
	// Set a fixed position for the gizmo on screen:
	gizmoShader.setMat4("model", glm::mat4(1.0f));
    gizmoShader.setVec2("u_viewportSize", glm::vec2(c->width, c->height));
    gizmoShader.setFloat("u_thickness", 8.0f);

	gizmoMesh.RenderInstanceAuto(GL_LINES, 2, 3);
}

World::~World()
{
    nlohmann::json chunkList;
    chunkList["chunks"] = nlohmann::json::array();
    // for now just save
    for (auto &pos : this->modified)
    {
        if (!this->chunks.contains(pos))  continue;
        nlohmann::json rle = this->chunks.at(pos)->To_RLE();
        chunkList["chunks"].push_back(rle);
    }
    std::ofstream out("chunk.json");
    out << chunkList.dump();

    this->threadPool.reset();
    queueCV.notify_all();
    this->loadThread.join();
}