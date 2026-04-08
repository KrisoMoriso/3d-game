#include "Renderer.h"

#include "Game.h"
#include "ResourceManager.h"
#define OFFSET 0.5f
void Renderer::update_mesh()
{
    Mesh cubeMesh = {0};
    // cubeMesh = GenMeshCube(1, 1, 1);
    cubeMesh.vertexCount = 4;
    cubeMesh.triangleCount = 2;

    cubeMesh.vertices = (float *)MemAlloc(cubeMesh.vertexCount * 3 * sizeof(float));
    cubeMesh.texcoords = (float *)MemAlloc(cubeMesh.vertexCount * 2 * sizeof(float));
    cubeMesh.indices = (unsigned short *)MemAlloc(cubeMesh.vertexCount * 3 * sizeof(unsigned short));

    if (!cubeMesh.vertices || !cubeMesh.texcoords || !cubeMesh.indices) return;
    //front z=1.0f
    cubeMesh.vertices[0] = 0; cubeMesh.vertices[1] = 1; cubeMesh.vertices[2] = 1; //top-left
    cubeMesh.vertices[3] = 1; cubeMesh.vertices[4] = 1; cubeMesh.vertices[5] = 1; //top-right
    cubeMesh.vertices[6] = 0; cubeMesh.vertices[7] = 0; cubeMesh.vertices[8] = 1; //bottom-left
    cubeMesh.vertices[9] = 1; cubeMesh.vertices[10] = 0; cubeMesh.vertices[11] = 1; //bottom-right

    cubeMesh.texcoords[0] = 0.0f; cubeMesh.texcoords[1] = 1.0f; // Top-Left
    cubeMesh.texcoords[2] = 1.0f; cubeMesh.texcoords[3] = 1.0f; // Top-Right
    cubeMesh.texcoords[4] = 0.0f; cubeMesh.texcoords[5] = 0.0f; // Bottom-Left
    cubeMesh.texcoords[6] = 1.0f; cubeMesh.texcoords[7] = 0.0f; // Bottom-Right

    // Triangle 1: Top-Left, Bottom-Left, Top-Right
    cubeMesh.indices[0] = 0; cubeMesh.indices[1] = 2; cubeMesh.indices[2] = 1;
    // Triangle 2: Top-Right, Bottom-Left, Bottom-Right
    cubeMesh.indices[3] = 1; cubeMesh.indices[4] = 2; cubeMesh.indices[5] = 3;

    UploadMesh(&cubeMesh, true);

    m_cube_model = LoadModelFromMesh(cubeMesh);
    m_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ResourceManager::Get().DIRT_TEXTURE;
}

void Renderer::render()
{
    DrawModel(m_cube_model, {0,0,0}, 1, WHITE);
}
