#include "Renderer.h"

#include "Game.h"
#include "ResourceManager.h"
void Renderer::update_mesh_chunk(World::ChunkPos chunk_pos)
{
    // todo:  REMEMBER TO UNLOAD MESHES !!!!
    // if (m_cube_model.meshes[0].vaoId > 0){
    //     UnloadMesh(m_cube_model.meshes[0]);
    // }
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<unsigned short> indices;
    std::vector<unsigned char> shades;
    m_chunk_positions.push_back(chunk_pos);

    auto chunk = Game::Get().m_world.m_chunks.find(chunk_pos);
    auto& blocks = chunk->second->m_blocks;

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
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x -1, chunk_pos.y, chunk_pos.z})){
                        if (x == 0  and Game::Get().m_world.m_chunks.find({chunk_pos.x -1, chunk_pos.y, chunk_pos.z})->
                            second->m_blocks[z + y * 16 + (15)*256].m_material_type == 0){
                            add_face(2, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                        }

                    } else if (x == 0){
                        add_face(2, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //X+
                    if (x != 15 and (current_block+256)->m_material_type == 0){
                        add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z})){
                        if (x == 15  and Game::Get().m_world.m_chunks.find({chunk_pos.x + 1, chunk_pos.y, chunk_pos.z})->
                            second->m_blocks[z + y * 16 + (0)*256].m_material_type == 0){
                            add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (x == 15){
                        add_face(3, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Z-
                    if (z != 0 and (current_block-1)->m_material_type == 0){
                        add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x, chunk_pos.y, chunk_pos.z-1})){
                        if (z == 0  and Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z-1})->
                            second->m_blocks[15 + y * 16 + x*256].m_material_type == 0){
                            add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 0){
                        add_face(1, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //z+
                    if (z != 15 and (current_block+1)->m_material_type == 0){
                        add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x, chunk_pos.y, chunk_pos.z+1})){
                        if (z == 15  and Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y, chunk_pos.z+1})->
                            second->m_blocks[0 + y * 16 + x*256].m_material_type == 0){
                            add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (z == 15){
                        add_face(0, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //Y-
                    if (y != 0 and (current_block-16)->m_material_type == 0){
                        add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x, chunk_pos.y-1, chunk_pos.z})){
                        if (y == 0  and Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y-1, chunk_pos.z})->
                            second->m_blocks[15 + (15) * 16 + x*256].m_material_type == 0){
                            add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                            }
                    } else if (y == 0){
                        add_face(5, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    //y+
                    if (y != 15 and (current_block+16)->m_material_type == 0){
                        add_face(4, x, y, z, vertices, texcoords, indices, shades, indice_counter);
                    }
                    if (Game::Get().m_world.m_chunks.contains({chunk_pos.x, chunk_pos.y+1, chunk_pos.z})){
                        if (y == 15  and Game::Get().m_world.m_chunks.find({chunk_pos.x, chunk_pos.y+1, chunk_pos.z})->
                            second->m_blocks[15 + (0) * 16 + x*256].m_material_type == 0){
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

    m_chunk_models.emplace_back(LoadModelFromMesh(cubeMesh));
    m_chunk_models[m_chunk_models.size() - 1].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ResourceManager::Get().DIRT_TEXTURE;
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
    for (int i = 0; i < m_chunk_models.size(); i++){
        World::ChunkPos chunk_pos = m_chunk_positions[i];
        DrawModel(m_chunk_models[i], Vector3{float(chunk_pos.x *16),float(chunk_pos.y *16),float(chunk_pos.z *16)}, 1, WHITE);

    }
}
void Renderer::update_mesh(){
    for (int x = -10; x < 10; ++x){
        for (int y = 0; y < 6; ++y){
            for (int z = -10; z < 10; ++z){
                update_mesh_chunk({x,y,z});
            }
        }
    }
}


