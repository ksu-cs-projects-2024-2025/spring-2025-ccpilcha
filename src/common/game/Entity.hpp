#pragma once

#include <boost/uuid.hpp>
#include <glm/glm.hpp>

#include "game/GameContext.hpp"

enum class EntityType { Particle, Player, Mob, Object };

class Entity {
public:
    boost::uuids::uuid id;
    glm::vec3 position, face, velocity;
    EntityType type;

    void Update(GameContext *c, double delta);
    void Render(GameContext *c);
};