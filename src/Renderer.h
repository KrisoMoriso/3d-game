#pragma once
#include <queue>
#include <vector>

#include "raylib.h"
#include "ThreadPool.h"
#include "World.h"
class Renderer {
public:


    Renderer(){}
    Model m_cube_model;
    std::unordered_map<World::ChunkPos, Mesh, World::ChunkPosHash> m_chunk_meshes;
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
    const float m_face_UVs[4][2]
    {
    {1.0f, 1.0f}, // Top-Right
    {0.0f, 1.0f}, // Top-Left
        {0.0f, 0.0f}, // Bottom-Left  u  v
    {1.0f, 0.0f} // Bottom-Right
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
        std::shared_ptr<Chunk> center_chunk;
        std::shared_ptr<Chunk> neighbour_chunks[6];
        bool do_neighbour_exists[6];
    };
    MeshJob pack_mesh_job(World::ChunkPos chunk_pos);
    void update_mesh_chunk(MeshJob mesh_job, ThreadPool::SafeQueue<MeshResult>& result_queue);
    void update_mesh(Vector3 player_pos);
    void add_face(int face_id, int x, int y, int z,
                    unsigned short block_material,
                    std::vector<float>& vertices,
                    std::vector<float>& texcoords,
                    std::vector<unsigned short>& indices,
                    std::vector<unsigned char>& shades,
                    int& indice_counter);
    void render_chunks(Vector3 player_pos);
    void send_chunk_to_thread(World::ChunkPos chunk_pos, bool is_priority);
    void update_block_meshes(World::ChunkPos chunk_pos, int local_x, int local_y, int local_z);
    ThreadPool::SafeQueue<MeshResult> m_result_queue;
    ThreadPool::SafeQueue<MeshResult> m_result_queue_priority;
    std::vector<World::ChunkPos> m_queue_to_mesh;
    std::queue<World::ChunkPos> m_queue_to_mesh_priority;
    World::ChunkPos m_last_player_chunk;
    std::vector<World::ChunkPos> m_chunks_to_unload;
};
