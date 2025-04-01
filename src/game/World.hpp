#pragma once

#include <SDL3/SDL.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <mutex>
#include <oneapi/tbb/concurrent_queue.h>

#include "GameContext.hpp"
#include "Terrain.hpp"
#include "../gfx/ChunkRenderer.hpp"
#include "../util/PriorityThreadPool.hpp"
#include "Chunk.hpp"
#include "PrioritizedChunk.hpp"
#include "ChunkPos.hpp"
#include "../gfx/Ray.hpp"


struct BlockFace {
    glm::ivec3 pos;
    int face = -1;

    glm::ivec3 getNeighbor()
    {
        switch(face){
        default:
        case 0:
            return pos + glm::ivec3({-1,0,0});
        case 1:
            return pos + glm::ivec3({ 1,0,0});
        case 2:
            return pos + glm::ivec3({0,-1,0});
        case 3:
            return pos + glm::ivec3({0, 1,0});
        case 4:
            return pos + glm::ivec3({0,0,-1});
        case 5:
            return pos + glm::ivec3({0,0, 1});
        }
    }
};

class World {
    Shader gizmoShader, skyShader, highlightShader;
    Mesh gizmoMesh, skyMesh, highlightMesh;
    ChunkRenderer renderer;
    std::shared_ptr<Terrain> terrain;
    std::thread loadThread;
    std::unique_ptr<PriorityThreadPool> threadPool;
    std::atomic<bool> loadSignal{false};
    int sX, sY, sZ, sF;
    // the intention is to build the BFS search order first
    void TraverseRays(GameContext *c);
    void LoadChunks(GameContext *c);
public:

    // TODO: make this part of the GameContext?
    // Shared queue and synchronization tools
    std::mutex queueLoadMutex, removeMutex;
    std::vector<ChunkPos> loadOrder;
    std::unordered_set<ChunkPos> modified;
    std::condition_variable queueCV;
    tbb::concurrent_queue<PrioritizedChunk> chunkLoadQueue;
    tbb::concurrent_queue<ChunkPos> chunkRemoveQueue;
    tbb::concurrent_queue<std::shared_ptr<Chunk>> freeChunks;
    tbb::concurrent_queue<Ray> rays;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<Chunk>> chunks; // this is a map of chunk positions to chunks loaded in memory
    World();
    ~World();
    bool AreAllNeighborsLoaded(const ChunkPos &pos);
    void Init(GameContext *c);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    bool ChunkReady(const ChunkPos &pos);
    bool ChunkLoaded(const ChunkPos &pos);
    bool ChunkMeshLoaded(const ChunkPos &pos);
    BLOCK_ID_TYPE GetBlockId(const ChunkPos &pos, int x, int y, int z);
    void SetBlockId(const ChunkPos &pos, int x, int y, int z, BLOCK_ID_TYPE id);
    void Render(GameContext *c);
    BlockFace RayTraversal(Ray ray, double tMin, double tMax);
};