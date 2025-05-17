#include "ModScript.hpp"

void ModScript::LoadMod(sol::state &lua)
{
    sol::load_result result = lua.load_file(path);
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua syntax/load error: " << err.what() << "\n";
        return;
    }

    sol::protected_function func = result;
    sol::protected_function_result exec = func();

    if (!exec.valid()) {
        sol::error err = exec;
        std::cerr << "Lua runtime error: " << err.what() << "\n";
        return;
    }

    // If main.lua returns a table (your mod table):
    sol::object returned = exec;
    if (returned.get_type() != sol::type::table) {
        std::cerr << "Mod did not return a valid table.\n";
        return;
    }

    this->script = returned.as<sol::table>();

    // 3. validate required functions
    static constexpr const char* requiredFunc[] = {
        "onInit",
        "onEvent",
        "onTick",
        "onExit"
    };
    for (auto fn : requiredFunc) {
        sol::object f = this->script[fn];
        if (f.get_type() != sol::type::function) {
            throw std::runtime_error(
                path + " is missing required function '" + fn + "'"
            );
        }
    }
    sol::object f = this->script["id"];
    if (f.get_type() != sol::type::string) throw std::runtime_error(
        path + " is missing an id!"
    );

    this->loaded = true;
    this->ID = f.as<std::string>();

    this->drawable = this->script["onFrame"] != sol::type::nil;
}