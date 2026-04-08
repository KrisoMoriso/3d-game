#include "Renderer.h"

#include "Game.h"
#include "ResourceManager.h"
void Renderer::update_mesh()
{
    if (m_cube_model.meshes[0].vaoId > 0){
        UnloadMesh(m_cube_model.meshes[0]);
    }
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<unsigned short> indices;


    Chunk& chunk = Game::Get().m_world.m_chunks[0][0][0];
    auto& blocks = chunk.m_blocks;

    Block* current_block = &blocks[0];

    int indice_counter = 0;
    for (int x = 0; x < 16; ++x){
        for (int y = 0; y < 16; ++y){
            for (int z = 0; z < 16; ++z){
                if (current_block->m_material_type == 1){
                    //culling
                    //check if block is at chunk edge
                    //check X-
                    if (x == 0 or (current_block-256)->m_material_type == 0){
                        add_face(2, x, y, z, vertices, texcoords, indices, indice_counter);

                    }
                    if (x == 15 or (current_block+256)->m_material_type == 0){
                        add_face(3, x, y, z, vertices, texcoords, indices, indice_counter);
                    }
                    if (z == 0 or (current_block-1)->m_material_type == 0){
                        add_face(1, x, y, z, vertices, texcoords, indices, indice_counter);
                    }
                    if (z == 15 or (current_block+1)->m_material_type == 0){
                        add_face(0, x, y, z, vertices, texcoords, indices, indice_counter);
                    }
                    if (y == 0 or (current_block-16)->m_material_type == 0){
                        add_face(5, x, y, z, vertices, texcoords, indices, indice_counter);
                    }
                    if (y == 15 or (current_block+16)->m_material_type == 0){
                        add_face(4, x, y, z, vertices, texcoords, indices, indice_counter);
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


    memcpy(cubeMesh.vertices, vertices.data(), vertices.size()*sizeof(float));
    memcpy(cubeMesh.texcoords, texcoords.data(), texcoords.size() * sizeof(float));
    memcpy(cubeMesh.indices, indices.data(), indices.size()*sizeof(unsigned short));
    UploadMesh(&cubeMesh, true);

    m_cube_model = LoadModelFromMesh(cubeMesh);
    m_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ResourceManager::Get().DIRT_TEXTURE;
}

void Renderer::add_face(int face_id, int x, int y, int z,
    std::vector<float>& vertices,
    std::vector<float>& texcoords,
    std::vector<unsigned short>& indices,
    int& indice_counter
    ){
    for (int i = 0; i < 12; ++i){
        switch (i%3){
        case 0: vertices.push_back(m_face_vertices[face_id][i] + x); break;
        case 1: vertices.push_back(m_face_vertices[face_id][i] + y); break;
        case 2: vertices.push_back(m_face_vertices[face_id][i] + z); break;
        }
    }
    for (auto index : m_face_indices){ indices.push_back(index + indice_counter*4); }
    indice_counter++;
    for (auto uv : m_face_UVs){ texcoords.push_back(uv); }
}

void Renderer::render()
{
    DrawModel(m_cube_model, {0,0,0}, 1, WHITE);
}


