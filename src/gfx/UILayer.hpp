#pragma once

#include <string>
#include <unordered_set>
#include <glm/glm.hpp>
#include "game/GameContext.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "game/GameState.hpp"
#include "TextRenderer.hpp"
#include "UIComponent.hpp"



class UILayer
{
    Shader guiShader;
    Mesh guiMesh;
    std::vector<UIComponent> elements;
public:
    TextRenderer textRenderer;
    UILayer();
    ~UILayer();
    void Init(GameContext* c, std::vector<UIComponent> elements);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext* c, double deltaTime);
    void Render(GameContext* c);
};