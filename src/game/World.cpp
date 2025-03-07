#include "World.hpp"

#include <thread>

bool World::AreAllNeighborsLoaded(const ChunkPos &pos)
{
    // Define neighbor offsets (adjust if you need more than 6 directions)
    constexpr std::array<ChunkPos, 6> neighborOffsets = {{
        ChunkPos{-1,  0,  0},
        ChunkPos{ 1,  0,  0},
        ChunkPos{ 0, -1,  0},
        ChunkPos{ 0,  1,  0},
        ChunkPos{ 0,  0, -1},
        ChunkPos{ 0,  0,  1}
    }};
    
    // Check each neighbor using atomic flag or safe retrieval of shared_ptr.
    for (const auto &offset : neighborOffsets) {
        ChunkPos neighborPos = pos + offset;
        if (!ChunkLoaded(neighborPos)) {
            return false;
        }
    }
    return true;
}

// this will allow for checking air blocks, even in neighboring chunks
BLOCK_ID_TYPE World::GetBlockId(const ChunkPos &pos, int x, int y, int z)
{
    // make sure this is a proper bound for a chunk
    int dx = 0, dy = 0, dz = 0;
    if (x < 0)              dx--;
    if (x >= CHUNK_X_SIZE)  dx++;
    if (y < 0)              dy--;
    if (y >= CHUNK_Y_SIZE)  dy++;
    if (z < 0)              dz--;
    if (z >= CHUNK_Z_SIZE)  dz++;
    x -= dx * CHUNK_X_SIZE;
    y -= dy * CHUNK_Y_SIZE;
    z -= dz * CHUNK_Z_SIZE;
    ChunkPos adjusted = pos + ChunkPos{dx,dy,dz};

    if (!ChunkLoaded(adjusted)) return 0;  // Ensure chunk is valid

    BLOCK_ID_TYPE block = chunks.at(adjusted)->GetBlockId(x,y,z);

    return block;
}

void World::SetBlockId(const ChunkPos &pos, int x, int y, int z, BLOCK_ID_TYPE id)
{
    // make sure this is a proper bound for a chunk
    int dx = 0, dy = 0, dz = 0;
    if (x < 0)              dx--;
    if (x >= CHUNK_X_SIZE)  dx++;
    if (y < 0)              dy--;
    if (y >= CHUNK_Y_SIZE)  dy++;
    if (z < 0)              dz--;
    if (z >= CHUNK_Z_SIZE)  dz++;
    x -= dx * CHUNK_X_SIZE;
    y -= dy * CHUNK_Y_SIZE;
    z -= dz * CHUNK_Z_SIZE;
    ChunkPos adjusted = pos + ChunkPos{dx,dy,dz};

    if (!ChunkLoaded(adjusted)) return;  // Ensure chunk is valid

    chunks.at(adjusted)->SetBlockId(x,y,z,id);

    renderer.chunkRenderQueue.push(adjusted);
    renderer.queueCV.notify_one();
}

bool gameRunning = true;

/**
 * @brief 
 * Implements the J. Amanatides and A. Woo ray tracing algorithm
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
        glm::vec3 step, tMax;
        step.x = (ray.direction.x >= 0) ? 1 : -1;
        step.y = (ray.direction.y >= 0) ? 1 : -1;
        step.z = (ray.direction.z >= 0) ? 1 : -1;
        

    }
}

/**
 * @brief Responsible for generating each chunk
 * This function will run on its own thread
 *
 */
