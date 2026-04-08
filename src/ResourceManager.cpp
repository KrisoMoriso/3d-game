#include "ResourceManager.h"

void ResourceManager::init() {
    // Mesh mesh = GenMeshCube(1, 1, 1);
    // DIRT_MODEL = LoadModelFromMesh(mesh);
    DIRT_TEXTURE = LoadTexture("textures/block/dirt.png");
    SetTextureFilter(DIRT_TEXTURE, TEXTURE_FILTER_POINT);
    // DIRT_MODEL.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass_texture;

}
