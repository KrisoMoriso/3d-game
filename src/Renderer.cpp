#include "Renderer.h"

#include <thread>

#include "Game.h"
#include "raymath.h"
#include "ResourceManager.h"

Renderer::MeshJob Renderer::pack_mesh_job(World::ChunkPos chunk_pos){
    MeshJob mesh_job;
    mesh_job.chunk_pos = chunk_pos;
    const World::ChunkPos offsets[6] = {
        {chunk_pos.x - 1, chunk_pos.y, chunk_pos.z}, // Left
        {chunk_pos.x + 1, chunk_pos.y, chunk_pos.z}, // Right
        {chunk_pos.x, chunk_pos.y, chunk_pos.z - 1},  // Back
        {chunk_pos.x, chunk_pos.y, chunk_pos.z + 1}, // Front
        {chunk_pos.x, chunk_pos.y - 1, chunk_pos.z}, // Bottom
        {chunk_pos.x, chunk_pos.y + 1, chunk_pos.z} // Top
    };
    auto it_center_chunk = Game::Get().m_world.m_chunks.find(chunk_pos);
    if (it_center_chunk != Game::Get().m_world.m_chunks.end()){
        mesh_job.center_blocks = it_center_chunk->second->m_blocks;
    }
    for (int i = 0; i < 6; ++i){
        auto it_neighbour_blocks = Game::Get().m_world.m_chunks.find(offsets[i]);

        if (it_neighbour_blocks != Game::Get().m_world.m_chunks.end() and !it_neighbour_blocks->second->m_is_generating){
            mesh_job.neighbour_blocks[i] = it_neighbour_blocks->second->m_blocks;
            mesh_job.do_neighbour_exists[i] = true;
        } else{
            mesh_job.do_neighbour_exists[i] = false;
        }
    }
    return mesh_job;
}

