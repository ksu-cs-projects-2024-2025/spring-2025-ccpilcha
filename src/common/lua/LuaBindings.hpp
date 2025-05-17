#pragma once
#include "bindings/UI_Bindings.hpp"
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include "objects/Event.hpp"

namespace AVE {

    // Wraps any table into a read-only proxy that throws on reassignment
    template<typename F>
    sol::table create_readonly_api(sol::state& lua, F fillTable) {
        sol::table raw = lua.create_table();
        fillTable(raw);
        sol::table proxy = lua.create_table();
        
        // Set up a metatable with __index (for access) and __newindex (to block writes)
        sol::table mt = lua.create_table_with(
            "__index", raw,
            "__newindex", [](sol::object /*self*/, sol::object key, sol::object /*value*/) {
                std::string k = key.is<std::string>() ? key.as<std::string>() : "?";
                throw std::runtime_error("Attempt to override protected function: " + k);
            }
        );

        proxy[sol::metatable_key] = mt;
        return proxy;
    }

    void registerLuaBindings(sol::environment& env, sol::state& lua) {

        lua.new_usertype<AVE::Event>("Event",
            "type", &AVE::Event::type,
            "argc", &AVE::Event::argc,
            "arg", &AVE::Event::arg
        );
        
        sol::table eventType = lua.create_table_with(
            "KEY_DOWN",     static_cast<int>(AVE::Event::Type::KEY_DOWN),
            "KEY_UP",       static_cast<int>(AVE::Event::Type::KEY_UP),
            "MOUSE_DOWN",   static_cast<int>(AVE::Event::Type::MOUSE_DOWN),
            "MOUSE_UP",     static_cast<int>(AVE::Event::Type::MOUSE_UP),
            "MOUSE_MOVE",   static_cast<int>(AVE::Event::Type::MOUSE_MOVE),
            "RESIZE",       static_cast<int>(AVE::Event::Type::RESIZE)
        );
        
        // Create a metatable that blocks overwriting
        sol::table mt = lua.create_table();
        mt.set_function("__newindex", [](sol::this_state, sol::table, sol::object, sol::object) {
            throw std::runtime_error("Attempt to modify read-only enum table 'EventType'");
        });
        
        eventType[sol::metatable_key] = mt;
        lua["EventType"] = eventType;
        
        // vectors
        lua.new_usertype<glm::vec2>("vec2",
            sol::call_constructor,          
            sol::constructors<
                glm::vec2(),
                glm::vec2(float),
                glm::vec2(float, float)
            >(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            sol::meta_function::addition,       sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(glm::operator+),
            sol::meta_function::subtraction,    sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(glm::operator-),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec2& lhs, const glm::vec2& rhs) {
                    return lhs * rhs;  // vector * vector
                },
                [](const glm::vec2& lhs, float scalar) {
                    return lhs * scalar;  // vector * scalar
                },
                [](float scalar, const glm::vec2& rhs) {
                    return scalar * rhs;  // scalar * vector
                }
            ),
            sol::meta_function::division, 
            [](const glm::vec2& m, float scalar) {
                return m / scalar;
            },
            sol::meta_function::equal_to,      
            [](const glm::vec2& v1, const glm::vec2& v2) {
                return v1 == v2;
            },
            sol::meta_function::to_string,
            [](glm::vec2 const& v) {
                std::ostringstream ss;
                ss << "vec2(\n";
                for (int r = 0; r < 2; ++r) {
                  ss << "  [" << v[r] << "]\n";
                }
                ss << ")";
                return ss.str();
            }
        );

        lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,          
            sol::constructors<
                glm::vec3(),
                glm::vec3(float),
                glm::vec3(float, float, float)
            >(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition,       sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::operator+),
            sol::meta_function::subtraction,    sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(glm::operator-),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec3& lhs, const glm::vec3& rhs) {
                    return lhs * rhs;  // matrix * matrix
                },
                [](const glm::vec3& lhs, float scalar) {
                    return lhs * scalar;  // matrix * scalar
                },
                [](float scalar, const glm::vec3& rhs) {
                    return scalar * rhs;  // scalar * matrix
                }
            ),
            sol::meta_function::division, 
            [](const glm::vec3& m, float scalar) {
                return m / scalar;
            },
            sol::meta_function::equal_to,      
            [](const glm::vec3& v1, const glm::vec3& v2) {
                return v1 == v2;
            }
        );

        lua.new_usertype<glm::vec4>("vec4",
            sol::call_constructor,          
            sol::constructors<
                glm::vec4(),
                glm::vec4(float),
                glm::vec4(float, float, float, float)
            >(),
            "x", &glm::vec4::x,
            "y", &glm::vec4::y,
            "z", &glm::vec4::z,
            "w", &glm::vec4::w,
            sol::meta_function::addition,       sol::resolve<glm::vec4(const glm::vec4&, const glm::vec4&)>(glm::operator+),
            sol::meta_function::subtraction,    sol::resolve<glm::vec4(const glm::vec4&, const glm::vec4&)>(glm::operator-),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec4& lhs, const glm::vec4& rhs) {
                    return lhs * rhs;  // matrix * matrix
                },
                [](const glm::vec4& lhs, float scalar) {
                    return lhs * scalar;  // matrix * scalar
                },
                [](float scalar, const glm::vec4& rhs) {
                    return scalar * rhs;  // scalar * matrix
                }
            ),
            sol::meta_function::division, 
            [](const glm::vec4& m, float scalar) {
                return m / scalar;
            },
            sol::meta_function::equal_to,      
            [](const glm::vec4& v1, const glm::vec4& v2) {
                return v1 == v2;
            }
        );

        // matrices
        auto mat2_type = lua.new_usertype<glm::mat2>("mat2",
            sol::call_constructor,          
            sol::constructors<
                glm::mat2(),
                glm::mat2(float)
            >(),
            sol::meta_function::addition,       sol::resolve<glm::mat2(const glm::mat2&, const glm::mat2&)>(glm::operator+),
            sol::meta_function::subtraction,    sol::resolve<glm::mat2(const glm::mat2&, const glm::mat2&)>(glm::operator-),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::mat2& lhs, const glm::vec2& rhs) {
                    return lhs * rhs;  // matrix * vector
                },
                [](const glm::mat2& lhs, const glm::mat2& rhs) {
                    return lhs * rhs;  // matrix * matrix
                },
                [](const glm::mat2& lhs, float scalar) {
                    return lhs * scalar;  // matrix * scalar
                },
                [](float scalar, const glm::mat2& rhs) {
                    return scalar * rhs;  // scalar * matrix
                }
            ),
            sol::meta_function::division, 
            [](const glm::mat2& m, float scalar) {
                return m / scalar;
            },
            sol::meta_function::equal_to,      
            [](const glm::mat2& v1, const glm::mat2& v2) {
                return v1 == v2;
            },
            // convert mat2 -> string
            sol::meta_function::to_string,
            [](glm::mat2 const& M) {
                std::ostringstream ss;
                ss << "mat2(\n";
                for (int r = 0; r < 2; ++r) {
                  ss << "  [" << M[0][r] << ", " << M[1][r] << "]\n";
                }
                ss << ")";
                return ss.str();
            }
        );
        lua["mat2"]["set"] = [](glm::mat2& m, const sol::table& t) {
            for (int col = 0; col < 2; ++col) {
                sol::table colTbl = t[col + 1];
                for (int row = 0; row < 2; ++row) {
                    m[col][row] = colTbl[row + 1];
                }
            }
        };
    }
}