#pragma once

#include "Client.hpp"

#include <game/GameEngine.hpp>
#include <game/GameContext.hpp>
#include <enet/enet.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class World;

class Server
{
    ENetAddress address;
    ENetHost* server; // max 32 clients

    bool showLastCommand = false;
    GameConsole console;
    std::vector<std::string> history;
    int history_pos = -1;
    CommandResult lastCommand;

    GameContext *context;
    World *world;
    std::unordered_map<ClientID, std::unique_ptr<Client>> clients;

    CommandResult RunCommand(const Command& command);
public:
    Server(GameContext *c);
    Server();
    void Init();
    void Run();
};