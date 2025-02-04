#pragma once

#include "../gfx/GraphicsEngine.hpp"
#include "World.hpp"
#include "Player.hpp"

#ifndef GAMEENGINE_H
#define GAMEENGINE_H
class GameEngine
{
    GameConfiguration config;
    GraphicsEngine gfx;
    World world;
    Player plr;
public:
    GameEngine();
    ~GameEngine();
    SDL_AppResult init();
    SDL_AppResult handleEvent(SDL_Event *event);
    SDL_AppResult update(double deltaTime);
    SDL_AppResult render();
};
#endif