#pragma once

namespace Game {
class GameEngine
{
public:
    GameEngine();
    ~GameEngine();
    void init();
    void handleEvent();
    void update(double deltaTime);
    void render();
};
}