#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <SDL3/SDL_events.h>
#include <vector>

#include "game/GameContext.hpp"
#include "game/ChunkPos.hpp"
#include "ModScript.hpp"


class LuaScript
{
    
    sol::state& lua;
    sol::environment& env; 
    ModScript mod;
    std::vector<const SDL_Event*> events;

public:
    LuaScript(sol::state& lua, sol::environment& env);
    ~LuaScript();
    void Init(GameContext *c);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};