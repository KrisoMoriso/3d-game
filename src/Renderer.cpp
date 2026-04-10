#include "Renderer.h"

#include <thread>

#include "Game.h"
#include "ResourceManager.h"
void Renderer::update_mesh_chunk(MeshJob mesh_job, SafeQueue<MeshResult>& result_queue)
{
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<unsigned short> indices;
    std::vector<unsigned char> shades;
    World::ChunkPos chunk_pos = mesh_job.chunk_pos;
    auto chunk = mesh_job.chunks.find(chunk_pos);
    auto& blocks = chunk->second.m_blocks;

    Block* current_block = &blocks[0];

    int indice_counter = 0;
    for (int x = 0; x < 16; ++x){
        for (int y = 0; y < 16; ++y){
            for (int z = 0; z < 16; ++z){
                if (current_block->m_material_type == 1){
                    //culling
                    //check if block is at chunk edge
                    //check X-
                    if (x != 0 and (current_block-256)->m_material_type == 0){
                        add_face(2, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x -1, chunk_pos.y, chunk_pos.z})){
                        if (x == 0  and mesh_job.chunks.find({chunk_pos.x -1, chunk_pos.y, chunk_pos.z})->
                            second.m_blocks[z + y * 16 + (15)*256].m_material_type == 0){
                            add_face(2, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                        }

                    } else if (x == 0){
                        add_face(2, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //X+
                    if (x != 15 and (current_block+256)->m_material_type == 0){
                        add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z})){
                        if (x == 15  and mesh_job.chunks.find({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z})->
                            second.m_blocks[z + y * 16 + (0)*256].m_material_type == 0){
                            add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (x == 15){
                        add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Z-
                    if (z != 0 and (current_block-1)->m_material_type == 0){
                        add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x, chunk_pos.y, chunk_pos.z-1})){
                        if (z == 0  and mesh_job.chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z-1})->
                            second.m_blocks[15 + y * 16 + x*256].m_material_type == 0){
                            add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 0){
                        add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //z+
                    if (z != 15 and (current_block+1)->m_material_type == 0){
                        add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x, chunk_pos.y, chunk_pos.z+1})){
                        if (z == 15  and mesh_job.chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z+1})->
                            second.m_blocks[0 + y * 16 + x*256].m_material_type == 0){
                            add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 15){
                        add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Y-
                    if (y != 0 and (current_block-16)->m_material_type == 0){
                        add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x, chunk_pos.y-1, chunk_pos.z})){
                        if (y == 0  and mesh_job.chunks.find({chunk_pos.x, chunk_pos.y-1, chunk_pos.z})->
                            second.m_blocks[15 + (15) * 16 + x*256].m_material_type == 0){
                            add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (y == 0){
                        add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //y+
                    if (y != 15 and (current_block+16)->m_material_type == 0){
                        add_face(4, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (mesh_job.chunks.contains({chunk_pos.x, chunk_pos.y+1, chunk_pos.z})){
                        if (y == 15  and mesh_job.chunks.find({chunk_pos.x, chunk_pos.y+1, chunk_pos.z})->
                            second.m_blocks[15 + (0) * 16 + x*256].m_material_type == 0){
                            add_face(4, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (y == 15){
                        add_face(4, x, y, z, vertices, texcoords, indices, shades, indice_counter);
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
    for (auto uv : m_face_UVs){ texcoords.push_back(uv); }
}

void Renderer::render_chunks()
{
    for (auto& pair : m_chunk_models){
        World::ChunkPos chunk_pos = pair.first;
        DrawModel(pair.second, Vector3{float(chunk_pos.x *16),float(chunk_pos.y *16),float(chunk_pos.z *16)}, 1, WHITE);

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

        if (m_chunk_models.contains(chunk_pos)){
            UnloadModel(m_chunk_models[chunk_pos]);
            m_chunk_models.erase(chunk_pos);
        }

        m_chunk_models[chunk_pos] = LoadModelFromMesh(cubeMesh);
        m_chunk_models[chunk_pos].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ResourceManager::Get().DIRT_TEXTURE;
        m_active_threads--;
        Game::Get().m_world.m_chunks[chunk_pos]->m_is_meshing = false;

    }


    World::ChunkPos chunk_pos = World::get_chunk_position(player_pos);
    int render_distance = 8;
    int sent_this_frame = 0;
    int max_sends = 1;
    for (int x = chunk_pos.x - render_distance; x < chunk_pos.x + render_distance; ++x){
        for (int y = 0; y < World::WORLD_CHUNK_HEIGHT; ++y){
            for (int z = chunk_pos.z - render_distance; z < chunk_pos.z + render_distance; ++z){
                World::ChunkPos position = {x, y, z};
                if (!m_chunk_models.contains(position)){
                    if (m_active_threads < 6 and !Game::Get().m_world.m_chunks[position]->m_is_meshing and sent_this_frame < max_sends){
                        send_chunk_to_thread(position);
                        Game::Get().m_world.m_chunks[position]->m_is_meshing = true;
                        m_active_threads++;
                        sent_this_frame++;
                    }
                }
            }
        }
    }
    std::vector<World::ChunkPos> models_to_erase;
    models_to_erase.reserve(25);
    for (auto pair : m_chunk_models){
        if (!(pair.first.x >= chunk_pos.x - render_distance and pair.first.x <= chunk_pos.x + render_distance
            and pair.first.z >= chunk_pos.z - render_distance and pair.first.z <= chunk_pos.z + render_distance)){
            UnloadModel(pair.second);
            models_to_erase.push_back(pair.first);
        }
    }
    for (auto to_erase : models_to_erase){
        m_chunk_models.erase(to_erase);
    }
}

void Renderer::send_chunk_to_thread(World::ChunkPos chunk_pos){
    MeshJob mesh_job;
    mesh_job.chunk_pos = chunk_pos;
    auto chunk_0 = Game::Get().m_world.m_chunks.find(chunk_pos);
    if (chunk_0 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_pos] = *(chunk_0->second);
    }
    auto chunk_1 = Game::Get().m_world.m_chunks.find({chunk_pos.x - 1, chunk_pos.y, chunk_pos.z});
    if (chunk_1 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_1->first] = *(chunk_1->second);
    }
    auto chunk_2 = Game::Get().m_world.m_chunks.find({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z});
    if (chunk_2 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_2->first] = *(chunk_2->second);
    }
    auto chunk_3 = Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y - 1, chunk_pos.z});
    if (chunk_3 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_3->first] = *(chunk_3->second);
    }
    auto chunk_4 = Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y + 1, chunk_pos.z});
    if (chunk_4 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_4->first] = *(chunk_4->second);
    }
    auto chunk_5 = Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z - 1});
    if (chunk_5 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_5->first] = *(chunk_5->second);
    }
    auto chunk_6 = Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z + 1});
    if (chunk_6 != Game::Get().m_world.m_chunks.end()){
        mesh_job.chunks[chunk_6->first] = *(chunk_6->second);
    }
    std::thread t(&Renderer::update_mesh_chunk, this, std::move(mesh_job), std::ref(m_result_queue));
    t.detach();
}

