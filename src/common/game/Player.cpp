#include "Player.hpp"
#include "Chunk.hpp"

#include <iostream>
#include "Physics.hpp"
#include "World.hpp"

Player::Player() : camera(), chunkPos({0, 0, 2}), lastPos({-1, -1, -1}), pos(0, 0, 0), radius(0.3, 0.3, 0.9), camOffset(0.0, 0.0, 0.7), velocity(0.0), acceleration(0,0,-10)
{
}

Player::~Player()
{
}

void Player::Init(GameContext *c, ChunkPos pos)
{
    chunkPos = pos;
    movement[0] = false;
    movement[1] = false;
    movement[2] = false;
    movement[3] = false;
    movement[4] = false;
    movement[5] = false;
    movement[6] = false;
    movement[7] = false;
    movement[8] = false;

    camera.aspect = (float)c->aspectRatio;
    camera.setFOV(c->fov);
    camera.viewport(c->aspectRatio, c->fov);
}

void Player::OnEvent(GameContext *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        camera.viewport(c->aspectRatio, c->fov);
    }
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP)
    {
        if (event->key.repeat != 0)  // auto‑repeat
            return;                // ignore it
        // handle movement
        bool isPressed = event->type == SDL_EVENT_KEY_DOWN;
        SDL_Keycode key = event->key.key;
        if (key == c->forward)
            movement[0] = isPressed;
        else if (key == c->backward)
            movement[1] = isPressed;
        else if (key == c->left)
            movement[2] = isPressed;
        else if (key == c->right)
            movement[3] = isPressed;
        else if (key == c->up)
            movement[4] = isPressed;
        else if (key == c->down)
            movement[5] = isPressed;
        else if (key == c->sprint)
            movement[6] = isPressed;
        else if (key == c->zenith)
            movement[7] = isPressed;
        else if (key == c->nadir)
            movement[8] = isPressed;

        if (key == c->jump)
            jump = isPressed;
        
        if (key == c->toggleFly && isPressed) {

            nextMethod = nextMethod != Movement::Fly ? Movement::Fly : Movement::Normal;
        }

        if (key == SDLK_1)
            cursor = 1;
        if (key == SDLK_2)
            cursor = 2;
        if (key == SDLK_3)
            cursor = 3;
        if (key == SDLK_4)
            cursor = 4;
        if (key == SDLK_5)
            cursor = 5;
        if (key == SDLK_6)
            cursor = 6;
        if (key == SDLK_7)
            cursor = 7;
        if (key == SDLK_8)
            cursor = 8;
    }
    if (!c->isFocused) return;
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        if (c->isFocused)
            camera.rotateBy(event->motion.xrel, event->motion.yrel);
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        bool isPressed = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;

        if (!isPressed) placeHold = 0;

        place = isPressed && event->button.button == SDL_BUTTON_RIGHT;
        dig = isPressed && event->button.button == SDL_BUTTON_LEFT;
    }
}

