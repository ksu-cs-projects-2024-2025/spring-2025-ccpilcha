#pragma once

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL.h>

#include "gfx/TextureArray.hpp"

class Player;
class World;

/**
 * @brief A container of arguments which get passed along to 
 * 
 * This is an object which has reference pointers to varius bits of information which might be useful.
 * 
 * When games are saved, (some of) this will be serialized
 * 
 */
struct GameContext {
    bool isFocused = true;
    SDL_Window *window;
    Player *plr;
    World *world;
    float fov;
    int width, height;
    float aspectRatio;
    float yawSensitivity;
    float pitchSensitivity;
    float moveSpeed;
    double maxBlockDistance;
    TextureArray texture;

    uint64_t seed; // world seed
    int renderDistance; // radius in chunks

    SDL_Keycode forward, backward, left, right, up, down, sprint;

    enum GameState {
        GAME_STATE_MENU,
        GAME_STATE_PLAYING,
        GAME_STATE_PAUSED
    } state;

    GameContext() : 
        fov(90.0f), 
        yawSensitivity(0.1f), 
        pitchSensitivity(0.1f), 
        texture("assets/textures/texturepack-simple.png") // TODO: i want all these variables to come from a config file
    {
        plr = nullptr;
        world = nullptr;

        moveSpeed = 20.f;

        renderDistance = 32;
        maxBlockDistance = 10.0f;

        forward = SDLK_W;
        backward = SDLK_S;
        left = SDLK_A;
        right = SDLK_D;
        up = SDLK_SPACE;
        down = SDLK_LSHIFT;
        sprint = SDLK_LALT;
    }
};