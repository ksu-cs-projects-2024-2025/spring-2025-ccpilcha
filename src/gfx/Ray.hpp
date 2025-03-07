#pragma once

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    Ray() : origin(0), direction(0) {}
    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
    glm::vec3 at(float t) const {
        return origin + t * direction;
    }
};