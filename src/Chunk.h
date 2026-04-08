#pragma once
#include <array>
#include <vector>

#include "Block.h"
#include "raylib.h"


class Chunk {
public:
    Vector3 m_chunk_id; //chunk coordinate system 0 0 0
    Vector3 m_position;
    std::vector<Block> m_blocks;
    Chunk(Vector3 chunk_id, Vector3 position);
    void draw();
    Chunk(){}
};
