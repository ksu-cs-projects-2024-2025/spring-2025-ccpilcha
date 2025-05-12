#include <iostream>

#include "Server.hpp"
#include "common/game/GameContext.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GameContext* context;
Server* server;

int main()
{
    std::cout << "Booting server...";
    // TODO:
    // 1. create server IP and open port
    // 2. create a listener for requests to join
    // 3. maintain a local copy of player and world instances 

    context = new GameContext();
    server = new Server(context);

    
    
}