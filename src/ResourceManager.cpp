#include "ResourceManager.h"

void ResourceManager::init() {

    Texture texture = LoadTexture("texture_atlas.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    WORLD_MATERIAL = LoadMaterialDefault();
    SetMaterialTexture(&WORLD_MATERIAL, MATERIAL_MAP_ALBEDO, texture);
    Shader voxelShader = LoadShader("src/voxel.vs.glsl", "src/voxel.fs.glsl");
    WORLD_MATERIAL.shader = voxelShader;
    CROSSHAIR_TEXTURE = LoadTexture("crosshair.png");
    SetTextureFilter(CROSSHAIR_TEXTURE, TEXTURE_FILTER_POINT);
}
