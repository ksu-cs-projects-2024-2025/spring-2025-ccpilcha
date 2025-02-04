

#define SDL_MAIN_USE_CALLBACKS 1
#include <iostream>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "game/GameEngine.hpp"
#include "game/GameConfiguration.hpp"

static const char *appname = "Hello World";
GameEngine *game;
GameConfiguration *g_config;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static double deltaTime = 0.0;
static Uint64 then = 0;
static Uint64 frequency; 
static int frameCount = 0;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    Uint64 then = SDL_GetPerformanceCounter();
    frequency = SDL_GetPerformanceFrequency();
    /* Create the window */

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    if (!SDL_CreateWindowAndRenderer(appname, 800, 600, SDL_WINDOW_OPENGL, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    game = new GameEngine();
    game->init();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    
    return game->handleEvent(event);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    deltaTime += (double)(now - then) / frequency;
    then = now;
    frameCount++;
    if (deltaTime >= 0.25) {
        fprintf(stdout, "\rFPS: %.6f", (double) frameCount/deltaTime);
        deltaTime = 0.0;
        frameCount = 0;
        std::cout.flush();
    }
    game->update(deltaTime);
    game->render();
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}