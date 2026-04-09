#include "World.h"
#include "Chunk.h"
World::World()
{
    // m_chunks.resize(1);
    // m_chunks[0].resize(1);
    // m_chunks[0][0].emplace_back();
    // m_chunks[0][0][0].m_blocks[4085-256].m_material_type = 0;
    // m_chunks[0][0][0].m_blocks[4085-256-16].m_material_type = 0;
    m_chunks[{0, 0, 0}] = std::make_unique<Chunk>();

}



