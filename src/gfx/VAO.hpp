#pragma once

#include "GLHelper.hpp"
#include <glad/gl.h>

class VAO
{
    unsigned int vaoID;
public:
    /**
     * @brief Construct a new VAO object
     * 
     */
    VAO() {
        glCall(glGenVertexArrays(1, &this->vaoID));
    }
    /**
     * @brief Bind the VAO to memory for use
     * 
     */
    void bind()
    {
        glCall(glBindVertexArray(this->vaoID));
    }

    /**
     * @brief Destroy the VAO object
     * 
     */
    ~VAO() {
        glCall(glDeleteVertexArrays(1, &this->vaoID));
    }
};
