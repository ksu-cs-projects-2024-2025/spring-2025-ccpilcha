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
    plr.OnEvent(context, event);
    world->OnEvent(context, event);
                
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Update(double deltaTime) {
    plr.Update(context, deltaTime);
    world->Update(context, deltaTime);
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Render() {
    float color = context->plr->camera.pitch*0.002f;
    glCall(glClearColor(0.1f + color, 0.4f + color, 0.6f + color, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    plr.Render(context);
    world->Render(context);
    return SDL_APP_CONTINUE;
}