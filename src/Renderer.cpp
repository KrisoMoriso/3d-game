#include "Renderer.h"

#include <format>
#include <thread>

#include "Game.h"
#include "raymath.h"
#include "ResourceManager.h"
#include "rlgl.h"

bool Renderer::are_chunk_neighbours_ready(World::ChunkPos chunk_pos){
    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                World::ChunkPos target_pos = {chunk_pos.x + dx, chunk_pos.y + dy, chunk_pos.z + dz};

                if (target_pos.y < 0 || target_pos.y >= World::WORLD_CHUNK_HEIGHT) {
                    continue;
                }
                auto it = Game::Get().m_world.m_chunks.find(target_pos);
                if (it != Game::Get().m_world.m_chunks.end() and !it->second->m_is_generating) {
                } else {
                    return false;
                }
            }
        }
    }
    return true;
}

Renderer::MeshJob Renderer::pack_mesh_job(World::ChunkPos chunk_pos){
    MeshJob mesh_job;
    mesh_job.chunk_pos = chunk_pos;

    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int index = (dx + 1) + (dy + 1) * 3 + (dz + 1) * 9;
                World::ChunkPos target_pos = {chunk_pos.x + dx, chunk_pos.y + dy, chunk_pos.z + dz};

                auto it = Game::Get().m_world.m_chunks.find(target_pos);
                if (it != Game::Get().m_world.m_chunks.end() and !it->second->m_is_generating) {
                    mesh_job.neighbour_chunks[index] = it->second;
                    mesh_job.do_neighbour_exists[index] = true;
                } else {
                    mesh_job.neighbour_chunks[index] = nullptr;
                    mesh_job.do_neighbour_exists[index] = false;
                }
            }
        }
    }
    // Set center shortcut for convenience
    mesh_job.center_chunk = mesh_job.neighbour_chunks[13]; //13 is the center chunk
    return mesh_job;


}

