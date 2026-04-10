#include "World.h"
#include "Chunk.h"
World::World()
{

    for (int x = -60; x < 60; ++x){
        for (int y = 0; y < WORLD_CHUNK_HEIGHT; ++y){
            for (int z = -60; z < 60; ++z){
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

World::ChunkPos World::get_chunk_position(Vector3 position){
    ChunkPos chunk_pos;
    chunk_pos.x = (int)position.x >> 4;
    chunk_pos.y = (int)position.y >> 4;
    chunk_pos.z = (int)position.z >> 4;
    return chunk_pos;
}

