#pragma once

#include <SDL3/SDL.h>

#include "../game/GameContext.hpp"
#include "VertexAttribute.hpp"
#include "ChunkVertex.hpp"
#include "Shader.hpp"

class ChunkRenderer
{   
    GLuint vao;
    Shader chunkShader;
    std::vector<VertexAttribute> ChunkVertexAttribs = {
        {3, GL_INT, GL_FALSE, sizeof(ChunkVertex), 0},
        {1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(int))},
        {1, GL_INT, GL_FALSE, sizeof(ChunkVertex), (void*)(4 * sizeof(int))}
    };
public:
    ChunkRenderer();
    ~ChunkRenderer();
    void Init(GameContext *c);
    void OnEvent(GameContext *c, SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};