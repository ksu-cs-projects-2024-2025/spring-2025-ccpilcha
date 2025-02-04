#include "Player.hpp"

Player::Player() : camera()
{
}

Player::~Player()
{
}

void Player::init()
{
    movement[0] = false;
    movement[1] = false;
    movement[2] = false;
    movement[3] = false;
    movement[4] = false;
    movement[5] = false;
}

void Player::handleEvent(GameConfiguration *c, SDL_Event *event)
{
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
    }
}

void Player::update(GameConfiguration *c, double deltaTime)
{
    // handle movement
    glm::vec3 move = glm::vec3(0.0f);
    if (movement[0]) move += camera.forward;
    if (movement[1]) move -= camera.forward;
    if (movement[2]) move -= camera.right;
    if (movement[3]) move += camera.right;
    if (movement[4]) move += camera.up;
    if (movement[5]) move -= camera.up;
    camera.translate(move * (c->moveSpeed * (float)deltaTime));
}

void Player::render(GameConfiguration *c)
{
    // not sure what would go here yet
}
