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

    BLOCK_ID_TYPE block = chunks[adjusted]->GetBlockId(x,y,z);

    return block;
}

bool gameRunning = true;

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
            queueCV.wait(lock, [this, c] { return c->plr->chunkPos != c->plr->lastPos || !gameRunning; });

            if (!gameRunning) return;  // Exit thread when game ends

        }
        ChunkPos pPos = c->plr->chunkPos;
        for (int r = -c->renderDistance+1; r < c->renderDistance+1; r++) {
        for (int dz = -r; dz < r; dz++) {
        for (int dy = -r; dy < r; dy++) {
        for (int dx = -r; dx < r; dx++) {
            int x = pPos.x + dx, y = pPos.y + dy, z = pPos.z + dz;
            if (dx*dx + dy*dy > r*r) continue;
            ChunkPos nPos({x,y,z});
            if (!ChunkReady(nPos)) {
                this->chunks[nPos] = std::make_shared<Chunk>();
                this->chunks[nPos]->Init(c);
                threadPool.enqueueTask([this, nPos]() {
                    std::vector<CHUNK_DATA> data = this->terrain.generateChunk(nPos);
                    this->chunks[nPos]->Load(data);
                    if (!this->chunks[nPos]->IsEmpty())
                    {
                        renderer.chunkRenderQueue.push(nPos);
                        renderer.queueCV.notify_one();
                        // we should then queue another thread responsible for loading chunk vertex data
                    }
                });
            }
        }}}}
    }
}

World::World() : terrain(), chunks(), renderer(this), threadPool(8)
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

void World::OnEvent(GameContext *c, SDL_Event *event)
{
}

/**
 * @brief This handles the loading and unloading of chunks based upon a player's position and render distance.
 * 
 * @param c 
 * @param deltaTime 
 */
void World::Update(GameContext *c, double deltaTime)
{
    // we need to loop through chunks in a radius around the camera.
    // this needs to work from the center outward
    // check which chunks need to be loaded
    if (c->plr->chunkPos != c->plr->lastPos)
    {
        
        queueCV.notify_one();  // Wake up the chunk loader thread
    }

    // needs to be aware of the chunks which are queued for render
    renderer.Update(c, deltaTime);
}

bool World::ChunkReady(const ChunkPos &pos)
{
    return this->chunks.contains(pos) && this->chunks[pos] != nullptr;
}

bool World::ChunkLoaded(const ChunkPos &pos)
{
    return this->ChunkReady(pos) && this->chunks[pos]->loaded;
}

void World::Render(GameContext *c)
{
    renderer.Render(c);
}


World::~World()
{
}