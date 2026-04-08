#pragma once
#include <vector>
#include "Chunk.h"

class World {
public:
    std::vector<std::vector<std::vector<Chunk>>> m_chunks;
    World();
};
