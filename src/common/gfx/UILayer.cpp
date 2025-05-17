#include "UILayer.hpp"

UILayer::UILayer() :
    textRenderer(),
    guiShader("assets/shaders/ui/button.v.glsl", "assets/shaders/ui/button.f.glsl")
{
}

UILayer::~UILayer()
{
}

void UILayer::Init(GameContext *c)
{
    textRenderer.Init(c);
    guiShader.Init(c);
    guiMesh.Init(c);
    textRenderer.LoadGlyphsFromJson("assets/fonts/atlas.json");
    textRenderer.LoadFontTexture("assets/fonts/atlas.png");
}

void UILayer::Swap(std::vector<UIComponent> elements)
{
    this->elements = std::move(elements);
    textRenderer.Clear();

    for (auto &elem : this->elements)
    {

        glm::vec2 buttonSize = elem.radius;
        glm::vec2 textSize = textRenderer.MeasureText(elem.text);
    
        // Fit by width and height (maintain aspect ratio)
        float scaleX = buttonSize.x / textSize.x;
        float scaleY = buttonSize.y / textSize.y;
        float scale = std::min(scaleX, scaleY); // ensures text fits *within* box
        textRenderer.SetText(elem.text, elem.origin, elem.textAnchor, scale);
    }
}

void UILayer::OnEvent(GameContext *c, const SDL_Event *event)
{
    int w, h;
    SDL_GetWindowSizeInPixels(c->window, &w, &h);
    float pixelDensity = SDL_GetWindowPixelDensity(c->window);
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        for (auto &elem : elements)
            elem.hover = elem.isInside(glm::vec2(event->motion.x * pixelDensity, h-event->motion.y * pixelDensity));
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        for (auto &elem : elements)
        {
            if (elem.isInside(glm::vec2(event->motion.x * pixelDensity, h-event->motion.y * pixelDensity)))
                elem.select = true;
        }
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        for (auto &elem : elements)
        {
            elem.select = false;
            if (elem.isInside(glm::vec2(event->motion.x * pixelDensity, h-event->motion.y * pixelDensity)))
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
    SDL_GetWindowSizeInPixels(c->window, &w, &h);
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
