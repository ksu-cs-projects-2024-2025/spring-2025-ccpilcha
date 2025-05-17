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
}

void TextRenderer::LoadFontTexture(const std::string& filepath) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
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

    emSize = j["metrics"]["emSize"];
    ascender = j["metrics"]["ascender"];
    // Store in class if needed
    
    // Get atlas size
    int atlasWidth = j["atlas"]["width"];
    int atlasHeight = j["atlas"]["height"];
    glm::vec2 texSize(atlasWidth, atlasHeight);

    try {

        // Loop through each glyph
        for (const auto& glyph : j["glyphs"]) {
            if (!glyph.contains("planeBounds") || !glyph.contains("atlasBounds"))
                continue; // skip unsupported or invisible glyphs
            int unicode = glyph.at("unicode");
            char32_t c = static_cast<char32_t>(unicode); // only safe for ASCII range

            Glyph g;

            // Size in EM units
            glm::vec2 planeMin = {
                glyph.at("planeBounds").at("left"),
                glyph.at("planeBounds").at("bottom")
            };
            glm::vec2 planeMax = {
                glyph.at("planeBounds").at("right"),
                glyph.at("planeBounds").at("top")
            };

            // Size in pixels in atlas
            glm::vec2 atlasMin = {
                glyph.at("atlasBounds").at("left"),
                glyph.at("atlasBounds").at("bottom")
            };
            glm::vec2 atlasMax = {
                glyph.at("atlasBounds").at("right"),
                glyph.at("atlasBounds").at("top")
            };

            g.uvMin = atlasMin / texSize;
            g.uvMax = atlasMax / texSize;
            g.offset = planeMin;                        // in ems; adjust depending on your coordinate system
            g.size = planeMax - planeMin;
            g.advance = glyph["advance"];

            this->glyphs[c] = g;
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
    in.close();

    return true;
}

glm::vec2 TextRenderer::MeasureText(const std::u32string& text) {
    float width = 0.0f;
    float maxAboveBaseline = 0.0f;
    float maxBelowBaseline = 0.0f;

    for (char32_t c : text) {
        if (glyphs.find(c) == glyphs.end()) continue;
        const Glyph& g = glyphs.at(c);

        width += g.advance;

        float top = g.offset.y;
        float bottom = g.offset.y - g.size.y;
        maxAboveBaseline = std::max(maxAboveBaseline, top);
        maxBelowBaseline = std::min(maxBelowBaseline, bottom);
    }

    float height = maxAboveBaseline - maxBelowBaseline;
    return { width, height };
}

void TextRenderer::SetText(const std::u32string& text, glm::vec2 pos, glm::vec2 anchor, float scale) {
    auto textSize = MeasureText(text); // Update the text size
    currentText = text;
    basePosition = pos - anchor * textSize * scale; // Adjust the base position based on anchor
    currentScale = scale;

    float cursorX = basePosition.x;
    float cursorY = basePosition.y;

    for (char32_t c : text) {
        if (glyphs.find(c) == glyphs.end()) continue;
        const Glyph& g = glyphs.at(c);

        InstanceData instance;
        instance.offset = glm::vec2(cursorX + g.offset.x * scale, cursorY + g.offset.y * scale);
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
    shader.Init(c);

    // Quad geometry (fullscreen unit quad centered at origin)
    // Vertex layout: vec2 position, vec2 uv
    float quadVertices[] = {
        // aPos     // aUV
        0.0f, 1.0f,  0.0f, 1.0f, // Top-left
        1.0f, 1.0f,  1.0f, 1.0f, // Top-right
        0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
        1.0f, 0.0f,  1.0f, 0.0f  // Bottom-right
    };

    glCall(glGenVertexArrays(1, &VAO));
    glCall(glGenBuffers(1, &VBO));
    glCall(glGenBuffers(1, &instanceVBO));

    glCall(glBindVertexArray(VAO));

    // Static quad VBO
    glCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW));// Per-vertex data (unit quad)

    // Each vertex = vec2 pos + vec2 uv = 4 floats = 16 bytes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // aUV
    glEnableVertexAttribArray(1);
    
    // Per-instance data
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    
    // Offset
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, offset));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    
    // Size
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
    
    // UV Min
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uvMin));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    
    // UV Max
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uvMax));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    c->glCleanupQueue.emplace([=]() {
        glCall(glDeleteVertexArrays(1, &VAO));
        glCall(glDeleteBuffers(1, &VBO));
        glCall(glDeleteBuffers(1, &instanceVBO));
        glCall(glDeleteTextures(1, &textureID));
    });
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
    shader.setFloat("uPxRange", 2.0f);
    shader.setVec4("uTextColor", glm::vec4(0, 0, 0, 1));
    shader.setInt("uFontAtlas", 0);
                         // Bind sampler to texture unit 0
    glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instances.size()));
}
