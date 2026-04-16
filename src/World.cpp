#include "World.h"
#include "Chunk.h"
#include "Game.h"
#define FASTNOISELITE_IMPLEMENTATION
#include "FastNoiseLite.h"
#include <cmath>
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
    // TraceLog(LOG_WARNING, "chunks: %d | meshes: %d | in_progress: %d | gen_queue: %d",
    // (int)m_chunks.size(),
    // (int)Game::Get().m_renderer.m_chunk_meshes.size(),
    // (int)m_chunks_in_progress.size(),
    // (int)m_queue_to_generate.size());
     ChunkPos chunk_pos = get_chunk_position(player_position);

    RadarResult radar_result;
    if (m_radar_results.try_pop(radar_result)) {


        int deleted = 0;
        for (const auto& pos : radar_result.chunks_to_remove) {
            // if (deleted >= 4) break;

            if (m_chunks.contains(pos) && !m_chunks[pos]->m_is_generating) {
                m_chunks.erase(pos);
                Game::Get().m_renderer.add_chunk_to_unload(pos);
                deleted++;
            }
        }
        for (const auto& pos : m_queue_to_generate) {
            m_chunks_in_progress.erase(pos);
        }
        m_queue_to_generate.clear();
        for (const auto& pos : radar_result.chunks_to_generate) {
            if (!m_chunks.contains(pos) && !m_chunks_in_progress.contains(pos)) {
                m_queue_to_generate.push_back(pos);
                m_chunks_in_progress.insert(pos);
            }
        }
    }


    if (chunk_pos != m_last_player_chunk or true) {


        std::vector<ChunkPos> active_keys;
        active_keys.reserve(m_chunks.size());
        for (const auto& pair : m_chunks) {
            active_keys.push_back(pair.first);
        }

        RadarJob job = {chunk_pos, Game::Get().RENDER_DISTANCE, std::move(active_keys)};

        Game::Get().m_thread_pool.enqueue([this, job]() {
            this->perform_radar_job(job, m_radar_results);
        });

        m_last_player_chunk = chunk_pos;
    }


    int constexpr max_generates = 4;
    int generates_this_frame = 0;
    while (!m_queue_to_generate.empty() && generates_this_frame < max_generates) {
        ChunkPos pos_queue = m_queue_to_generate.back();
        m_queue_to_generate.pop_back();

        GenerationJob job = {pos_queue};
        Game::Get().m_thread_pool.enqueue([this, job]() {
            this->generate_chunk(job, m_queue_generation_result);
        });

        generates_this_frame++;
    }


     GenerationResult finished_result;
    int constexpr max_finishes = 4;
    int finishes = 0;

    while (m_queue_generation_result.try_pop(finished_result) && finishes < max_finishes) {

        m_chunks[finished_result.chunk_pos] = std::move(finished_result.chunk);
        m_chunks[finished_result.chunk_pos]->m_is_generating = false;
        m_chunks_in_progress.erase(finished_result.chunk_pos);
        finishes++;
    }
}

