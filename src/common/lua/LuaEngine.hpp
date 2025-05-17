#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <SDL3/SDL_events.h>
#include <deque>
#include <unordered_map>
#include <string>

#include "game/GameContext.hpp"
#include "game/ChunkPos.hpp"
#include "ModScript.hpp"


class LuaEngine
{
    Uint64 then;
    sol::state lua;
    sol::environment env; 
    std::unordered_map<std::string, ModScript> mods;
    std::deque<SDL_Event> events;
    void LoadMods(GameContext *c);
public:
    LuaEngine();
    ~LuaEngine();
    void Init(GameContext *c);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};