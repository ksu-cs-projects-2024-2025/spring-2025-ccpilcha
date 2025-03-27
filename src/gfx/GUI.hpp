#pragma once

#include <string>
#include <unordered_set>
#include <glm/glm.hpp>
#include "game/GameContext.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "game/GameState.hpp"

struct GUIelem {
    glm::vec2 origin, radius;

    float borderWidth;
    glm::vec4 mainColor, shadowColor, highlightColor;

    std::string text;

    GLuint texturebg, texturefg;

    std::function<void()> action;

    bool hover, select;

    void hovering() {
        hover |= hover;
    }

    bool isInside(glm::vec2 p) const
    {
        glm::vec2 minCorner = this->origin - this->radius;
        glm::vec2 maxCorner = this->origin + this->radius;
        return minCorner.x <= p.x && minCorner.y <= p.y 
            && maxCorner.x >= p.x && maxCorner.y >= p.y;
    }
};

class GUI
{
    Shader guiShader;
    Mesh guiMesh;
    std::vector<GUIelem> elements;
public:
    GUI();
    ~GUI();
    void Init(GameContext* c, std::vector<GUIelem> elements);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext* c, double deltaTime);
    void Render(GameContext* c);
};