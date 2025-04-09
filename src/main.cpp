

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SDL_MAIN_USE_CALLBACKS 1
#define G_DEBUG
#include <string>
#include <iostream>
#include <cstdint>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gpu.h>
#include <png.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <FastNoise/FastNoise.h>

#include "util/GLHelper.hpp"
#include "game/GameEngine.hpp"
#include "game/GameContext.hpp"

// Game instance stuff

// Name of the application
static const char		*appname = "Voxel Engine";
GameEngine				*game;
GameContext				*context;
static SDL_Window		*window = NULL;     // pointer to the window object
static SDL_GLContext	G_OpenGL_CONTEXT;   // OpenGL context for the window

// FPS and delta calculation

static double   accTime = 0.0;  // Used for printing FPS
static Uint64   then = 0;       // The previous timestamp from the last game loop iteration
static Uint64   frequency;      // Precision of the time counter
static int      frameCount = 0; // Keeps track off the nth frame

// Runs at start up
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Instantiate variables
    then = SDL_GetPerformanceCounter();
    frequency = SDL_GetPerformanceFrequency();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to initialize"
            << std::endl;
        exit(1);
    }
    
    // Enable true type format library
    if (!TTF_Init()) {
        std::cout << "SDL TTF failed to initialize"
            << std::endl;
        exit(1);
    }
    // We now need to declare the use of OpenGL version 4.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // We are not using compatability mode ()
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // This is important for MacOS because of Metal
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    // We want to have a double buffer to prevent image stitching.
    // Depth size determines clarity on depth testing (3D rendering)
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Before context creation (example using SDL):
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Try 4 or 8

    // 0 disables V-Sync in SDL
    // TODO: should we allow V-Sync as a configuration?
    SDL_GL_SetSwapInterval(-1);

    // We can just default to 800x600 for now.
    window = SDL_CreateWindow(appname, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    // Checking that the window initialized
    if (window == nullptr)
    {
        std::cout << "SDL_Window failed to initialize"
            << std::endl;
        exit(1);
    }

    // Configuring window settings
    SDL_SetWindowResizable(window, true);
    SDL_SetWindowMinimumSize(window, 200, 200);

    // Now, we need to prepare for OpenGL rendering on the window
    SDL_GLContext G_OpenGL_CONTEXT = SDL_GL_CreateContext(window);
    if (!G_OpenGL_CONTEXT) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    // Check to make sure GLAD was able to locate system GL functions
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "GLAD was not initialized"
            << std::endl;
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(window, G_OpenGL_CONTEXT);
    SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    
    // Now we can start using OpenGL

    // After OpenGL context is created:
    //glEnable(GL_MULTISAMPLE);

#ifdef G_DEBUG
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif

// CREATE GAME CONTEXT
    context = new GameContext();
    // By default, we need to direct the game context towards the current window.
    // We also need to declare the aspect ratio so we can properly calculate the projection matrix
    context->window = window;
    SDL_GetWindowSizeInPixels(window, &context->width, &context->height);
    context->aspectRatio = context->width / (float)context->height;
    
    glViewport(0, 0, context->width, context->height);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

// START ENGINE!
    game = new GameEngine(context);
    game->Init();

    return SDL_APP_CONTINUE;
}

// Runs when a new event (mouse input, keypresses, etc) occurs. This is on a separate thread!
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    // Allow the OpenGL viewport to automatically resize with respect to the window
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED){ 
        Uint32 flags = SDL_GetWindowFlags(window);

        if (flags & SDL_WINDOW_FULLSCREEN) {
            std::cout << "Exclusive fullscreen mode\n";
        } else if (flags & SDL_WINDOW_BORDERLESS) {
            std::cout << "Borderless fullscreen mode\n";
        } else {
            std::cout << "Windowed mode (or macOS fullscreen button?)\n";
        }
        context->width = event->window.data1;
        context->height = event->window.data2;
        context->aspectRatio = (float) event->window.data1 / (float) event->window.data2;
        glViewport(0, 0, event->window.data1, event->window.data2);
    }
    
    return game->OnEvent(event);
}

// The game loop
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 frameStart = SDL_GetTicks();
    // This calculates the amount of time between the last frame and the current frame
    double deltaTime = (double)(now - then) / frequency;
    accTime += deltaTime;
    then = now;
    frameCount++;
    // About every 250ms, I'd like to refresh the console on the FPS
    if (accTime >= 0.25) {
        SDL_SetWindowTitle(window, (appname + std::string(" - FPS: ") + std::to_string((double) frameCount/accTime)).c_str());
        //fprintf(stdout, "\rFPS: %.6f", (double) frameCount/accTime);
        accTime = 0.0;
        frameCount = 0;
        //std::cout.flush();
    }

    // We need to update before we render
    game->Update(deltaTime);
    game->Render();
    Uint64 frameEnd = SDL_GetTicks();
    int frameTime = frameEnd - frameStart;
    int targetDelay = 1000 / 144;

    if (frameTime < targetDelay)
        SDL_Delay(targetDelay - frameTime);

    SDL_GL_SwapWindow(window);
    return SDL_APP_CONTINUE;
}

// This function runs once at shutdown.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    context->isClosing = true;
    // TODO: make sure everything get saved!
    delete game;
    delete context;
}