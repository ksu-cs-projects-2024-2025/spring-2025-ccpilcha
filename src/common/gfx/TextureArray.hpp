#pragma once

#include <string>

#include <glad/glad.h>

struct GameContext;

class TextureArray
{
    std::string path;
    unsigned int id;
    int width, height, nrChannels;
public:
    TextureArray(const std::string& path);
    void Init(GameContext *c);
    void use(GLenum texture);
    ~TextureArray();
};