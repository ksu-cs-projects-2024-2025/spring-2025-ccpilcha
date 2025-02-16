#include "GameEngine.hpp"
#include "World.hpp"
#include "glad/gl.h"
#include "../util/GLHelper.hpp"

GameEngine::GameEngine(GameContext *c) : context(c), plr()
{
    world = new World();
    context->plr = &plr;
}

GameEngine::~GameEngine()
{
}

SDL_AppResult GameEngine::Init() {
    plr.Init(context, ChunkPos());
    world->Init(context);
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::OnEvent(SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_ESCAPE:
                context->isFocused = false;
        }
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
        if (event->button.button == SDL_BUTTON_LEFT) {
            context->isFocused = true;
        }
    }
    if (SDL_GetWindowRelativeMouseMode(context->window) != context->isFocused) 
        SDL_SetWindowRelativeMouseMode(context->window, context->isFocused);
    world->OnEvent(context, event);
    plr.OnEvent(context, event);
                
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Update(double deltaTime) {
    world->Update(context, deltaTime);
    plr.Update(context, deltaTime);
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Render() {
    glCall(glClearColor(0.43f, 0.71f, 0.9f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    world->Render(context);
    plr.Render(context);
    return SDL_APP_CONTINUE;
}