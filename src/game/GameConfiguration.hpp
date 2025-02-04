#pragma once

#include <SDL3/SDL_keycode.h>

class Player;

struct GameConfiguration {
    Player *plr;
    float fov;
    float yawSensitivity;
    float pitchSensitivity;
    float moveSpeed;

    SDL_Keycode forward, backward, left, right, up, down;

    GameConfiguration() : fov(90.0f), yawSensitivity(0.1f), pitchSensitivity(0.1f) {
        moveSpeed = 0.1f;

        forward = SDLK_W;
        backward = SDLK_S;
        left = SDLK_A;
        right = SDLK_D;
        up = SDLK_SPACE;
        down = SDLK_LSHIFT;
    }
};