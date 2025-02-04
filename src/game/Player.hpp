#pragma once

#include <glm/glm.hpp>

namespace Game {
class Player
{
    
public:
    Player();
    ~Player();
    void update(double deltaTime);
    void render();
};
}