void Player::Update(GameContext *c, double deltaTime)
{
    if (nextMethod == Movement::Normal || nextMethod == Movement::Swim)
    {
        // now we are swimming
        nextMethod = c->blockRegistry[c->world->GetBlockId(chunkPos, floor(pos.x), floor(pos.y), floor(pos.z))].blockType == BlockType::Water ? Movement::Swim : Movement::Normal;
    }
    
    if (currentMethod != nextMethod)
    switch (nextMethod)
    {
        case Movement::Fly:
            verticalVelocity = 0;
            break;
        case Movement::Normal:
            if (currentMethod == Movement::Swim && jump) {
                verticalVelocity = 8;
            }
            break;
        case Movement::Swim:
            break;
    }
    currentMethod = nextMethod;
    glm::dvec3 move(0, 0, 0);
    camOffsetOffset = glm::dvec3(0.0);
    // handle movement
    this->lastPos = this->chunkPos;

    if ((place || dig) && !(place && dig)) {
        if (placeHold <= 0) {
            c->world->CastRay(Ray(camPos, camera.forward, chunkPos, dig));
            placeHold = dig ? maxDig : maxPlace;
        }
        else {
            placeHold -= deltaTime;
        }
    }

    glm::dvec3 frameDelta(0.0);
    if (c->isFocused) {
        switch (this->currentMethod)
        {
            case Movement::Swim:
            {
                if (movement[0])
                    move += camera.forward;
                if (movement[1])
                    move -= camera.forward;
                if (movement[2])
                    move -= camera.right;
                if (movement[3])
                    move += camera.right;
                if (movement[7])
                    move += glm::dvec3(0.0,0.0,1.0);
                if (movement[8])
                    move -= glm::dvec3(0.0,0.0,1.0);
                

                if (glm::length(move) > 0)
                {
                    // Normalize and scale the movement vector
                    move = maxSwimSpeed * (movement[6] ? sprintSwimMult : 1.0) * glm::normalize(move);
                }
            }
            break;
            case Movement::Normal:
            {
                if (movement[0])
                    move += glm::normalize(glm::vec3(camera.forward.x, camera.forward.y, 0));
                if (movement[1])
                    move -= glm::normalize(glm::vec3(camera.forward.x, camera.forward.y, 0));
                if (movement[2])
                    move -= camera.right;
                if (movement[3])
                    move += camera.right;
                if (jump) {
                    if (onGround) 
                    {
                        verticalVelocity = 9;
                    }
                }
                

                if (glm::length(move) > 0)
                {
                    // Normalize and scale the movement vector
                    move = maxSpeed * (movement[6] ? sprintMult : 1.0) * glm::normalize(move);
                }
            }
            break;
            case Movement::Fly:
            {
                if (movement[0])
                    move += camera.forward;
                if (movement[1])
                    move -= camera.forward;
                if (movement[2])
                    move -= camera.right;
                if (movement[3])
                    move += camera.right;
                if (movement[4])
                    move += glm::dvec3(0.0,0.0,1.0);
                if (movement[5])
                    move -= glm::dvec3(0.0,0.0,1.0);
                if (movement[7])
                    move += camera.up;
                if (movement[8])
                    move -= camera.up;

                if (glm::length(move) > 0)
                {
                    // Normalize and scale the movement vector
                    move = (movement[6] * 5 + 1) * c->moveSpeed * deltaTime * glm::normalize(move);
                }
            }
            break;
        }
    }

    float targetFOV = c->fov;

    if (movement[6] && glm::length(move) > 0) targetFOV = c->fov + speedFOV;
    
    if (c->blockRegistry[c->world->GetBlockId(chunkPos, floor(camPos.x), floor(camPos.y), floor(camPos.z))].blockType == BlockType::Water)
    {
        targetFOV = targetFOV / 1.16f; // im just going with half the index of refraction
    }
    
    double t = glm::clamp(10.0 * deltaTime, 0.0, 1.0);
    if (std::abs(targetFOV - camera.fov) < 1e-1) 
        camera.setFOV(targetFOV);
    else camera.setFOV(glm::mix(camera.fov, targetFOV, t));

    glm::dvec3 targetVelocity(0.0);
    glm::dvec3 dv;
    double    dvLen;
    switch (currentMethod) {
    case Movement::Fly:
        // --- FLY MODE: no gravity, no verticalVelocity, just apply moveInput directly ---
        frameDelta = move;
        break;
    case Movement::Swim:
        targetVelocity = glm::length(move) > 0.0 ? move : glm::dvec3(0.0,0.0,-1.0);

        // how far we need to change this frame:
        dv = targetVelocity - glm::dvec3(velocity.x, velocity.y, velocity.z);
        dvLen = glm::length(dv);

        if (dvLen > 1e-8) {
            // limit the change to maxAccel * deltaTime
            double maxStep = 160.0 * maxSwimAccel * deltaTime + std::max(0.0, dvLen - maxSwimSpeed) * 2.0 * maxSwimAccel;
            if (dvLen > maxStep) {
                dv *= (maxStep / dvLen);
            }
            velocity.x += dv.x;
            velocity.y += dv.y;
            velocity.z += dv.z;
        }
        frameDelta = velocity * deltaTime;
        break;
    case Movement::Normal:
        // --- WALK MODE: gravity + persisted verticalVelocity ---
        // 1) apply gravity if airborne
        if (!onGround) {
            verticalVelocity += 3.0 * acceleration.z * deltaTime;  // e.g. -9.81
        }
        targetVelocity.x = move.x;
        targetVelocity.y = move.y;

        // how far we need to change this frame:
        dv = targetVelocity - glm::dvec3(velocity.x, velocity.y, 0);
        dvLen = glm::length(dv);

        if (dvLen > 1e-8) {
            // limit the change to maxAccel * deltaTime
            double maxStep = 160.0 * maxAccel * (onGround ? 1.0 : 0.3) * deltaTime;
            if (dvLen > maxStep) {
                dv *= (maxStep / dvLen);
            }
            velocity.x += dv.x;
            velocity.y += dv.y;
        }
        // 2) build total delta: X/Y from input, Z from vertical velocity
        velocity.z = verticalVelocity;
        frameDelta = velocity * deltaTime;
        break;
    }


    // 1) Compute the swept‐AABB bounds
    glm::dvec3 start = pos;
    glm::dvec3 radii = radius;
    glm::dvec3 end = start + frameDelta;

    // We need the min/max over the union of AABBs at t=0 and t=1
    glm::dvec3 bbMin = glm::min(start - radii, end - radii);
    glm::dvec3 bbMax = glm::max(start + radii, end + radii);

    // 2) Gather all collidable block‐centers in that region
    std::vector<glm::dvec3> blockCenters;
    for (int x = (int)std::floor(bbMin.x); x <= (int)std::floor(bbMax.x); ++x)
    {
        for (int y = (int)std::floor(bbMin.y); y <= (int)std::floor(bbMax.y); ++y)
        {
            for (int z = (int)std::floor(bbMin.z); z <= (int)std::floor(bbMax.z); ++z)
            {
                auto id = c->world->GetBlockId(this->chunkPos, x, y, z);
                if (c->blockRegistry.at(id).isCollidable)
                {
                    // push the *center* of the block cube
                    blockCenters.emplace_back(
                        double(x) + 0.5,
                        double(y) + 0.5,
                        double(z) + 0.5);
                }
            }
        }
    }

    CollisionInfo info;
    // 3) Call the swept‐AABB + sliding mover
    glm::dvec3 newPos = moveWithSweptAABB(
        start,
        radii,
        frameDelta,
        blockCenters,
        /*blockSize=*/1.0,
        &info);

    // 1) Sync position & derive actual vertical velocity
    glm::dvec3 oldPos = pos;
    pos = newPos;

    // --- now update verticalVelocity & onGround *only* in Normal mode ---
    if (currentMethod == Movement::Normal || currentMethod == Movement::Swim) {
        double actualDZ = (pos.z - oldPos.z) / deltaTime;
        verticalVelocity = actualDZ;

        if (info.hitX0 || info.hitX1) velocity.x = 0;
        if (info.hitY0 || info.hitY1) velocity.y = 0;

        // landed?
        onGround = info.hitZ0;
        if (onGround && verticalVelocity < 0.0) verticalVelocity = 0.0;

        // hit your head?
        if (info.hitZ1 && verticalVelocity > 0.0) verticalVelocity = 0.0;
    
    }

    // Check if the player has moved to a new chunk
    ChunkPos newChunkPos = this->chunkPos + this->pos;
    if (newChunkPos != this->chunkPos)
    {
        this->chunkPos = newChunkPos; // Shift to the new chunk
        this->pos = Chunk::remainder(this->pos);
    }

    camPos = pos + camOffset + camOffsetOffset;
    // Update the camera position
    camera.translate(camPos);
    
}

void Player::Render(GameContext *c)
{
    // not sure what would go here yet
}

void Player::RenderDebug(GameContext *c)
{
}