void World::generate_chunk(GenerationJob generation_job,
    ThreadPool::SafeQueue<GenerationResult>& queue_generation_result){

    FastNoiseLite noise_macro; //continentalism
    noise_macro.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise_macro.SetSeed(Game::Get().WORLD_SEED);
    noise_macro.SetFrequency(0.0013f);
    noise_macro.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise_macro.SetFractalOctaves(3);

    // FastNoiseLite noise_macro2;
    // noise_macro2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    // noise_macro2.SetSeed(Game::Get().WORLD_SEED + 10);
    // noise_macro2.SetFrequency(0.008f);
    // noise_macro2.SetFractalType(FastNoiseLite::FractalType_Ridged);
    // noise_macro2.SetFractalOctaves(2);

    FastNoiseLite noise_micro;
    noise_micro.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise_micro.SetSeed(Game::Get().WORLD_SEED + 1);
    noise_micro.SetFrequency(0.005f);// smaller frequency for smaller bumps

    FastNoiseLite noise_caves;
    noise_caves.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise_caves.SetSeed(Game::Get().WORLD_SEED + 2);
    noise_caves.SetFrequency(0.015f); // frequency determines how tight the tunnels twist
    noise_caves.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise_caves.SetFractalOctaves(2);

    FastNoiseLite noise_cave_mask;
    noise_cave_mask.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise_cave_mask.SetSeed(Game::Get().WORLD_SEED + 3);
    noise_cave_mask.SetFrequency(0.005f);

    GenerationResult result;
    result.chunk_pos = generation_job.chunk_pos;
    result.chunk = std::make_unique<Chunk>();

    for (int x = 0; x <= 15; ++x) {
        for (int z = 0; z <= 15; ++z) {
            int world_x = result.chunk_pos.x * 16 + x;
            int world_z = result.chunk_pos.z * 16 + z;

            float val_macro = noise_macro.GetNoise((float)world_x, (float)world_z);
            float val_micro = noise_micro.GetNoise((float)world_x, (float)world_z);

            // int macro_sign = val_macro > 0 ? 1 : -1;
            // float shaped_macro = val_macro * val_macro * macro_sign;
            // float shaped_macro = val_macro;
            float shaped_macro = val_macro - std::abs(val_micro) * 0.3f;
            int base_height = 90;
            int surface_y = base_height + (int)(shaped_macro * 60.0f) + (int)(val_micro * 6.0f);
            constexpr int SAND_LEVEL = 35;
            constexpr int MOUNTAIN_LEVEL = 110;

            for (int y = 0; y <= 15; ++y) {
                int world_y = result.chunk_pos.y * 16 + y;
                unsigned short current_material = BLOCK_MATERIALS::AIR;

                if (world_y > surface_y){
                    current_material = BLOCK_MATERIALS::AIR;
                }
                else if (world_y == surface_y){
                    if (world_y < SAND_LEVEL){
                        current_material = BLOCK_MATERIALS::SAND;

                    } else if (surface_y > MOUNTAIN_LEVEL){
                        current_material = BLOCK_MATERIALS::STONE;
                    } else{
                        current_material = BLOCK_MATERIALS::GRASS_BLOCK;
                    }
                }
                else if (world_y > surface_y -4){
                    if (surface_y < SAND_LEVEL){
                        current_material = BLOCK_MATERIALS::SAND;
                    } else if (surface_y > MOUNTAIN_LEVEL){
                        current_material = BLOCK_MATERIALS::STONE;
                    } else{
                        current_material = BLOCK_MATERIALS::DIRT;
                    }
                } else{
                        current_material = BLOCK_MATERIALS::STONE;

                }

                if (current_material != BLOCK_MATERIALS::AIR) {

                    float mask_val = noise_cave_mask.GetNoise((float)world_x, (float)world_y, (float)world_z);

                    if (mask_val > 0.4f) {

                        float cave_val = noise_caves.GetNoise((float)world_x, (float)world_y, (float)world_z);

                        float base_threshold = 0.12f;

                        float carve_threshold = base_threshold * mask_val;

                        int depth_from_surface = surface_y - world_y;
                        if (depth_from_surface <= 5) {
                            carve_threshold += (5 - depth_from_surface) * 0.008f;
                        }

                        if (std::abs(cave_val) < carve_threshold) {
                            current_material = BLOCK_MATERIALS::AIR;
                        }
                    }
                }
                result.chunk->setBlock(x, y, z, current_material);
            }
        }
    }

    queue_generation_result.push(std::move(result));
}


void World::perform_radar_job(RadarJob job, ThreadPool::SafeQueue<RadarResult>& queue_radar_results){
    RadarResult result;
    int render_dist = job.render_distance + 2;

    for (const auto& pos : job.active_chunk_keys) {
        if (std::abs(pos.x - job.center_chunk.x) > render_dist ||
            std::abs(pos.z - job.center_chunk.z) > render_dist) {
            result.chunks_to_remove.push_back(pos);
            }
    }


    for (int x = job.center_chunk.x - render_dist; x < job.center_chunk.x + render_dist; ++x) {
        for (int y = 0; y < WORLD_CHUNK_HEIGHT; ++y) {
            for (int z = job.center_chunk.z - render_dist; z < job.center_chunk.z + render_dist; ++z) {
                result.chunks_to_generate.push_back({x, y, z});
            }
        }
    }


    std::sort(result.chunks_to_generate.begin(), result.chunks_to_generate.end(),
        [&job](const ChunkPos& a, const ChunkPos& b) {
            int distA = (a.x - job.center_chunk.x)*(a.x - job.center_chunk.x) +
                        (a.z - job.center_chunk.z)*(a.z - job.center_chunk.z);
            int distB = (b.x - job.center_chunk.x)*(b.x - job.center_chunk.x) +
                        (b.z - job.center_chunk.z)*(b.z - job.center_chunk.z);
            return distA > distB;
    });


    queue_radar_results.push(std::move(result));
}








World::ChunkPos World::get_chunk_position(Vector3 position){
    ChunkPos chunk_pos;
    chunk_pos.x = static_cast<int>(std::floor(position.x / 16.0f));
    chunk_pos.y = static_cast<int>(std::floor(position.y / 16.0f));
    chunk_pos.z = static_cast<int>(std::floor(position.z / 16.0f));
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
        {
            std::unique_lock<std::shared_mutex> lock(chunk_it->second->m_block_mutex);
            chunk_it->second->setBlock(block_x, block_y, block_z, material);
        }
    }

}

