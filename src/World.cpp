#include "World.h"
#include "Chunk.h"
#include "Game.h"
#define FASTNOISELITE_IMPLEMENTATION
#include "FastNoiseLite.h"
World::World()
{

    m_chunks[{0, WORLD_CHUNK_HEIGHT - 1, 0}] = std::make_unique<Chunk>();
    for (auto& block : m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks){
        block.m_material_type = 0;
    }
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2730].m_material_type = BLOCK_MATERIALS::GRASS_BLOCK;
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2731].m_material_type = BLOCK_MATERIALS::GRASS_BLOCK;
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2732].m_material_type = BLOCK_MATERIALS::DIRT;
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2733].m_material_type = BLOCK_MATERIALS::DIRT;
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2765].m_material_type = BLOCK_MATERIALS::OAK_PLANKS;
    m_chunks.find({0, WORLD_CHUNK_HEIGHT - 1, 0})->second->m_blocks[2797].m_material_type = BLOCK_MATERIALS::OAK_LOG;

}

void World::generate_world(Vector3 player_position){
     ChunkPos chunk_pos = get_chunk_position(player_position);
    int render_distance = Game::Get().RENDER_DISTANCE + 2;
    if (chunk_pos != m_last_player_chunk or true){
        for (int x = chunk_pos.x - render_distance; x < chunk_pos.x + render_distance; ++x){
            for (int y = 0; y < WORLD_CHUNK_HEIGHT; ++y){
                for (int z = chunk_pos.z - render_distance; z < chunk_pos.z + render_distance; ++z){
                    ChunkPos position = {x, y, z};
                    if (!m_chunks.contains(position)){
                        m_chunks[position] = std::make_unique<Chunk>();
                        m_chunks[position]->m_is_generating = true;
                        m_queue_to_generate.emplace(position);
                    }
                }
            }
        }
}

    int constexpr max_generates = 24;
    int generates_this_frame = 0;
    while (!m_queue_to_generate.empty() and generates_this_frame < max_generates){
        ChunkPos pos_queue = m_queue_to_generate.front();
        m_queue_to_generate.pop();
        GenerationJob job = {pos_queue};
        Game::Get().m_thread_pool.enqueue([this, job](){
            this->generate_chunk(job, m_queue_generation_result);
        });
        generates_this_frame++;
    }
    GenerationResult finished_result;
    while (m_queue_generation_result.try_pop(finished_result)){
        m_chunks[finished_result.chunk_pos] = std::move(finished_result.chunk);
        m_chunks[finished_result.chunk_pos]->m_is_generating = false;
    }

}

void World::generate_chunk(GenerationJob generation_job,
    ThreadPool::SafeQueue<GenerationResult>& queue_generation_result){
    FastNoiseLite noise_lite;
    noise_lite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise_lite.SetSeed(Game::Get().WORLD_SEED);
    noise_lite.SetFrequency(0.005f);
    noise_lite.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise_lite.SetFractalOctaves(4);

    GenerationResult result;
    result.chunk_pos = generation_job.chunk_pos;
    result.chunk = std::make_unique<Chunk>();

    for (int x = 0; x <= 15; ++x) {
        for (int z = 0; z <= 15; ++z) {
            int world_x = result.chunk_pos.x * 16 + x;
            int world_z = result.chunk_pos.z * 16 + z;

            // 1. Get 2D noise (returns -1.0 to 1.0)
            float noise_val = noise_lite.GetNoise((float)world_x, (float)world_z);

            // 2. Calculate the surface height
            int base_height = 80;
            int height_variation = 16;
            int surface_y = base_height + (int)(noise_val * height_variation);


            for (int y = 0; y <= 15; ++y) {
                int world_y = result.chunk_pos.y * 16 + y;

                if (world_y <= surface_y){
                    result.chunk->setBlock(x, y, z, BLOCK_MATERIALS::STONE);
                } else{
                    result.chunk->setBlock(x, y, z, BLOCK_MATERIALS::AIR);
                }
            }
        }
    }
    for (int x = 0; x <= 15; ++x) {
        for (int y = 2; y <= 14; ++y) {
            for (int z = 0; z <= 15; ++z) {
                int world_y = result.chunk_pos.y * 16 + y;
                if (result.chunk->getBlockMaterial(x, y, z) == BLOCK_MATERIALS::STONE and result.chunk->getBlockMaterial(x, y + 1, z) == BLOCK_MATERIALS::AIR ){
                    if (world_y <= 80){
                        result.chunk->setBlock(x, y, z, BLOCK_MATERIALS::SAND);
                        result.chunk->setBlock(x, y - 1, z, BLOCK_MATERIALS::SAND);
                        result.chunk->setBlock(x, y - 2, z, BLOCK_MATERIALS::SAND);
                    } else{
                        result.chunk->setBlock(x, y, z, BLOCK_MATERIALS::GRASS_BLOCK);
                        result.chunk->setBlock(x, y - 1, z, BLOCK_MATERIALS::DIRT);
                        result.chunk->setBlock(x, y - 2, z, BLOCK_MATERIALS::DIRT);

                    }

                }
            }
        }
    }



    queue_generation_result.push(std::move(result));
}


World::ChunkPos World::get_chunk_position(Vector3 position){
    ChunkPos chunk_pos;
    chunk_pos.x = (int)position.x >> 4;
    chunk_pos.y = (int)position.y >> 4;
    chunk_pos.z = (int)position.z >> 4;
    return chunk_pos;
}

unsigned short World::get_block_material(int x, int y, int z){
    int chunk_x = x >> 4;
    int chunk_y = y >> 4;
    int chunk_z = z >> 4;

    int block_x = x & 15;
    int block_y = y & 15;
    int block_z = z & 15;

    auto chunk_it = m_chunks.find({chunk_x, chunk_y, chunk_z});

    if (chunk_it != m_chunks.end()){
        return chunk_it->second->getBlockMaterial(block_x, block_y, block_z);
    }
    return 0;
}

void World::set_block_material(int x, int y, int z, unsigned short material){
    int chunk_x = x >> 4;
    int chunk_y = y >> 4;
    int chunk_z = z >> 4;

    int block_x = x & 15;
    int block_y = y & 15;
    int block_z = z & 15;

    auto chunk_it = m_chunks.find({chunk_x, chunk_y, chunk_z});

    if (chunk_it != m_chunks.end()){
        chunk_it->second->setBlock(block_x, block_y, block_z, material);
    }

}

