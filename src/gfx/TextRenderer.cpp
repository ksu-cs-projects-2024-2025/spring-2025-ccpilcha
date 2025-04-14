// TextRenderer.cpp
// This file was assisted by Copilot
#include <stb_image.h>
#include <SDL3_image/SDL_image.h>
#include "TextRenderer.hpp"
#include <glad/glad.h> // Or your OpenGL loader
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

TextRenderer::TextRenderer(): VAO(0), VBO(0), instanceVBO(0), textureID(0),
    shader("assets/shaders/ui/msdf.v.glsl", "assets/shaders/ui/msdf.f.glsl") {
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
}

void TextRenderer::LoadFontTexture(const std::string& filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4); // Force RGBA
    
    if (!data) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return;
    }
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
}

bool TextRenderer::LoadGlyphsFromJson(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Failed to open font JSON: " << path << std::endl;
        return false;
    }

    nlohmann::json j;
    in >> j;

    int scaleW = j["common"]["scaleW"];
    int scaleH = j["common"]["scaleH"];
    glm::vec2 texSize(scaleW, scaleH);

    for (const auto& ch : j["chars"]) {
        char c = static_cast<char>(ch["id"].get<int>());

        Glyph g;
        glm::vec2 pos(ch["x"], ch["y"]);
        glm::vec2 size(ch["width"], ch["height"]);

        g.uvMin = pos / texSize;
        g.uvMax = (pos + size) / texSize;
        g.size = size;
        g.offset = glm::vec2(ch["xoffset"], ch["yoffset"]);
        g.advance = ch["xadvance"];

        this->glyphs[c] = g;
    }

    return true;
}

void TextRenderer::SetText(const std::string& text, glm::vec2 pos, float scale) {
    instances.clear();
    currentText = text;
    basePosition = pos;
    currentScale = scale;

    float cursorX = pos.x;

    for (char c : text) {
        if (glyphs.find(c) == glyphs.end()) continue;
        const Glyph& g = glyphs.at(c);

        InstanceData instance;
        instance.offset = glm::vec2(cursorX + g.offset.x * scale, pos.y + g.offset.y * scale);
        instance.size = g.size * scale;
        instance.uvMin = g.uvMin;
        instance.uvMax = g.uvMax;

        instances.push_back(instance);
        cursorX += g.advance * scale;
    }

    glCall(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
    glCall(glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW));
}

void TextRenderer::Init(GameContext *c)
{
    // Quad geometry (fullscreen unit quad centered at origin)
    float quadVertices[8] = {
        0.0f, 1.0f,  // Top-left
        1.0f, 1.0f,  // Top-right
        0.0f, 0.0f,  // Bottom-left
        1.0f, 0.0f   // Bottom-right
    };

    glCall(glGenVertexArrays(1, &VAO));
    glCall(glGenBuffers(1, &VBO));
    glCall(glGenBuffers(1, &instanceVBO));

    glCall(glBindVertexArray(VAO));

    // Static quad VBO
    glCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW));
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0));

    // Instance buffer
    glCall(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
    glCall(glEnableVertexAttribArray(1)); // offset
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, offset)));
    glCall(glVertexAttribDivisor(1, 1));

    glCall(glEnableVertexAttribArray(2)); // size
    glCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size)));
    glCall(glVertexAttribDivisor(2, 1));

    glCall(glEnableVertexAttribArray(3)); // uvMin
    glCall(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uvMin)));
    glCall(glVertexAttribDivisor(3, 1));

    glCall(glEnableVertexAttribArray(4)); // uvMax
    glCall(glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uvMax)));
    glCall(glVertexAttribDivisor(4, 1));

    glCall(glBindVertexArray(0));
}

void TextRenderer::OnEvent(GameContext *c, const SDL_Event *event)
{
}

void TextRenderer::Update(GameContext *c, double deltaTime)
{
}

void TextRenderer::Render(GameContext *c)
{
    if (instances.size() == 0) return;
    
    glCall(glActiveTexture(GL_TEXTURE0));                     // Use texture unit 0
    glCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
    glCall(glBindVertexArray(this->VAO));
    shader.use();
    shader.setVec2("uScreenSize", glm::vec2(c->width, c->height));
    shader.setFloat("pxRange", 4.0f);       // MUST match distanceRange in .json
    shader.setVec4("bgColor", glm::vec4(1.0f,1.0f,1.0f,0.0f));
    shader.setVec4("fgColor", glm::vec4(0.0f,0.0f,0.0f,1.0f));
    shader.setInt("msdf", 0);                         // Bind sampler to texture unit 0
    glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instances.size()));
}
