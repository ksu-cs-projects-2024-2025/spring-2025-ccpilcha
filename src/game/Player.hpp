#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "Camera.hpp"
#include "GameContext.hpp"
#include "Chunk.hpp"

class Player
{
    bool movement[7];
public:
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