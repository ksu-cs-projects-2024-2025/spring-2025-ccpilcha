#include <fstream>
#include <filesystem>
#include "ModScript.hpp"
#include "LuaEngine.hpp"
#include "LuaBindings.hpp"
#include "objects/Event.hpp"

namespace fs = std::filesystem;

void LuaEngine::LoadMods(GameContext *c)
{
    if (!fs::exists(c->pathToMods)) return;

    for (auto const& dir_entry : fs::directory_iterator(c->pathToMods)) {
        if (!dir_entry.is_directory()) continue;

        fs::path mod_dir = dir_entry.path();
        fs::path main_lua = mod_dir / "main.lua";

        if (!fs::exists(main_lua)) {
            std::cerr << "Skipping “" << mod_dir << "”: no main.lua\n";
            continue;
        }

        ModScript script(main_lua);
        script.LoadMod(this->lua);
        if (script.isLoaded()) mods.insert_or_assign(script.getID(), script);
    }
}

LuaEngine::LuaEngine()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::coroutine);
    env = sol::environment(lua, sol::create);
}

LuaEngine::~LuaEngine() {

    for (auto& [id, mod] : mods)
    {
        mod.Invoke_onExit(lua);
    }
}

void LuaEngine::Init(GameContext *c) {

    // bind all datatypes 
    AVE::registerLuaBindings(this->env, this->lua);

    // load all mods
    this->LoadMods(c);

    for (auto& [id, mod] : mods)
    {
        mod.Invoke_onInit(lua);
    }
}

/**
 * @brief 
 * 
 * Lua can't really run on separate threads. 
 * My solution is to just dispatch all of the events via the main thread
 * @param c 
 * @param event 
 */
void LuaEngine::OnEvent(GameContext *c, const SDL_Event *event) {

    if (event->type != SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
         && event->type != SDL_EVENT_KEY_DOWN
         && event->type != SDL_EVENT_KEY_UP
        ) 
        return;

    events.push_back(*event);
}

void LuaEngine::Update(GameContext *c, double deltaTime) {
    while (!events.empty())
    {
        auto event = events.front();
        events.pop_front();
        /* code */
        AVE::Event::Type type;
        std::vector<sol::object> args;
        if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) { 
            type = AVE::Event::Type::RESIZE;
            
            args.push_back(sol::make_object(lua, c->width));
            args.push_back(sol::make_object(lua, c->height));
        }
        else if (event.type == SDL_EVENT_KEY_DOWN){ 
            type = AVE::Event::Type::KEY_DOWN;
            
            args.push_back(sol::make_object(lua, (int)event.key.key));
        }
        else if (event.type == SDL_EVENT_KEY_UP){ 
            type = AVE::Event::Type::KEY_UP;
            
            args.push_back(sol::make_object(lua, (int)event.key.key));
        }
        else {
            continue;
        }
        AVE::Event ev(type, args);

        for (auto& [id, mod] : mods)
        {
            mod.Invoke_onEvent(lua, ev);
        }
    }

    for (auto& [id, mod] : mods)
    {
        mod.Invoke_onTick(lua, deltaTime);
    }
    
}

void LuaEngine::Render(GameContext *c) {
    auto now = SDL_GetPerformanceCounter();
    for (auto& [id, mod] : mods)
    {
        if (mod.isDrawable())
        mod.Invoke_onFrame(lua, (now - then)/(double)SDL_GetPerformanceFrequency());
    }
    then = now;
}
