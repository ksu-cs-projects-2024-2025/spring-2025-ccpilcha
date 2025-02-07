#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <queue>

#include "../game/GameContext.hpp"
#include "VertexAttribute.hpp"
#include "ChunkMesh.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"

class ChunkMesh;

class ChunkRenderer
{   
    GLuint vao;
    Shader chunkShader;
    std::vector<VertexAttribute> ChunkVertexAttribs = {
        {3, GL_INT, GL_FALSE, sizeof(ChunkVertex), 0},
        {1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(int))},
        {1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(4 * sizeof(int))}
    };
    std::unordered_map<ChunkPos, ChunkMesh> chunkMeshes;
    std::queue<ChunkMesh*> queue;
public:
    ChunkRenderer();
    ~ChunkRenderer();
    void Init(GameContext *c);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};