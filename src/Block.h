#pragma once
#include "raylib.h"


class Block {
    public:

    unsigned short m_material_type;
    Block();
    Block(int material_type);
};
