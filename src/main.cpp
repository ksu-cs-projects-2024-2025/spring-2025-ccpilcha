

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

static const char		*appname = "Hello World";
GameEngine				*game;
GameContext				*context;
static SDL_Window		*window = NULL;
static SDL_GLContext	G_OpenGL_CONTEXT;

static double   accTime = 0.0;
static Uint64   then = 0;
static Uint64   frequency; 
static int      frameCount = 0;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, 
                                GLenum severity, GLsizei length, 
                                const GLchar* message, const void* userParam) {
    std::cerr << "GL CALLBACK: " << message << std::endl;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    Uint64 then = SDL_GetPerformanceCounter();
    frequency = SDL_GetPerformanceFrequency();
    /* Create the window */

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to initialize"
            << std::endl;
        exit(1);
    }
    TTF_Init();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetSwapInterval(0); // 0 disables V-Sync in SDL

    window = SDL_CreateWindow("VoxelEngine", 800, 600, SDL_WINDOW_OPENGL);
    SDL_SetWindowResizable(window, true);
    SDL_SetWindowRelativeMouseMode(window, true);

    if (window == nullptr)
    {
        std::cout << "SDL_Window failed to initialize"
            << std::endl;
        exit(1);
    }

    SDL_GLContext G_OpenGL_CONTEXT = SDL_GL_CreateContext(window);
    if (!G_OpenGL_CONTEXT) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        std::cout << "GLAD was not initialized"
            << std::endl;
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(window, G_OpenGL_CONTEXT);

    glViewport(0, 0, 800, 600);
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glEnable(GL_CULL_FACE));

#ifdef G_DEBUG
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif

    context = new GameContext();
    context->window = window;
    context->aspectRatio = 800/600.f;
    game = new GameEngine(context);
    game->Init();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED){ 
        glViewport(0, 0, event->window.data1, event->window.data2);
    }
    
    return game->OnEvent(event);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    double deltaTime = (double)(now - then) / frequency;
    accTime += deltaTime;
    then = now;
    frameCount++;
    if (accTime >= 0.25) {
        fprintf(stdout, "\rFPS: %.6f", (double) frameCount/accTime);
        accTime = 0.0;
        frameCount = 0;
        std::cout.flush();
    }
    game->Update(deltaTime);
    game->Render();
    SDL_GL_SwapWindow(window);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // TODO: make sure everything get saved!
    delete game;
    delete context;
}