#pragma once
#include <math.h>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Chunk.h"
#include "ThreadPool.h"

class World {
public:
    struct BLOCK_MATERIALS{
        static constexpr unsigned short AIR = 0;
        static constexpr unsigned short DIRT = 1;
        static constexpr unsigned short GRASS_BLOCK = 2;
        static constexpr unsigned short STONE = 3;
        static constexpr unsigned short OAK_PLANKS = 4;
        static constexpr unsigned short OAK_LOG = 5;
        static constexpr unsigned short SAND = 6;
        static std::string match_material(unsigned short material){
            if (material == 0){return "AIR";}
            if (material == 1){return "DIRT";}
            if (material == 2) {return "GRASS_BLOCK";}
            if (material == 3) {return "STONE";}
            if (material == 4) {return "OAK_PLANKS";}
            if (material == 5) {return "OAK_LOG";}
            if (material == 6) {return "SAND";}
            return "UNKNOWN";
        }
    };
    struct ChunkPos{
        int x, y, z;
        bool operator==(const ChunkPos& other) const{
            return x == other.x and y == other.y and z == other.z;
        }
    };
    struct ChunkPosHash{
        std::size_t operator()(const ChunkPos& pos) const{
            std::size_t hash = 0;
            auto hash_combine = [](std::size_t& hash_, int position){
              hash_ ^= std::hash<int>{}(position) + 0x9e3779b9 + (hash_ << 6) + (hash_ >> 2);
            };
            hash_combine(hash, pos.x);
            hash_combine(hash, pos.y);
            hash_combine(hash, pos.z);
            return hash;
        }
    };
    std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash> m_chunks;
    static constexpr int WORLD_CHUNK_HEIGHT = 10;
    World();
    static ChunkPos get_chunk_position(Vector3 position);
    unsigned short get_block_material(int x, int y, int z);
    void set_block_material(int x, int y, int z, unsigned short material);
    void generate_world(Vector3 player_position);
private:
    ChunkPos m_last_player_chunk;
    struct GenerationJob{
        ChunkPos chunk_pos;
    };
    struct GenerationResult{
        ChunkPos chunk_pos;
        std::unique_ptr<Chunk> chunk;
    };
    void generate_chunk(GenerationJob generation_job, ThreadPool::SafeQueue<GenerationResult>& queue_generation_result);
    struct RadarJob {
        ChunkPos center_chunk;
        int render_distance;
        std::vector<ChunkPos> active_chunk_keys;
    };

    struct RadarResult {
        std::vector<ChunkPos> chunks_to_generate;
        std::vector<ChunkPos> chunks_to_remove;
    };

    ThreadPool::SafeQueue<GenerationResult> m_queue_generation_result;
    std::vector<ChunkPos> m_queue_to_generate;
    std::unordered_set<ChunkPos, ChunkPosHash> m_chunks_in_progress;
    ThreadPool::SafeQueue<RadarResult> m_radar_results;
    void perform_radar_job(RadarJob job, ThreadPool::SafeQueue<RadarResult>& queue_radar_results);
};
