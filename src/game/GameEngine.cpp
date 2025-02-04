#include "GameEngine.hpp"

GameEngine::GameEngine() : world(), plr(), config()
{
    config.plr = &plr;
}

GameEngine::~GameEngine()
{
}

SDL_AppResult GameEngine::init() {
    world.init();
    plr.init();
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::handleEvent(SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_ESCAPE:
                return SDL_APP_SUCCESS;
            default:
                
                return SDL_APP_CONTINUE;
        }
    }
    world.handleEvent(&config, event);
                
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::update(double deltaTime) {
    world.update(&config, deltaTime);
    return SDL_APP_CONTINUE;
}

SDL_AppResult GameEngine::render() {
    world.render(&config);
    return SDL_APP_CONTINUE;
}