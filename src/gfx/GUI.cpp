#include "GUI.hpp"

GUI::GUI() :
    guiShader("assets/shaders/ui/button.v.glsl", "assets/shaders/ui/button.f.glsl")
{
}

GUI::~GUI()
{
}

void GUI::Init(GameContext *c, std::vector<GUIelem> elements)
{
    this->elements = std::move(elements);

}

void GUI::OnEvent(GameContext *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        for (auto &elem : elements)
        {
            if (elem.isInside(glm::vec2(event->motion.x, event->motion.y)))
            {
                elem.hover = true;
            }
        }
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        for (auto &elem : elements)
        {
            if (elem.isInside(glm::vec2(event->motion.x, event->motion.y)))
            {
                elem.action();
            }
        }
    }
}

void GUI::Update(GameContext *c, double deltaTime)
{
}

void GUI::Render(GameContext *c)
{
    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glDisable(GL_CULL_FACE));
    guiShader.use();
    int w, h;
    SDL_GetWindowSize(c->window, &w, &h);
    guiShader.setVec2("screenSize", glm::vec2(w, h));
    for (const auto &elem : elements)
    {
        guiShader.setVec2("origin", elem.origin);
        guiShader.setVec2("radius", elem.radius);
        guiShader.setFloat("borderWidth", elem.borderWidth);
        guiShader.setVec4("mainColor", elem.mainColor);
        guiShader.setVec4("highlightColor", elem.highlightColor);
        guiShader.setVec4("shadowColor", elem.shadowColor);
        guiMesh.RenderInstanceAuto(GL_TRIANGLE_STRIP, 4, 1);
    }
}
