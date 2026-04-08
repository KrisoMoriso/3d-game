#include "Renderer.h"

#include "Game.h"
#include "ResourceManager.h"
#define OFFSET 0.5f
void Renderer::update_mesh()
{
    Mesh cubeMesh = {0};
    cubeMesh.vertexCount = 4;
    cubeMesh.triangleCount = 2;

    cubeMesh.vertices = (float *)MemAlloc(cubeMesh.vertexCount * 3 * sizeof(float));
    cubeMesh.texcoords = (float *)MemAlloc(cubeMesh.vertexCount * 2 * sizeof(float));
    cubeMesh.indices = (unsigned short *)MemAlloc(cubeMesh.vertexCount * 3 * sizeof(unsigned short));
    //front z=1.0f
    cubeMesh.vertices[0] = 0; cubeMesh.vertices[1] = 1; cubeMesh.vertices[2] = 1; //top-left
    cubeMesh.vertices[3] = 1; cubeMesh.vertices[4] = 1; cubeMesh.vertices[5] = 1; //top-right
    cubeMesh.vertices[6] = 0; cubeMesh.vertices[7] = 0; cubeMesh.vertices[8] = 1; //bottom-left
    cubeMesh.vertices[9] = 1; cubeMesh.vertices[10] = 0; cubeMesh.vertices[11] = 1; //bottom-right

    m_cube_model = LoadModelFromMesh(cubeMesh);
    m_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ResourceManager::Get().DIRT_TEXTURE;
}

void Renderer::render()
{
    DrawModel(m_cube_model, {0,0,0}, 1, WHITE);
}
