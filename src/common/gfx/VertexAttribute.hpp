#pragma once

#include <glad/glad.h>
#include "../util/GLHelper.hpp"

struct VertexAttribute {
    int size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void* pointer;
    int divisor;

    void Enable(int index)
    {
        if (type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT)
        {
            glCall(glVertexAttribIPointer(index, size, type, stride, pointer));
        }
        else {
            glCall(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
        }
        glCall(glEnableVertexAttribArray(index));
        glCall(glVertexAttribDivisor(index, divisor));
    }

    static void Enable(int index, int size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer, int divisor)
    {
        if (type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT)
        {
            glCall(glVertexAttribIPointer(index, size, type, stride, pointer));
        }
        else {
            glCall(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
        }
        glCall(glEnableVertexAttribArray(index));
        glCall(glVertexAttribDivisor(index, divisor));
    }
};