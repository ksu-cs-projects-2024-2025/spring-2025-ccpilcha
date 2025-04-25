#pragma once

#include "GameContext.hpp"
#include "Player.hpp"
#include "gfx/UIComponent.hpp"
#include "gfx/UILayer.hpp"
#include "GameState.hpp"
#include "GameConsole.hpp"
#include "SoundDriver.hpp"

#include <imgui.h>

#include <vector>
#include <string>

class World;

class GameEngine
{
    bool showLastCommand = false;
    GameConsole console;
    std::vector<std::string> history;
    int history_pos = -1;
    CommandResult lastCommand;
    GameState state = GameState::MENU, newState = GameState::MENU;
    UILayer gui;
    std::vector<UIComponent> menuGUI, playGUI;
    GameContext *context;
    World *world;
    Player plr;
    SoundDriver soundDriver;

    CommandResult RunCommand(const Command& command);
public:
    int TextEditCallback(ImGuiInputTextCallbackData* data);
    GameEngine(GameContext *c);
    ~GameEngine();
    SDL_AppResult Init();
    SDL_AppResult ChangeState();
    SDL_AppResult OnEvent(SDL_Event *event);
    SDL_AppResult Update(double deltaTime);
    SDL_AppResult Render();
};