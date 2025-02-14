#include "World.hpp"

#include <thread>


// this will allow for checking air blocks, even in neighboring chunks
BLOCK_ID_TYPE World::GetBlockId(const ChunkPos &pos, int x, int y, int z)
{
    // make sure this is a proper bound for a chunk
    int dx = 0, dy = 0, dz = 0;
    if (x < 0)              dx++;
    if (x >= CHUNK_X_SIZE)  dx--;
    if (y < 0)              dy++;
    if (y >= CHUNK_Y_SIZE)  dy--;
    if (z < 0)              dz++;
    if (z >= CHUNK_Z_SIZE)  dz--;
    x += dx * CHUNK_X_SIZE;
    y += dy * CHUNK_Y_SIZE;
    z += dz * CHUNK_Z_SIZE;
    ChunkPos adjusted = pos + ChunkPos{dx,dy,dz};

    // check if this chunk exists
    if (!chunks.contains(adjusted)) return 0; // TODO: it would be cool if there was some lookup table or we could have macros for block IDs

    BLOCK_ID_TYPE block = chunks[adjusted]->GetBlockId(x,y,z);
    if (block != 0)
    {
        1;
    }

    return block;
}

bool gameRunning = true;

/**
 * @brief Responsible for generating each chunk
 * This function will run on its own thread
 * 
 */
void World::LoadChunks()
{
    // TODO: this thread needs to end when the game exits the playing state
    while (true)
    {
        ChunkPos pos;

        {
            std::unique_lock<std::mutex> lock(queueLoadMutex);
            queueCV.wait(lock, [this] { return !chunkLoadQueue.empty() || !gameRunning; });

            if (!gameRunning) return;  // Exit thread when game ends

            pos = chunkLoadQueue.front();
            chunkLoadQueue.pop();
        }

        std::vector<CHUNK_DATA> data = this->terrain.generateChunk(pos);

        this->chunks[pos]->blocks.insert(this->chunks[pos]->blocks.end(), data.begin(), data.end());

        {
            std::lock_guard<std::mutex> lock(renderer.queueRenderMutex);
            renderer.chunkRenderQueue.push(pos);
        }
        renderer.queueCV.notify_one();  // Wake up the chunk loader thread
        // we should then queue another thread responsible for loading chunk vertex data
    }
}

// Function to request a chunk load
void World::requestChunkLoad(ChunkPos pos) {
    {
        std::lock_guard<std::mutex> lock(queueLoadMutex);
        chunkLoadQueue.push(pos);
    }
    queueCV.notify_one();  // Wake up the chunk loader thread
}

World::World() : terrain(), chunks(), renderer(this)
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
    // TODO: create thread which works on generating the world
    std::thread thr(&World::LoadChunks, this);
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
    ChunkPos pos = c->plr->chunkPos;
    // check which chunks need to be loaded
    if (!ChunkLoaded(pos)) {
        this->chunks.emplace(pos, std::make_unique<Chunk>());
        this->chunks[pos]->Init(c);
        this->requestChunkLoad(pos);
    }

    // needs to be aware of the chunks which are queued for render
    renderer.Update(c, deltaTime);
}

bool World::ChunkLoaded(const ChunkPos &pos)
{
    return this->chunks.contains(pos);
}

void World::Render(GameContext *c)
{
    renderer.Render(c);
}


World::~World()
{
}