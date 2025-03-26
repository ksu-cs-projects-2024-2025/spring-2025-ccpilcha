#pragma once

#include <glm/glm.hpp>
#include "game/ChunkPos.hpp"

struct Ray {
    ChunkPos originC;
    glm::vec3 origin;
    glm::vec3 direction;
    Ray() : origin(0), direction(0) {}
    Ray(glm::vec3 origin, glm::vec3 direction, ChunkPos cPos) : origin(origin), direction(direction), originC(cPos) {}
    glm::vec3 at(float t) const {
        return origin + t * direction;
    }
};