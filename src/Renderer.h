#pragma once
#include <queue>
#include <vector>

#include "raylib.h"
#include "World.h"
class Renderer {
public:
    //safe queue for threads
    template<typename T>
    class SafeQueue{
    private:
        std::queue<T> m_queue;
        std::mutex m_mutex;
    public:
        void push(T item){
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(item));
        }
        bool try_pop(T& item){
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) return false;
            item = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }
    };

    Renderer(){}
    Model m_cube_model;
    std::unordered_map<World::ChunkPos, Model, World::ChunkPosHash> m_chunk_models;
    const Vector3 m_face_vertices[6][4]{
        { {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1} }, // Front (Z+)
        { {1,0,0}, {0,0,0}, {0,1,0}, {1,1,0} }, // Back (Z-)
        { {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0} }, // Left (X-)
        { {1,0,1}, {1,0,0}, {1,1,0}, {1,1,1} }, // Right (X+)
        { {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0} }, // Top (Y+)
        { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} }  // Bottom (Y-)
    };
    const unsigned short m_face_indices[6]
    {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };
    const float m_face_UVs[8]
    {
        0.0f, 0.0f,  // Bottom-Left
    1.0f, 0.0f,  // Bottom-Right
    1.0f, 1.0f,  // Top-Right
    0.0f, 1.0f   // Top-Left
    };
    const unsigned char m_shades[6]{
        200,
        160,
        200,
        160,
        255,
        130
    };
    struct MeshResult{
        std::vector<float> vertices;
        std::vector<float> texcoords;
        std::vector<unsigned short> indices;
        std::vector<unsigned char> shades;
        World::ChunkPos chunk_pos;
    };
    struct MeshJob{
        World::ChunkPos chunk_pos;
        // std::unordered_map<World::ChunkPos, Chunk, World::ChunkPosHash> chunks;
        std::vector<Block> center_blocks;
        std::vector<Block> neighbour_blocks[6];
        bool do_neighbour_exists[6];
    };
    MeshJob pack_mesh_job(World::ChunkPos chunk_pos);
    void update_mesh_chunk(MeshJob mesh_job, SafeQueue<MeshResult>& result_queue);
    void update_mesh(Vector3 player_pos);
    void add_face(int face_id, int x, int y, int z,
                    std::vector<float>& vertices,
                    std::vector<float>& texcoords,
                    std::vector<unsigned short>& indices,
                    std::vector<unsigned char>& shades,
                    int& indice_counter);
    void render_chunks();
    void send_chunk_to_thread(World::ChunkPos chunk_pos);
    SafeQueue<MeshResult> m_result_queue;
    std::queue<World::ChunkPos> m_queue_to_mesh;
    World::ChunkPos m_last_player_chunk;
    std::vector<World::ChunkPos> m_chunks_to_unload;
};
