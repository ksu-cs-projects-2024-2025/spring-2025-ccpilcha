#pragma once

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL.h>

class Player;
class World;

// for now this will include default settings for the game.
// eventually i want a config file to be read in
struct GameContext {
    bool isFocused = true;
    SDL_Window *window;
    Player *plr;
    World *world;
    float fov;
    float aspectRatio;
    float yawSensitivity;
    float pitchSensitivity;
    float moveSpeed;

    uint64_t seed;

    SDL_Keycode forward, backward, left, right, up, down;

    enum GameState {
        GAME_STATE_MENU,
        GAME_STATE_PLAYING,
        GAME_STATE_PAUSED
    } state;

    GameContext() : fov(90.0f), yawSensitivity(0.1f), pitchSensitivity(0.1f) {
        plr = nullptr;
        world = nullptr;

        moveSpeed = 0.1f;

        forward = SDLK_W;
        backward = SDLK_S;
        left = SDLK_A;
        right = SDLK_D;
        up = SDLK_SPACE;
        down = SDLK_LSHIFT;
    }
};