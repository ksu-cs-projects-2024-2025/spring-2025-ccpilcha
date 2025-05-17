#pragma once

#include "objects/Event.hpp"
#include <sol/sol.hpp>
#include <string>


class ModScript {
    sol::table script;
    sol::environment env;
    std::string path;
    std::string ID;

    bool loaded = false;
    bool drawable = false;

public:

    ModScript(const std::string& path) : path(path) {}

    void LoadMod(sol::state& lua);
    bool isLoaded() { return loaded; }
    bool isDrawable() { return drawable; }
    std::string getID() { return ID; }
    void Invoke_onInit  (sol::state& lua)               { script["onInit"]  (); }
    void Invoke_onEvent (sol::state& lua, const AVE::Event& e){ script["onEvent"] (sol::make_object_userdata<AVE::Event>(lua, e)); }    
    void Invoke_onTick  (sol::state& lua, float delta)  { script["onTick"]  (sol::make_object(lua, delta)); }    
    void Invoke_onFrame (sol::state& lua, float delta)  { script["onFrame"] (sol::make_object(lua, delta)); }    
    void Invoke_onExit  (sol::state& lua)               { script["onExit"]  (); }    
};