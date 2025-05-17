#pragma once

#include <sol/sol.hpp>

namespace AVE {
    struct Event {
        std::vector<sol::object> args;  // store Lua-side arguments

        enum class Type {
            KEY_DOWN = 1,     
            KEY_UP,       
            MOUSE_DOWN,   
            MOUSE_UP,     
            MOUSE_MOVE,   
            RESIZE  
        } type;
    
        Event(const Type& type, const std::vector<sol::object>& args)
            : type(type), args(args) {}
    
        int argc() const { return static_cast<int>(args.size()); }
    
        sol::object arg(int i, sol::this_state ts) const {
            if (i < 1 || i > args.size()) {
                return sol::lua_nil;  // Lua is 1-indexed
            }
            return args[i - 1];  // args[0] = Lua arg 1
        }
    };
}