void Renderer::update_mesh_chunk(MeshJob mesh_job, ThreadPool::SafeQueue<MeshResult>& result_queue)
{


    std::vector<std::shared_lock<std::shared_mutex>> locks;
    locks.reserve(27);
    for (int i = 0; i < 27; ++i) {
        if (mesh_job.do_neighbour_exists[i]) {
            locks.emplace_back(mesh_job.neighbour_chunks[i]->m_block_mutex);
        }
    }
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<unsigned short> indices;
    std::vector<unsigned char> shades;
    World::ChunkPos chunk_pos = mesh_job.chunk_pos;
    const Block* current_block = mesh_job.center_chunk->m_blocks.data();

    int indice_counter = 0;
    for (int x = 0; x < 16; ++x){
        for (int y = 0; y < 16; ++y){
            for (int z = 0; z < 16; ++z){
                if (current_block->m_material_type != 0){
                    //culling
                    //check if block is at chunk edge
                    //check X-
                    if (!is_solid(mesh_job, x - 1, y, z)){
                        add_face(2, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }
                    //X+
                    // X+
                    if (!is_solid(mesh_job, x + 1, y, z)) {
                        add_face(3, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }
                    // Z-
                    if (!is_solid(mesh_job, x, y, z - 1)) {
                        add_face(1, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }
                    // Z+
                    if (!is_solid(mesh_job, x, y, z + 1)) {
                        add_face(0, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }
                    // Y-
                    if (!is_solid(mesh_job, x, y - 1, z)) {
                        add_face(5, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }
                    // Y+
                    if (!is_solid(mesh_job, x, y + 1, z)) {
                        add_face(4, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter, mesh_job);
                    }

                }
                current_block++;
            }
        }
    }
    MeshResult mesh_result;
    mesh_result.indices = indices;
    mesh_result.shades = shades;
    mesh_result.texcoords = texcoords;
    mesh_result.vertices = vertices;
    mesh_result.chunk_pos = chunk_pos;
    result_queue.push(std::move(mesh_result));
}


Vector2 Renderer::get_atlas_coords(unsigned short material_type, int face_id){
    float atlas_pos_x = 0.0f;
    float atlas_pos_y = 0.0f;
    if (material_type == World::BLOCK_MATERIALS::DIRT){
        //default
    } else if (material_type == World::BLOCK_MATERIALS::GRASS_BLOCK){
        if (face_id <= 3){
            atlas_pos_x = 1.0f;
        } else if (face_id == 4){
            atlas_pos_x = 2.0f;
        } else if (face_id == 5){
            //default
        }
    } else if (material_type == World::BLOCK_MATERIALS::STONE){
        atlas_pos_y = 1.0f;
    } else if (material_type == World::BLOCK_MATERIALS::OAK_PLANKS){
        atlas_pos_y = 2.0f;
    } else if (material_type == World::BLOCK_MATERIALS::OAK_LOG){
        if (face_id <= 3){
            atlas_pos_x = 1.0f;
            atlas_pos_y = 2.0f;
        } else{
            atlas_pos_x = 2.0f;
            atlas_pos_y = 2.0f;
        }
    } else if (material_type == World::BLOCK_MATERIALS::SAND){
        atlas_pos_y = 3.0f;
    }
    return Vector2{atlas_pos_x, atlas_pos_y};
}


void Renderer::add_face(int face_id, int x, int y, int z,
                        unsigned short block_material,
                        std::vector<float>& vertices,
                        std::vector<float>& texcoords,
                        std::vector<unsigned short>& indices,
                        std::vector<unsigned char>& shades,
                        int& indice_counter, const MeshJob& job
){
    vertices.reserve(12);
    for (int i = 0; i < 4; ++i){
        vertices.push_back(m_face_vertices[face_id][i].x + x);
        vertices.push_back(m_face_vertices[face_id][i].y + y);
        vertices.push_back(m_face_vertices[face_id][i].z + z);
    }
    shades.reserve(16);
    unsigned char face_shade = m_shades[face_id];
    unsigned char ao_results[4];

    for (int i = 0; i < 4; ++i){
        const auto& aov = ao_neighbors[face_id][i];
        unsigned char ao = compute_ao(job, x, y, z,
            aov.s1.dx, aov.s1.dy, aov.s1.dz,
            aov.s2.dx, aov.s2.dy, aov.s2.dz,
            aov.corner.dx, aov.corner.dy, aov.corner.dz);

        ao_results[i] = ao;
        unsigned char combined = (unsigned char)((int)face_shade * ao / 255);

        shades.push_back(combined);
        shades.push_back(combined);
        shades.push_back(combined);
        shades.push_back(255); // alpha
    }

    int offset = indice_counter * 4;

    if (ao_results[0] + ao_results[2] > ao_results[1] + ao_results[3]) {
        // Split along diagonal 0-2
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);
    } else {
        // Split along diagonal 1-3
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
    }

    indice_counter++;

    Vector2 atlas_coords = get_atlas_coords(block_material, face_id);


    for (auto uv : m_face_UVs){
        texcoords.push_back((uv[0] + atlas_coords.x) * 0.0625f);
        texcoords.push_back((uv[1] + atlas_coords.y) * 0.0625f);
    }


}
bool Renderer::is_solid(const MeshJob& job, int x, int y, int z) {
    int dx = (x < 0) ? -1 : (x > 15 ? 1 : 0);
    int dy = (y < 0) ? -1 : (y > 15 ? 1 : 0);
    int dz = (z < 0) ? -1 : (z > 15 ? 1 : 0);

    // 2. Get the neighbor index
    int index = (dx + 1) + (dy + 1) * 3 + (dz + 1) * 9;

    // 3. Safety check
    if (!job.do_neighbour_exists[index]) return false;

    // 4. Wrap local coordinates to 0-15
    // (val % 16 + 16) % 16 handles negative coordinates correctly
    int lx = ((x % 16) + 16) % 16;
    int ly = ((y % 16) + 16) % 16;
    int lz = ((z % 16) + 16) % 16;

    return job.neighbour_chunks[index]->m_blocks[lz + ly * 16 + lx * 256].m_material_type != World::BLOCK_MATERIALS::AIR;
}



unsigned char Renderer::compute_ao(const MeshJob& job, int x, int y, int z,
                                   int dx1, int dy1, int dz1,   // side 1
                                   int dx2, int dy2, int dz2,   // side 2
                                   int dcx, int dcy, int dcz)   // corner diagonal
{
    bool side1  = is_solid(job, x + dx1, y + dy1, z + dz1);
    bool side2  = is_solid(job, x + dx2, y + dy2, z + dz2);
    bool corner = is_solid(job, x + dcx, y + dcy, z + dcz);

    int ao;
    if (side1 && side2)
        ao = 0;
    else
        ao = 3 - (int(side1) + int(side2) + int(corner));

    // ao is 0..3; map to shade (e.g. 140, 175, 210, 255)
    constexpr unsigned char ao_table[4] = { 140, 175, 210, 255 };
    return ao_table[ao];
}
void Renderer::render_chunks(Vector3 player_pos)
{
    World::ChunkPos player_chunk = World::get_chunk_position(player_pos);
    int render_distance = Game::Get().RENDER_DISTANCE;

    Frustum frustum = extract_frustum();

    for (auto& pair : m_chunk_meshes){
        World::ChunkPos chunk_pos = pair.first;
        if (pair.first.x >= player_chunk.x - render_distance and pair.first.x <= player_chunk.x + render_distance
                and pair.first.z >= player_chunk.z - render_distance and pair.first.z <= player_chunk.z + render_distance){


            Vector3 chunk_min = { float(chunk_pos.x * 16), float(chunk_pos.y * 16), float(chunk_pos.z * 16) };
            Vector3 chunk_max = { float(chunk_pos.x * 16 + 16), float(chunk_pos.y * 16 + 16), float(chunk_pos.z * 16 + 16) };

            if (check_aabb_against_frustum(frustum, chunk_min, chunk_max)){
                DrawMesh(pair.second, ResourceManager::Get().WORLD_MATERIAL, MatrixTranslate(float(chunk_pos.x *16), float(chunk_pos.y *16), float(chunk_pos.z *16)));
            }
        }
    }
}


void Renderer::upload_mesh_to_gpu(const MeshResult& mesh){
    std::vector<float> vertices = mesh.vertices;
    std::vector<float> texcoords = mesh.texcoords;
    std::vector<unsigned short> indices = mesh.indices;
    std::vector<unsigned char> shades = mesh.shades;
    World::ChunkPos chunk_pos = mesh.chunk_pos;
    Mesh cubeMesh = {0};
    cubeMesh.vertexCount = vertices.size()/3;
    cubeMesh.triangleCount = indices.size()/3;
    cubeMesh.vertices = (float *)MemAlloc(cubeMesh.vertexCount * 3 * sizeof(float));
    cubeMesh.texcoords = (float *)MemAlloc(cubeMesh.vertexCount * 2 * sizeof(float));
    cubeMesh.indices = (unsigned short *)MemAlloc(cubeMesh.triangleCount * 3 * sizeof(unsigned short));
    cubeMesh.colors = (unsigned char *)MemAlloc(cubeMesh.vertexCount * 4 * sizeof(unsigned char));

    memcpy(cubeMesh.vertices, vertices.data(), vertices.size()*sizeof(float));
    memcpy(cubeMesh.texcoords, texcoords.data(), texcoords.size() * sizeof(float));
    memcpy(cubeMesh.indices, indices.data(), indices.size()*sizeof(unsigned short));
    memcpy(cubeMesh.colors, shades.data(), shades.size()* sizeof(unsigned char) );
    UploadMesh(&cubeMesh, true);

    if (m_chunk_meshes.contains(chunk_pos)){
        UnloadMesh(m_chunk_meshes[chunk_pos]);
        m_chunk_meshes.erase(chunk_pos);
    }

    m_chunk_meshes[chunk_pos] = cubeMesh;
    auto it = Game::Get().m_world.m_chunks.find(chunk_pos);

    if (it != Game::Get().m_world.m_chunks.end()) it->second->m_is_meshing = false;
}

void Renderer::update_mesh(Vector3 player_pos){
    MeshResult finished_mesh;
    while (m_result_queue_priority.try_pop(finished_mesh)){
        upload_mesh_to_gpu(finished_mesh);
    }
    while (m_result_queue.try_pop(finished_mesh)){
        upload_mesh_to_gpu(finished_mesh);
    }

    World::ChunkPos chunk_pos = World::get_chunk_position(player_pos);
    int render_distance = Game::Get().RENDER_DISTANCE;
    bool needs_sorting = false;


    for (int x = chunk_pos.x - render_distance; x <= chunk_pos.x + render_distance; ++x) {
        for (int z = chunk_pos.z - render_distance; z <= chunk_pos.z + render_distance; ++z) {
            for (int y = 0; y < World::WORLD_CHUNK_HEIGHT; ++y) {

                World::ChunkPos pos = {x, y, z};
                auto it = Game::Get().m_world.m_chunks.find(pos);

                if (it != Game::Get().m_world.m_chunks.end()) {
                    auto& chunk = it->second;
                    if (!m_chunk_meshes.contains(pos) and !chunk->m_is_meshing and !chunk->m_is_generating and are_chunk_neighbours_ready(pos)) {
                        m_queue_to_mesh.push_back(pos);
                        chunk->m_is_meshing = true;
                        needs_sorting = true;
                    }
                }
            }
        }
    }

    if (needs_sorting) {
        std::sort(m_queue_to_mesh.begin(), m_queue_to_mesh.end(),
            [chunk_pos](const World::ChunkPos& a, const World::ChunkPos& b) {

                int distA = (a.x - chunk_pos.x)*(a.x - chunk_pos.x) + (a.z - chunk_pos.z)*(a.z - chunk_pos.z);
                int distB = (b.x - chunk_pos.x)*(b.x - chunk_pos.x) + (b.z - chunk_pos.z)*(b.z - chunk_pos.z);


                return distA > distB;
        });
    }

    int max_sends = 3;
    int sent_this_frame = 0;
    while (!m_queue_to_mesh_priority.empty()){
        World::ChunkPos pos_queue = m_queue_to_mesh_priority.front();
        m_queue_to_mesh_priority.pop();
        send_chunk_to_thread(pos_queue, true);
        sent_this_frame++;
    }
    while (!m_queue_to_mesh.empty() and sent_this_frame < max_sends){
        World::ChunkPos pos_queue = m_queue_to_mesh.back();
        m_queue_to_mesh.pop_back();
        send_chunk_to_thread(pos_queue, false);
        sent_this_frame++;
    }


    int unloaded_this_frame = 0;
    int max_unloads = 2;
    while (!m_chunks_to_unload.empty() and unloaded_this_frame < max_unloads){
        World::ChunkPos to_erase = m_chunks_to_unload.back();
        m_chunks_to_unload.pop_back();

        if (to_erase.x >= chunk_pos.x - render_distance && to_erase.x <= chunk_pos.x + render_distance &&
        to_erase.z >= chunk_pos.z - render_distance && to_erase.z <= chunk_pos.z + render_distance) {
            continue; // Chunk is valid again. Abort deletion.
        }

        auto it = m_chunk_meshes.find(to_erase);
        if (it != m_chunk_meshes.end()){
            UnloadMesh(it->second);
            m_chunk_meshes.erase(it);
            unloaded_this_frame++;

        }
    }


}

bool Renderer::send_chunk_to_thread(World::ChunkPos chunk_pos, bool is_priority){
    MeshJob mesh_job = pack_mesh_job(chunk_pos);
    if (!mesh_job.center_chunk) return false;
    if (!is_priority){
        Game::Get().m_thread_pool.enqueue([this, mesh_job](){
           this->update_mesh_chunk(mesh_job, m_result_queue);
        });


    } else{
        Game::Get().m_thread_pool.enqueue([this, mesh_job](){
           this->update_mesh_chunk(mesh_job, m_result_queue_priority);
        });
    }
    return true;
}



// Pass in the local coordinates (0-15) of the broken/placed block
void Renderer::update_block_meshes(World::ChunkPos chunk_pos, int local_x, int local_y, int local_z) {

    // Helper lambda to safely queue a chunk
    auto queue_chunk = [](World::ChunkPos pos) {
        if (Game::Get().m_world.m_chunks.count(pos) > 0) {
            auto& chunk = Game::Get().m_world.m_chunks[pos];
            if (chunk && !chunk->m_is_meshing) {
                Game::Get().m_renderer.m_queue_to_mesh_priority.emplace(pos);
                chunk->m_is_meshing = true;
            }
        }
    };

    // 1. ALWAYS remesh the chunk the block is inside of
    queue_chunk(chunk_pos);

    // 2. ONLY remesh neighbors if the block is on the edge (0 or 15)
    if (local_x == 0)  queue_chunk({chunk_pos.x - 1, chunk_pos.y, chunk_pos.z});
    if (local_x == 15) queue_chunk({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z});

    if (local_y == 0)  queue_chunk({chunk_pos.x, chunk_pos.y - 1, chunk_pos.z});
    if (local_y == 15) queue_chunk({chunk_pos.x, chunk_pos.y + 1, chunk_pos.z});

    if (local_z == 0)  queue_chunk({chunk_pos.x, chunk_pos.y, chunk_pos.z - 1});
    if (local_z == 15) queue_chunk({chunk_pos.x, chunk_pos.y, chunk_pos.z + 1});
}

Renderer::Frustum Renderer::extract_frustum() {
    Frustum f;

    Matrix vp = MatrixMultiply(rlGetMatrixModelview(), rlGetMatrixProjection());

    f.planes[0] = {vp.m3 + vp.m0, vp.m7 + vp.m4, vp.m11 + vp.m8,  vp.m15 + vp.m12}; // Left
    f.planes[1] = {vp.m3 - vp.m0, vp.m7 - vp.m4, vp.m11 - vp.m8,  vp.m15 - vp.m12}; // Right
    f.planes[2] = {vp.m3 + vp.m1, vp.m7 + vp.m5, vp.m11 + vp.m9,  vp.m15 + vp.m13}; // Bottom
    f.planes[3] = {vp.m3 - vp.m1, vp.m7 - vp.m5, vp.m11 - vp.m9,  vp.m15 - vp.m13}; // Top
    f.planes[4] = {vp.m3 + vp.m2, vp.m7 + vp.m6, vp.m11 + vp.m10, vp.m15 + vp.m14}; // Near
    f.planes[5] = {vp.m3 - vp.m2, vp.m7 - vp.m6, vp.m11 - vp.m10, vp.m15 - vp.m14}; // Far

    // Normalize each plane for accurate distance calculations
    for (int i = 0; i < 6; i++) {
        float length = sqrt(f.planes[i].x * f.planes[i].x +
                            f.planes[i].y * f.planes[i].y +
                            f.planes[i].z * f.planes[i].z);
        f.planes[i].x /= length;
        f.planes[i].y /= length;
        f.planes[i].z /= length;
        f.planes[i].d /= length;
    }

    return f;
}

bool Renderer::check_aabb_against_frustum(const Frustum& frustum, Vector3 min, Vector3 max) {
    for (int i = 0; i < 6; i++) {
        Vector3 p = min;
        if (frustum.planes[i].x >= 0) p.x = max.x;
        if (frustum.planes[i].y >= 0) p.y = max.y;
        if (frustum.planes[i].z >= 0) p.z = max.z;

        if (frustum.planes[i].x * p.x +
            frustum.planes[i].y * p.y +
            frustum.planes[i].z * p.z +
            frustum.planes[i].d < -0.1f) {
            return false;
        }
    }
    return true;
}