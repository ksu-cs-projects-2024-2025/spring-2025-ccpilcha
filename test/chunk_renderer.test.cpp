#include <glad/glad.h>

#include <gtest/gtest.h>
#include "gfx/ChunkRenderer.hpp"
#include "game/World.hpp"
#include "game/GameContext.hpp"
#include "game/Player.hpp"
#include <iostream>
#include <chrono>
#include <thread>

Player *plr;
GameContext *c;
std::unique_ptr<World> world;

void basics()
{
    plr = new Player();
    c = new GameContext();
    c->plr = plr;
    world = std::make_unique<World>();
    world->Init(c);
}

TEST(ChunkRendererTest, BasicChunkAllocation2) {
    // arrange
    basics();
    ChunkRenderer renderer(world.get());
    
    // assert
    ASSERT_TRUE(true);
    world.reset();
}