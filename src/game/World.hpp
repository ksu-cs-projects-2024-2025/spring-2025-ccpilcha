#pragma once

#include <SDL3/SDL.h>

#include "GameConfiguration.hpp"
#include "../gfx/Camera.hpp"

class World {

public:
    World();
    ~World();
    void init();
    void handleEvent(GameConfiguration *c, SDL_Event *event);
    void update(GameConfiguration *c, double deltaTime);
    void render(GameConfiguration *c);
};