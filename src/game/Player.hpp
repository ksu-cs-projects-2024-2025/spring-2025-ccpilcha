#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "gfx/Camera.hpp"
#include "GameContext.hpp"
#include "Chunk.hpp"

class Player
{
    bool movement[7];
    bool focused = true;
public:
    BLOCK_ID_TYPE cursor = 1;
    ChunkPos chunkPos, lastPos;
    glm::dvec3 pos;
    Camera camera;
    Player();
    ~Player();
    void Init(GameContext *c, ChunkPos pos);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};