void Renderer::update_mesh_chunk(MeshJob mesh_job, ThreadPool::SafeQueue<MeshResult>& result_queue)
{
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<unsigned short> indices;
    std::vector<unsigned char> shades;
    World::ChunkPos chunk_pos = mesh_job.chunk_pos;
    // auto chunk = mesh_job.chunks.find(chunk_pos);
    // auto& blocks = chunk->second.m_blocks;
    std::vector<Block> blocks = mesh_job.center_blocks;
    Block* current_block = &blocks[0];

    int indice_counter = 0;
    for (int x = 0; x < 16; ++x){
        for (int y = 0; y < 16; ++y){
            for (int z = 0; z < 16; ++z){
                if (current_block->m_material_type != 0){
                    //culling
                    //check if block is at chunk edge
                    //check X-
                    if (x != 0 and (current_block-256)->m_material_type == 0){
                        add_face(2, x, y, z, current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[0]){
                        if (x == 0  and mesh_job.neighbour_blocks[0][z + y * 16 + (15)*256].m_material_type == 0){
                            add_face(2, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                        }

                    } else if (x == 0){
                        add_face(2, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //X+
                    if (x != 15 and (current_block+256)->m_material_type == 0){
                        add_face(3, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[1]){
                        if (x == 15  and mesh_job.neighbour_blocks[1][z + y * 16 + (0)*256].m_material_type == 0){
                            add_face(3, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (x == 15){
                        add_face(3, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Z-
                    if (z != 0 and (current_block-1)->m_material_type == 0){
                        add_face(1, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[2]){
                        if (z == 0  and mesh_job.neighbour_blocks[2][15 + y * 16 + x*256].m_material_type == 0){
                            add_face(1, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 0){
                        add_face(1, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //z+
                    if (z != 15 and (current_block+1)->m_material_type == 0){
                        add_face(0, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[3]){
                        if (z == 15  and mesh_job.neighbour_blocks[3][0 + y * 16 + x*256].m_material_type == 0){
                            add_face(0, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 15){
                        add_face(0, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Y-
                    if (y != 0 and (current_block-16)->m_material_type == 0){
                        add_face(5, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[4]){
                        if (y == 0  and mesh_job.neighbour_blocks[4][z + (15) * 16 + x*256].m_material_type == 0){
                            add_face(5, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (y == 0){
                        add_face(5, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //y+
                    if (y != 15 and (current_block+16)->m_material_type == 0){
                        add_face(4, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.do_neighbour_exists[5]){
                        if (y == 15  and mesh_job.neighbour_blocks[5][z + (0) * 16 + x*256].m_material_type == 0){
                            add_face(4, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (y == 15){
                        add_face(4, x, y, z,current_block->m_material_type, vertices, texcoords, indices, shades, indice_counter);
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

void Renderer::add_face(int face_id, int x, int y, int z,
    unsigned short block_material,
    std::vector<float>& vertices,
    std::vector<float>& texcoords,
    std::vector<unsigned short>& indices,
    std::vector<unsigned char>& shades,
    int& indice_counter
    ){
    vertices.reserve(12);
    for (int i = 0; i < 4; ++i){
        vertices.push_back(m_face_vertices[face_id][i].x + x);
        vertices.push_back(m_face_vertices[face_id][i].y + y);
        vertices.push_back(m_face_vertices[face_id][i].z + z);
    }
    shades.reserve(16);
    for (int i = 0; i < 4; ++i){
        shades.push_back(m_shades[face_id]);
        shades.push_back(m_shades[face_id]);
        shades.push_back(m_shades[face_id]);
        shades.push_back(255);

    }

    for (auto index : m_face_indices){ indices.push_back(index + indice_counter*4); }
    indice_counter++;
    float atlas_pos_x = 0.0f;
    float atlas_pos_y = 0.0f;
    if (block_material == World::BLOCK_MATERIALS::DIRT){
        //default
    } else if (block_material == World::BLOCK_MATERIALS::GRASS_BLOCK){
        if (face_id <= 3){
            atlas_pos_x = 1.0f;
        } else if (face_id == 4){
            atlas_pos_x = 2.0f;
        } else if (face_id == 5){
            //default
        }

    } else if (block_material == World::BLOCK_MATERIALS::STONE){
        atlas_pos_y = 1.0f;
    } else if (block_material == World::BLOCK_MATERIALS::OAK_PLANKS){
        atlas_pos_y = 2.0f;
    } else if (block_material == World::BLOCK_MATERIALS::OAK_LOG){
        if (face_id <= 3){
            atlas_pos_x = 1.0f;
            atlas_pos_y = 2.0f;
        } else{
            atlas_pos_x = 2.0f;
            atlas_pos_y = 2.0f;
        }
    } else if (block_material == World::BLOCK_MATERIALS::SAND){
        atlas_pos_y = 3.0f;
    }



    for (auto uv : m_face_UVs){
        texcoords.push_back((uv[0] + atlas_pos_x) * 0.0625f);
        texcoords.push_back((uv[1] + atlas_pos_y) * 0.0625f);
    }


}

void Renderer::render_chunks(Vector3 player_pos)
{
    World::ChunkPos player_chunk = World::get_chunk_position(player_pos);
    int render_distance = Game::Get().RENDER_DISTANCE;
    for (auto& pair : m_chunk_meshes){
        World::ChunkPos chunk_pos = pair.first;
        if (pair.first.x >= player_chunk.x - render_distance and pair.first.x <= player_chunk.x + render_distance
                and pair.first.z >= player_chunk.z - render_distance and pair.first.z <= player_chunk.z + render_distance){

            DrawMesh(pair.second, ResourceManager::Get().WORLD_MATERIAL, MatrixTranslate(float(chunk_pos.x *16), float(chunk_pos.y *16), float(chunk_pos.z *16)));
        }

    }
}
void Renderer::update_mesh(Vector3 player_pos){
    MeshResult finished_mesh;
    if (m_result_queue.try_pop(finished_mesh)){
        std::vector<float> vertices = finished_mesh.vertices;
        std::vector<float> texcoords = finished_mesh.texcoords;
        std::vector<unsigned short> indices = finished_mesh.indices;
        std::vector<unsigned char> shades = finished_mesh.shades;
        World::ChunkPos chunk_pos = finished_mesh.chunk_pos;
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
        Game::Get().m_world.m_chunks[chunk_pos]->m_is_meshing = false;

    }

    World::ChunkPos chunk_pos = World::get_chunk_position(player_pos);
    int render_distance = Game::Get().RENDER_DISTANCE;
    if (chunk_pos != m_last_player_chunk){
    }
        for (int x = chunk_pos.x - render_distance; x < chunk_pos.x + render_distance; ++x){
            for (int y = 0; y < World::WORLD_CHUNK_HEIGHT; ++y){
                for (int z = chunk_pos.z - render_distance; z < chunk_pos.z + render_distance; ++z){
                    World::ChunkPos position = {x, y, z};
                    if (!m_chunk_meshes.contains(position) and Game::Get().m_world.m_chunks.contains(position)){
                        if (!Game::Get().m_world.m_chunks[position]->m_is_meshing and !Game::Get().m_world.m_chunks[position]->m_is_generating){
                            m_queue_to_mesh.emplace(position);
                            Game::Get().m_world.m_chunks[position]->m_is_meshing = true;
                        }
                    }
                }
            }
    }


    int max_sends = 12;
    int sent_this_frame = 0;
    while (!m_queue_to_mesh.empty() and sent_this_frame < max_sends){
        World::ChunkPos pos_queue = m_queue_to_mesh.front();
        m_queue_to_mesh.pop();
        send_chunk_to_thread(pos_queue);
        sent_this_frame++;
    }



    if (chunk_pos != m_last_player_chunk){
        for (const auto& pair : m_chunk_meshes){
            if (!(pair.first.x >= chunk_pos.x - render_distance and pair.first.x <= chunk_pos.x + render_distance
                and pair.first.z >= chunk_pos.z - render_distance and pair.first.z <= chunk_pos.z + render_distance
                )){
                m_chunks_to_unload.push_back(pair.first);
            }

    }
        m_last_player_chunk = chunk_pos;
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

void Renderer::send_chunk_to_thread(World::ChunkPos chunk_pos){
    MeshJob mesh_job = pack_mesh_job(chunk_pos);
    Game::Get().m_thread_pool.enqueue([this, mesh_job](){
       this->update_mesh_chunk(mesh_job, m_result_queue);
    });
}

