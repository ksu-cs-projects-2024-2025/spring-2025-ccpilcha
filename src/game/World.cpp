#include "World.hpp"

#include <thread>


// this will allow for checking air blocks, even in neighboring chunks
BLOCK_ID_TYPE World::GetBlockId(const ChunkPos &pos, int8_t x, int8_t y, int8_t z)
{
    // make sure this is a proper bound for a chunk
    ChunkPos adjusted = pos + ChunkPos{x/CHUNK_X_SIZE,y/CHUNK_Y_SIZE,z/CHUNK_Z_SIZE};
    x %= CHUNK_X_SIZE;
    y %= CHUNK_Y_SIZE;
    z %= CHUNK_Z_SIZE;

    // check if this chunk exists
    if (!chunks.contains(adjusted)) return 0; // TODO: it would be cool if there was some lookup table or we could have macros for block IDs

    //return chunks[adjusted].GetBlockId(x,y,z);
    return 0;
}


// Shared queue and synchronization tools
std::queue<ChunkPos> chunkQueue;
std::mutex queueMutex;
std::condition_variable queueCV;
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
        ChunkPos chunk;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [] { return !chunkQueue.empty() || !gameRunning; });

            if (!gameRunning) return;  // Exit thread when game ends

            chunk = chunkQueue.front();
            chunkQueue.pop();
        }


    }
}

// Function to request a chunk load
void requestChunkLoad(ChunkPos pos) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        chunkQueue.push(pos);
    }
    queueCV.notify_one();  // Wake up the chunk loader thread
}

World::World() : terrain(), chunks(), renderer()
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
    // TODO: create thread which works on generating the world
    //std::thread(LoadChunks);
}

void World::OnEvent(GameContext *c, SDL_Event *event)
{
}

/**
 * @brief 
 * 
 * @param c 
 * @param deltaTime 
 */
void World::Update(GameContext *c, double deltaTime)
{
    // we need to loop through chunks in a radius around the camera.
    // this needs to work from the center outward

    // needs to be aware of the chunks which are queued for render
    renderer.Update(c, deltaTime);
}

void World::Render(GameContext *c)
{
    renderer.Render(c);
}


World::~World()
{
}