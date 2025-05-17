#pragma once

#include "game/Player.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using ClientID = boost::uuids::uuid;  // or std::string, or boost::uuids::uuid

struct Client {
    Player plr;
    ClientID id;
};