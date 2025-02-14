#pragma once

#include "game/GameContext.hpp"


// TODO: Information will be displayed here such as
// fps
class HUD
{
public:
    HUD();
    ~HUD();
    void Init(GameContext* c);
    void Update(GameContext* c, double deltaTime);
    void Render(GameContext* c);
};