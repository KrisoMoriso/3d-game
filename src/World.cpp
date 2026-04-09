#include "World.h"
#include "Chunk.h"
World::World()
{

    for (int x = -10; x < 10; ++x){
        for (int y = 0; y < 6; ++y){
            for (int z = -10; z < 10; ++z){
                m_chunks[{x, y, z}] = std::make_unique<Chunk>();
            }
        }
    }
    for (auto& block : m_chunks.find({7, 5, 7})->second->m_blocks){
        block.m_material_type = 0;
    }
    // m_chunks.find({7, 5, 7})->second->m_blocks[2730].m_material_type = 1;
    m_chunks.find({7, 5, 7})->second->m_blocks[2731].m_material_type = 1;
    m_chunks.find({7, 5, 7})->second->m_blocks[2732].m_material_type = 1;
    m_chunks.find({7, 5, 7})->second->m_blocks[2733].m_material_type = 1;



}



