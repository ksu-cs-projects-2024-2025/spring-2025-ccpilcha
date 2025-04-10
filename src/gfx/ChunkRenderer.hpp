/**
 * @file ChunkRenderer.hpp
 * @author Cameron Pilchard
 * @brief 
 * @version 0.1
 * @date 2025-03-07
 * 
 * @copyright Copyright (c) 2025
 * 
 * This is the manager for the rendering of chunks 
 */

#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <oneapi/tbb/concurrent_priority_queue.h>
#include <SDL3/SDL.h>
#include <thread>
#include <shared_mutex>

#include "game/GameContext.hpp"
#include "game/PrioritizedChunk.hpp"
#include "game/ChunkPos.hpp"
#include "util/ThreadPool.hpp"
#include "Mesh.hpp"
#include "VertexAttribute.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "BlockInfo.hpp"
#include "Shader.hpp"

class World;

class ChunkRenderer
{   
    // TODO: i am considering if I want to switch to using a UBO instead of changing uniform variables per chunk draw call
    GLuint ubo;
    // this is the shader object for all chunks
    Shader chunkShader;
    // this is the thread worker pool which will process/render chunks
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<ThreadPool> threadPoolP;

    std::thread loadThread;

    RenderType ParseRenderType(const std::string& str) {
        if (str == "Opaque") return RenderType::Opaque;
        if (str == "Translucent") return RenderType::Translucent;
        if (str == "Cutout") return RenderType::Cutout;
        return RenderType::Invisible;
    }
    
    void LoadBlockRegistry(const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "Failed to load block metadata: " << path << "\n";
            return;
        }
    
        nlohmann::json data;
        in >> data;
    
        for (auto& entry : data["blocks"]) {
            int id = entry["id"];
            BlockInfo info;
            info.name = entry["name"];
            info.renderType = ParseRenderType(entry["renderType"]);
            info.emitsLight = entry["emitsLight"];
            info.isCollidable = entry["isCollidable"];
            info.hardness = entry["hardness"];
            if (entry.contains("textureIndices"))
                info.textureIndices = entry["textureIndices"].get<std::array<int, 6>>();
            blockRegistry[id] = info;
        }
    }

    std::unordered_map<int, BlockInfo> blockRegistry;
    
    World* world;
    void RenderChunkAt(PrioritizedChunk pos);
    void RenderChunks(GameContext *c);
public:
    std::atomic<int> chunkGenFrameId = 0;
    // the render queue is for chunks which are ready to be rendered
    // the remove queue is for chunks which are ready to be deleted
    tbb::concurrent_priority_queue<PrioritizedChunk> chunkRenderQueue;
    tbb::concurrent_queue<ChunkPos> chunkRemoveQueue;
    // instead of creating more mesh instances, we can just use one from here
    tbb::concurrent_queue<std::shared_ptr<ChunkMesh>> freeMeshes;
    tbb::concurrent_unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>> chunkMeshes;
    std::mutex queueRenderMutex;
    std::condition_variable queueCV;
    ChunkRenderer(World*);
    ~ChunkRenderer();
    bool IsLoaded(ChunkPos pos);
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};