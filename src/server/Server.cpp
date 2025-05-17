#include "Server.hpp"

Server::Server(GameContext *c) : context(c)
{
    address.host = ENET_HOST_ANY;
    address.port = 12345;

    server = enet_host_create(&address, 32, 2, 0, 0); // max 32 clients

}

void Server::Init()
{

}

void Server::Run()
{
    ENetEvent event;
    while (enet_host_service(server, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_CONNECT:
                // Add new player to map
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                // Remove player
                break;
        }
    }
    
}