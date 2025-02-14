#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <queue>

#include <thread>

#include "../game/GameContext.hpp"
#include "VertexAttribute.hpp"
#include "ChunkPos.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"

class World;

class ChunkRenderer
{   
    GLuint vao;
    Shader chunkShader;
    std::unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>> chunkMeshes;
    std::queue<ChunkMesh*> queue;
    World* world;
    void RenderChunkAt(ChunkPos pos);
    void RenderChunks();

public:
    std::queue<ChunkPos> chunkRenderQueue;
    std::mutex queueRenderMutex;
    std::condition_variable queueCV;
    ChunkRenderer(World*);
    ~ChunkRenderer();
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};