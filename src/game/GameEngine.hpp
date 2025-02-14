#pragma once

#include "GameContext.hpp"
#include "Player.hpp"

class World;

class GameEngine
{
    GameContext *context;
    World *world;
    Player plr;
public:
    GameEngine(GameContext *c);
    ~GameEngine();
    SDL_AppResult Init();
    SDL_AppResult OnEvent(SDL_Event *event);
    SDL_AppResult Update(double deltaTime);
    SDL_AppResult Render();
};