void World::LoadChunks(GameContext *c)
{
    // TODO: this thread needs to end when the game exits the playing state
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueLoadMutex);
            queueCV.wait(lock, [this, c] { return loadSignal || !gameRunning; });

            if (!gameRunning) return;  // Exit thread when game ends

            loadSignal = false;
        }
        ChunkPos pPos = c->plr->chunkPos;
        for (int r = 0; r < c->renderDistance*sqrt(3)+1; r++) {
        for (int dz = -r; dz < r; dz++) {
        for (int dy = -r; dy < r; dy++) {
        for (int dx = -r; dx < r; dx++) {
            int x = pPos.x + dx, y = pPos.y + dy, z = pPos.z + dz;
            if (dx*dx + dy*dy > r*r) continue;
            ChunkPos nPos({x,y,z});
            if (!ChunkReady(nPos)) {
                this->chunks.emplace(nPos, std::make_shared<Chunk>(nPos));
                this->chunks.at(nPos)->Init(c);
                threadPool.enqueueTask([this, nPos]() {
                    if (ChunkReady(nPos)) {
                        this->chunks.at(nPos)->visible = this->terrain.getVisibilityFlags(nPos);
                        std::vector<CHUNK_DATA> data = this->terrain.generateChunk(nPos);
                        this->chunks.at(nPos)->Load(data);
                        if (this->chunks.at(nPos)->visible == 0) return;
                        if (!this->chunks.at(nPos)->IsEmpty())
                        {
                            renderer.chunkRenderQueue.push(nPos);
                            renderer.queueCV.notify_one();
                            // we should then queue another thread responsible for loading chunk vertex data
                        }
                        this->chunks.at(nPos)->loaded.store(true);
                    }
                });
            }
        }}}}
        for (const auto &[pos, cptr]: chunks)
        {
            if (pos.distance(pPos) > c->renderDistance) {
                chunkRemoveQueue.push(pos);
            }
        }
    }
}

World::World() : 
    terrain(), 
    chunks(), 
    renderer(this),
    threadPool(16),
    gizmoShader("assets/shaders/gizmo.v.glsl", "assets/shaders/gizmo.f.glsl"),
	skyShader("assets/shaders/sky.v.glsl", "assets/shaders/sky.f.glsl")
{
	
}

/**
 * @brief Initializes the world.
 * 
 * @param c 
 */
void World::Init(GameContext *c)
{
    terrain.seed = c->seed;
    renderer.Init(c);
    this->chunks.reserve(10000);
    // TODO: create thread which works on generating the world
    std::thread thr(&World::LoadChunks, this, c);
    thr.detach();
}

void World::OnEvent(GameContext *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            rays.emplace(Ray(c->plr->pos, c->plr->camera.forward));
        }
    }
}

/**
 * @brief This handles the loading and unloading of chunks based upon a player's position and render distance.
 * 
 * @param c 
 * @param deltaTime 
 */
void World::Update(GameContext *c, double deltaTime)
{
    TraverseRays(c);

    // we need to loop through chunks in a radius around the camera.
    // this needs to work from the center outward
    // check which chunks need to be loaded
    if (c->plr->chunkPos != c->plr->lastPos)
    {
        {
            std::lock_guard<std::mutex> lock(queueLoadMutex);
            loadSignal = true;
        }
        queueCV.notify_all();
    }

    for (int i = 0; i < std::min((size_t)10, chunkRemoveQueue.unsafe_size()); i++)
    {
        ChunkPos pos;
        if (!chunkRemoveQueue.try_pop(pos)) return;
        std::lock_guard<std::mutex> lock(removeMutex);
        auto &cptr = chunks.at(pos);
        // If it hasn’t finished loading, skip removal (it’ll be retried later)
        if (!cptr || !cptr->loaded.load())
            continue;
        // Mark for removal and remove the chunk
        cptr->removing.store(true);
    }

    // needs to be aware of the chunks which are queued for render
    renderer.Update(c, deltaTime);
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
    // RENDER SKY
    glDisable(GL_CULL_FACE);
    skyShader.use();
	skyShader.setMat4("projection", c->plr->camera.proj);
	skyShader.setMat4("view", glm::lookAt(glm::vec3(0.f), c->plr->camera.forward, c->plr->camera.up));
	skyMesh.RenderInstanceAuto(GL_TRIANGLE_STRIP, 4, 6);

    // RENDER WORLD
    renderer.Render(c);

    // RENDER GIZMO
    glDisable(GL_DEPTH_TEST);
	gizmoShader.use();
	gizmoShader.setMat4("projection", c->plr->camera.proj);
	gizmoShader.setMat4("view", glm::lookAt(-10.0f * c->plr->camera.forward, glm::vec3(0.f), c->plr->camera.up));
	// Set a fixed position for the gizmo on screen:
	gizmoShader.setMat4("model", glm::mat4(1.0f));

	gizmoMesh.RenderInstanceAuto(GL_LINES, 2, 3);
}


World::~World()
{
}