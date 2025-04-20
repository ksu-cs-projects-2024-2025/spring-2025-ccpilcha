#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <xstring>

struct UIComponent {
    // Geometry
    glm::vec2 origin;
    glm::vec2 radius;
    
    // Appearance
    float borderWidth;
    glm::vec4 mainColor;
    glm::vec4 shadowColor;
    glm::vec4 highlightColor;
    
    // Content
    std::u32string text;
    glm::vec2 textAnchor;
    GLuint texturebg;
    GLuint texturefg;
    
    // Interaction
    std::function<void()> action;
    bool hover = false;
    bool select = false;
    
    // Utility: Check if a point is inside the component's bounds.
    bool isInside(glm::vec2 p) const {
        glm::vec2 minCorner = origin - radius;
        glm::vec2 maxCorner = origin + radius;
        return minCorner.x <= p.x && minCorner.y <= p.y 
            && maxCorner.x >= p.x && maxCorner.y >= p.y;
    }
};