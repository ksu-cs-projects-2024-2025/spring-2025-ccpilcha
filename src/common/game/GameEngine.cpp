#include "GameEngine.hpp"
#include "World.hpp"
#include "glad/glad.h"
#include "../util/GLHelper.hpp"
#include <SDL3/SDL.h>
#include <variant>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

CommandResult GameEngine::RunCommand(const Command &command)
{
    auto result = CommandResult();

    constexpr auto defaultToFloat = [](const CommandToken& token) -> float
    {
        float v;
        const float *vf = std::get_if<float>(&token);
        if (vf) v = *vf;
        else v = (float) std::get<int>(token);
        return v;
    };

    try {
        switch (command.type) {
            case Command::Type::TELEPORT:
                float x = defaultToFloat(command.args.at(0).token);
                float y = defaultToFloat(command.args.at(1).token);
                float z = defaultToFloat(command.args.at(2).token);
                auto absPlr = plr.chunkPos.abs(plr.pos);
                auto newPosRel = glm::dvec3(x,y,z) - absPlr;
                plr.chunkPos = ChunkPos::adjust(plr.chunkPos, newPosRel);
                plr.pos += newPosRel;

                result.code = 0;
                result.message << "Successfully teleported to " << x << ", " << y << ", " << z;
                break;
        }
    } catch (std::exception& e)
    {
        result.code = -1; //TODO: make codes which represent the type of error like too few args
    }

    this->console.LogCommand(command, result);
    return result;
}

GameEngine::GameEngine(GameContext *c) : context(c), plr(),
                                         gui()
{
    luaEngine = new LuaEngine();
    // TODO: make this work off of a json or yaml. imports all the stuff necessary
    int w, h;
    SDL_GetWindowSizeInPixels(context->window, &w, &h);
    UIComponent startButton;
    startButton.text = U"PLAY";
    startButton.textAnchor = glm::vec2(0.5f, 0.5f);
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
    delete luaEngine;
    delete world;
}

SDL_AppResult GameEngine::Init() 
{
    luaEngine->Init(context);
    gui.Init(context);
    return this->ChangeState();
}

SDL_AppResult GameEngine::ChangeState()
{
    this->state = newState;
    switch(state){
    case MENU:
        // We will want the mouse to be "grabbed"
        SDL_SetWindowRelativeMouseMode(context->window, false);
        gui.Swap(menuGUI);
        break;
    case PLAY:
        this->world = new World();
        context->plr = &plr;
        context->world = world;
        // We will want the mouse to be "grabbed"
        SDL_SetWindowRelativeMouseMode(context->window, true);
        plr.Init(context, ChunkPos({-2,-1,7})); // TODO: get this to change based on height level
        world->Init(context);
        gui.Swap(playGUI);
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::OnEvent(SDL_Event *event) {
    luaEngine->OnEvent(context, event);
    gui.OnEvent(context, event);    
    ImGuiIO& io = ImGui::GetIO();
    switch(state){
        case MENU:
            break;
        case PLAY:
            world->OnEvent(context, event);
            plr.OnEvent(context, event);
            if (event->type == SDL_EVENT_KEY_DOWN) {
                switch (event->key.key) {
                    case SDLK_TAB:
                        context->isFocused = false;
                        break;
                }
            }
            if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                if (event->button.button == SDL_BUTTON_LEFT) {
                    context->isFocused = !(io.WantCaptureMouse);
                }
            }
            if (SDL_GetWindowRelativeMouseMode(context->window) != context->isFocused) 
                SDL_SetWindowRelativeMouseMode(context->window, context->isFocused);
            break;
    } 
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::Update(double deltaTime) {
    luaEngine->Update(context, deltaTime);
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

int GameEngine::TextEditCallback(ImGuiInputTextCallbackData* data) {
    // only called when CallbackHistory flag is set
    if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        // save old pos
        int prev_pos = history_pos;
        if (data->EventKey == ImGuiKey_UpArrow) {
            // move up in history
            if (history_pos == -1) 
                history_pos = (int)history.size() - 1;
            else if (history_pos > 0)
                history_pos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow) {
            // move down (or back to fresh)
            if (history_pos != -1) {
                if (history_pos < (int)history.size() - 1)
                    history_pos++;
                else
                    history_pos = -1;
            }
        }

        // if position changed, update buffer
        if (prev_pos != history_pos) {
            const char* hist_str = (history_pos >= 0) 
                ? history[history_pos].c_str() 
                : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, hist_str);
        }
    }
    return 0;
}

// static trampoline
static int CallbackStatic(ImGuiInputTextCallbackData* data) {
    // recover `this` pointer
    GameEngine* self = static_cast<GameEngine*>(data->UserData);
    return self->TextEditCallback(data);
}

SDL_AppResult GameEngine::Render() {
    luaEngine->Render(context);
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

SDL_AppResult GameEngine::RenderDebug()
{
    switch(state){
        case MENU:
            break;
        case PLAY:
            world->RenderDebug(context);
            plr.RenderDebug(context);
            break;
    }
    std::string input;
    if (ImGui::InputText("Command", &input, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, CallbackStatic, this))
    {
        history_pos = -1;
        history.push_back(input);
        auto command = this->console.ParseCommand(input);
        lastCommand = this->RunCommand(command);
        showLastCommand = true;
    }
    if (ImGui::IsItemActivated()) showLastCommand = false;
    if (showLastCommand) ImGui::Text("%s", lastCommand.message.str().c_str());
    return SDL_APP_CONTINUE;
}
