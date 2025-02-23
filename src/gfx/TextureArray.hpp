#pragma once

#include <glad/glad.h>

class TextureArray
{
    unsigned int id;
    int width, height, nrChannels;
public:
    TextureArray(const char* path);
    void use(GLenum texture);
    ~TextureArray();
};