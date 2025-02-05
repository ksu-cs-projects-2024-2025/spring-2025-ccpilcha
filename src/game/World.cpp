#include "World.hpp"

World::World() : terrain(), chunks(), renderer()
{
}

void World::Init(GameContext *c)
{
    terrain.seed = c->seed;
}

void World::OnEvent(GameContext *c, SDL_Event *event)
{
}

void World::Update(GameContext *c, double deltaTime)
{
    // physics stuff can go here!!
}

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

    return 0;
}

void World::Render(GameContext *c)
{
    
}


World::~World()
{
}