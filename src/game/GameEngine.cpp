#include "GameEngine.hpp"
#include "World.hpp"
#include "glad/glad.h"
#include "../util/GLHelper.hpp"
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>


GameEngine::GameEngine(GameContext *c) : context(c), plr(), 
gui()
{
    // TODO: make this work off of a json or yaml. imports all the stuff necessary
    int w, h;
    SDL_GetWindowSizeInPixels(c->window, &w, &h);
    UIComponent startButton;
    startButton.text = "PLAY";
    startButton.origin = glm::vec2(w/2, h/2);
    startButton.radius = glm::vec2(400.f, 100.f);
    startButton.borderWidth = 20.f;
    startButton.mainColor = glm::vec4(0.9f,0.9f,0.92f,1.0f);
    startButton.highlightColor = glm::vec4(0.95f,0.95f,0.95f,1.0f);
    startButton.shadowColor = glm::vec4(0.6f,0.6f,0.6f,1.0f);
    startButton.action = [this]() {
        this->newState = GameState::PLAY;
    };
    this->menuGUI.push_back(startButton);
}

GameEngine::~GameEngine()
{
    delete world;
}

SDL_AppResult GameEngine::Init() 
{

    return this->ChangeState();
}

SDL_AppResult GameEngine::ChangeState()
{
    this->state = newState;
    switch(state){
    case MENU:
        // We will want the mouse to be "grabbed"
        SDL_SetWindowRelativeMouseMode(context->window, false);
        gui.Init(context, menuGUI);
        break;
    case PLAY:
        this->world = new World();
        context->plr = &plr;
        context->world = world;
        // We will want the mouse to be "grabbed"
        SDL_SetWindowRelativeMouseMode(context->window, true);
        plr.Init(context, ChunkPos({-3,-1,12})); // TODO: get this to change based on height level
        world->Init(context);
        gui.Init(context, playGUI);
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::OnEvent(SDL_Event *event) {
    gui.OnEvent(context, event);    
    ImGuiIO& io = ImGui::GetIO();
    switch(state){
        case MENU:
            break;
        case PLAY:
            if (event->type == SDL_EVENT_KEY_DOWN) {
                switch (event->key.key) {
                    case SDLK_ESCAPE:
                        context->isFocused = false;
                }
            }
            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                if (event->button.button == SDL_BUTTON_LEFT) {
                    if (!io.WantCaptureMouse)
                        context->isFocused = true;
                }
            }
            if (SDL_GetWindowRelativeMouseMode(context->window) != context->isFocused) 
                SDL_SetWindowRelativeMouseMode(context->window, context->isFocused);
            world->OnEvent(context, event);
            plr.OnEvent(context, event);
            break;
    } 
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Update(double deltaTime) {
    if (this->state != this->newState) this->ChangeState();
    switch(state){
    case MENU:
        break;
    case PLAY:
        world->Update(context, deltaTime);
        plr.Update(context, deltaTime);
        break;
    }
    gui.Update(context, deltaTime);
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Render() {
    glCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));
    switch(state){
    case MENU:
        break;
    case PLAY:
        world->Render(context);
        plr.Render(context);
        break;
    }
    gui.Render(context);
    return SDL_APP_CONTINUE;
}