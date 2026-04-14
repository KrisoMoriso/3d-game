#pragma once
#include <array>
#include <shared_mutex>
#include <vector>

#include "Block.h"
#include "raylib.h"


class Chunk {
public:
    std::vector<Block> m_blocks;
    Chunk();
    unsigned short getBlockMaterial(Vector3 position);//position relative to the chunk
    bool m_is_meshing = false;
    bool m_is_generating = false;
    void setBlock(int x, int y, int z, unsigned short material);
    unsigned short getBlockMaterial(int x, int y, int z);
    std::shared_mutex m_block_mutex;
};
