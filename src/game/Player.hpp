#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "gfx/Camera.hpp"
#include "GameContext.hpp"
#include "Chunk.hpp"

class Player
{
    bool movement[9];
    bool focused = true;
    bool flying = true;
    bool jump = false;
    bool place = false;
    bool dig = false;

    double      maxSpeed    = 4.2;     // top speed in units/sec
    double      sprintMult  = 1.5;     // speed multiplier for sprinting
    double      slowMult    = 1.5;     // speed multiplier for going slow
    double      maxAccel    = 0.5;    // units/sec²

    double      speedFOV    = 15.0;     // ratio of speed to FOV

    
    double camBobZ = 0.0;         // persistent across frames
    double      placeHold   = 0.0;
    double      maxPlace    = 0.25;
    double      maxDig      = 1.0/3.0;

    double          verticalVelocity; // persists across frames
    bool            onGround;         // persists across frames

    enum Movement {
        Fly,
        Normal,
        Swim
    } moveMethod = Movement::Fly;

    glm::dvec3 camOffsetOffset; // a 3rd offset from camOffset!!! used for bobbing effects
public:
    BLOCK_ID_TYPE cursor = 1;
    ChunkPos chunkPos, lastPos;
    glm::dvec3 pos;
    glm::dvec3 camPos;
    glm::dvec3 camOffset;
    glm::dvec3 radius;
    glm::dvec3 velocity;
    glm::dvec3 acceleration;
    Camera camera;
    Player();
    ~Player();
    void Init(GameContext *c, ChunkPos pos);
    void OnEvent(GameContext *c, const SDL_Event *event);
    void Update(GameContext *c, double deltaTime);
    void Render(GameContext *c);
};