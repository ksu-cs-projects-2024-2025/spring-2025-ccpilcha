#pragma once

#include "GameContext.hpp"
#include "Player.hpp"
#include "gfx/GUI.hpp"
#include "GameState.hpp"

class World;

class GameEngine
{
    GameState state = GameState::MENU, newState = GameState::MENU;
    GUI gui;
    std::vector<GUIelem> menuGUI, playGUI;
    GameContext *context;
    World *world;
    Player plr;
public:
    GameEngine(GameContext *c);
    ~GameEngine();
    SDL_AppResult Init();
    SDL_AppResult ChangeState();
    SDL_AppResult OnEvent(SDL_Event *event);
    SDL_AppResult Update(double deltaTime);
    SDL_AppResult Render();
};