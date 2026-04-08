#pragma once
#include <array>
#include <vector>

#include "Block.h"
#include "raylib.h"


class Chunk {
public:
    std::vector<Block> m_blocks;
    Chunk();
    unsigned short getBlockMaterial(Vector3 position);//position relative to the chunk



};
