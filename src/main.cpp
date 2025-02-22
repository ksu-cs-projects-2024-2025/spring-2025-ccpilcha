

#define SDL_MAIN_USE_CALLBACKS 1
#define G_DEBUG
#include <iostream>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

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
    Uint64 then = SDL_GetPerformanceCounter();
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
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // We want to have a double buffer to prevent image stitching.
    // Depth size determines clarity on depth testing (3D rendering)
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // 0 disables V-Sync in SDL
    // TODO: should we allow V-Sync as a configuration?
    SDL_GL_SetSwapInterval(0);

    // We can just default to 800x600 for now.
    window = SDL_CreateWindow(appname, 800, 600, SDL_WINDOW_OPENGL);

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

    // We will want the mouse to be "grabbed"
    SDL_SetWindowRelativeMouseMode(window, true);

    // Now, we need to prepare for OpenGL rendering on the window
    SDL_GLContext G_OpenGL_CONTEXT = SDL_GL_CreateContext(window);
    if (!G_OpenGL_CONTEXT) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    // Check to make sure GLAD was able to locate system GL functions
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        std::cout << "GLAD was not initialized"
            << std::endl;
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(window, G_OpenGL_CONTEXT);
    
    // Now we can start using OpenGL
    glViewport(0, 0, 800, 600);
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glEnable(GL_CULL_FACE));

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
    context->aspectRatio = 800/600.f;

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
    if (event->type == SDL_EVENT_WINDOW_RESIZED){ 
        glViewport(0, 0, event->window.data1, event->window.data2);
    }
    
    return game->OnEvent(event);
}

// The game loop
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    // This calculates the amount of time between the last frame and the current frame
    double deltaTime = (double)(now - then) / frequency;
    accTime += deltaTime;
    then = now;
    frameCount++;
    // About every 250ms, I'd like to refresh the console on the FPS
    if (accTime >= 0.25) {
        fprintf(stdout, "\rFPS: %.6f", (double) frameCount/accTime);
        accTime = 0.0;
        frameCount = 0;
        std::cout.flush();
    }
    // We need to update before we render
    game->Update(deltaTime);
    game->Render();
    SDL_GL_SwapWindow(window);
    return SDL_APP_CONTINUE;
}

// This function runs once at shutdown.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // TODO: make sure everything get saved!
    delete game;
    delete context;
}