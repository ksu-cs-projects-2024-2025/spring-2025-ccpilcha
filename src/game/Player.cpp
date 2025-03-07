#include "Player.hpp"
#include "Chunk.hpp"

#include <iostream>
#include "World.hpp"

Player::Player() : camera(), chunkPos({0,0,2}), lastPos({-1,-1,-1}), pos(0,0,0)
{
}

Player::~Player()
{
}

void Player::Init(GameContext *c, ChunkPos pos)
{
    movement[0] = false;
    movement[1] = false;
    movement[2] = false;
    movement[3] = false;
    movement[4] = false;
    movement[5] = false;

    camera.aspect = (float)c->aspectRatio;
    camera.viewport(c->aspectRatio, c->fov);
}

void Player::OnEvent(GameContext *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
        c->aspectRatio = (float) event->window.data1 / (float) event->window.data2;
        camera.viewport(c->aspectRatio, c->fov);
    }
    else if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        if (c->isFocused) camera.rotateBy(event->motion.xrel, event->motion.yrel);
    }
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP)
    {
        // handle movement
        bool isPressed = event->type == SDL_EVENT_KEY_DOWN;
        SDL_Keycode key = event->key.key;
        if (key == c->forward)          movement[0] = isPressed;
        else if (key == c->backward)    movement[1] = isPressed;
        else if (key == c->left)        movement[2] = isPressed;
        else if (key == c->right)       movement[3] = isPressed;
        else if (key == c->up)          movement[4] = isPressed;
        else if (key == c->down)        movement[5] = isPressed;
        else if (key == c->sprint)      movement[6] = isPressed;
    }
}

void Player::Update(GameContext *c, double deltaTime)
{
    // handle movement
    this->lastPos = this->chunkPos;
    glm::dvec3 move(0,0,0);
    if (movement[0]) move += camera.forward;
    if (movement[1]) move -= camera.forward;
    if (movement[2]) move -= camera.right;
    if (movement[3]) move += camera.right;
    if (movement[4]) move += camera.up;
    if (movement[5]) move -= camera.up;
    if (glm::length(move) > 0)
        move = (movement[6]*5 + 1) * c->moveSpeed * deltaTime * glm::normalize(move); // we want constant moving velocity

    this->pos += move;
    ChunkPos newPos = this->chunkPos + this->pos;
    if (newPos != this->chunkPos)
    {
        this->chunkPos = newPos; // so now we have shifted
        this->pos = Chunk::remainder(this->pos);
    }
    camera.translate(pos);

}

void Player::Render(GameContext *c)
{
    // not sure what would go here yet
}
