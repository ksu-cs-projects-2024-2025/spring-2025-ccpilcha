#pragma once

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL.h>
#include <string>
#include <mutex>
#include <queue>
#include <functional>


#include "gfx/TextureArray.hpp"
#include "BlockInfo.hpp"

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
    std::atomic<bool> isClosing = false;
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

    uint64_t seed = 112351; // world seed
    int renderDistance; // radius in chunks

    SDL_Keycode forward, backward, left, right, up, down, zenith, nadir, sprint, toggleFly, jump;

    std::unordered_map<int, BlockInfo> blockRegistry;

    std::string pathToTextureJSON;
    std::string pathToIcons;
    std::string pathToMods;

    std::mutex glCleanupMutex;
    std::queue<std::function<void()>> glCleanupQueue;

    GameContext() : 
        fov(110.0f), 
        yawSensitivity(1.f), 
        pitchSensitivity(1.f), 
        texture("assets/textures/texturepack-simple.png") // TODO: i want all these variables to come from a config file
    {
        pathToIcons = "assets/icon/";
        pathToMods = "assets/mods/";
        pathToTextureJSON = "assets/textures/texturepack-simple.json";
        plr = nullptr;
        world = nullptr;

        moveSpeed = 20.f;

        renderDistance = 16; 
        maxBlockDistance = 10.0f;

        toggleFly    =   SDLK_F1;
        forward      =   SDLK_W;
        backward     =   SDLK_S;
        left         =   SDLK_A;
        right        =   SDLK_D;
        up           =   SDLK_E;
        down         =   SDLK_Q;
        zenith       =   SDLK_SPACE;
        nadir        =   SDLK_LSHIFT;
        jump         =   SDLK_SPACE;
        sprint       =   SDLK_LALT;
    }
};