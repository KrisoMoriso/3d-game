#pragma once
#include "raylib.h"


class Block {
    public:
    // Vector3 m_position;
    // Vector3 m_size;
    // Color m_color;
    // Vector3 m_position_middle;
    // Vector3 m_position_in_chunk;
    unsigned short m_material_type;
    Block();
    // Block(Vector3 position, int material_type);
    // void draw() const;
    // Block(Vector3 chunk_position, Vector3 position_in_chunk);
    Block(int material_type);
};
