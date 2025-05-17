#pragma once

#include <vector>
#include "ChunkConstants.hpp"
#include "BlockInfo.hpp"

struct BlockInstance {
    int16_t x, y, z;
    BlockType type;
};

class Structures
{
    static std::vector<BlockInstance> Tree;
};

std::vector<BlockInstance> Structures::Tree = {
    {0, 0, 0, BlockType::Wood},
    {0, 0, 1, BlockType::Wood},
    {0, 0, 2, BlockType::Wood},
    {0, 0, 3, BlockType::Wood},
    {0, 0, 4, BlockType::Wood},
    {0, 0, 5, BlockType::Wood},
    {0, 0, 6, BlockType::Wood},
    {0, 0, 7, BlockType::Wood},
    //leaves
    {-1, 0, 3, BlockType::Leaves},
    {-1, 0, 4, BlockType::Leaves},
    {-1, 0, 5, BlockType::Leaves},
    {-1, 0, 6, BlockType::Leaves},
    {-1, 0, 7, BlockType::Leaves},
    { 1, 0, 3, BlockType::Leaves},
    { 1, 0, 4, BlockType::Leaves},
    { 1, 0, 5, BlockType::Leaves},
    { 1, 0, 6, BlockType::Leaves},
    { 1, 0, 7, BlockType::Leaves},
    { 0,-1, 3, BlockType::Leaves},
    { 0,-1, 4, BlockType::Leaves},
    { 0,-1, 5, BlockType::Leaves},
    { 0,-1, 6, BlockType::Leaves},
    { 0,-1, 7, BlockType::Leaves},
    { 0, 1, 3, BlockType::Leaves},
    { 0, 1, 4, BlockType::Leaves},
    { 0, 1, 5, BlockType::Leaves},
    { 0, 1, 6, BlockType::Leaves},
    { 0, 1, 7, BlockType::Leaves},
    { 0, 0, 8, BlockType::Leaves},
};