// TextRenderer.hpp
#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "game/GameContext.hpp"

struct Glyph {
    glm::vec2 uvMin;
    glm::vec2 uvMax;
    glm::vec2 size;
    glm::vec2 offset;
    float advance;
};

class TextRenderer {
    Shader shader;

    struct InstanceData {
        glm::vec2 offset;
        glm::vec2 size;
        glm::vec2 uvMin;
        glm::vec2 uvMax;
    };

    std::vector<InstanceData> instances;
    std::unordered_map<char, Glyph> glyphs;

    unsigned int VAO, VBO, instanceVBO;
    unsigned int textureID;
    unsigned int screenWidth, screenHeight;

    std::string currentText;
    glm::vec2 basePosition;
    float currentScale;

public:
    TextRenderer();
    ~TextRenderer();

    void Init(GameContext* c);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext* c, double deltaTime);
    void Render(GameContext* c);

    void LoadFontTexture(const std::string& filepath);
    bool LoadGlyphsFromJson(const std::string& path);
    void SetText(const std::string& text, glm::vec2 pos, float scale);
    
};
