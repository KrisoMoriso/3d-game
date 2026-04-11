#pragma once
#include <memory>
#include <unordered_map>
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
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>, ChunkPosHash> m_chunks;
    World();
    static constexpr int WORLD_CHUNK_HEIGHT = 10;
    static ChunkPos get_chunk_position(Vector3 position);
    void generate_world(Vector3 player_position);
    ChunkPos m_last_player_chunk;
    struct GenerationJob{
        ChunkPos chunk_pos;
    };
    struct GenerationResult{
        ChunkPos chunk_pos;
        std::unique_ptr<Chunk> chunk;
    };
    void generate_chunk(GenerationJob generation_job, ThreadPool::SafeQueue<GenerationResult>& queue_generation_result);
private:
    ThreadPool::SafeQueue<GenerationResult> m_queue_generation_result;
    std::queue<ChunkPos> m_queue_to_generate;
};
