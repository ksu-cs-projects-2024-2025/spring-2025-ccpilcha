#include "UILayer.hpp"

UILayer::UILayer() :
    textRenderer(),
    guiShader("assets/shaders/ui/button.v.glsl", "assets/shaders/ui/button.f.glsl")
{
}

UILayer::~UILayer()
{
}

void UILayer::Init(GameContext *c, std::vector<UIComponent> elements)
{
    this->elements = std::move(elements);
}

void UILayer::OnEvent(GameContext *c, const SDL_Event *event)
{
    int w, h;
    SDL_GetWindowSize(c->window, &w, &h);
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        for (auto &elem : elements)
            elem.hover = elem.isInside(glm::vec2(event->motion.x, h-event->motion.y));
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        for (auto &elem : elements)
        {
            if (elem.isInside(glm::vec2(event->motion.x, h-event->motion.y)))
                elem.select = true;
        }
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        for (auto &elem : elements)
        {
            elem.select = false;
            if (elem.isInside(glm::vec2(event->motion.x, h-event->motion.y)))
                elem.action();
        }
    }
}

void UILayer::Update(GameContext *c, double deltaTime)
{
    textRenderer.Update(c, deltaTime);
}

void UILayer::Render(GameContext *c)
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
        guiShader.setBool("isClicked", elem.select);
        guiMesh.RenderInstanceAuto(GL_TRIANGLE_STRIP, 4, 1);
    }
    textRenderer.Render(c);
}
