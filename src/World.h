#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "Chunk.h"
class World {
public:
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
    static const int WORLD_CHUNK_HEIGHT = 6;
    static ChunkPos get_chunk_position(Vector3 position);

};
