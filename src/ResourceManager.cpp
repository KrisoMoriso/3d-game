#include "ResourceManager.h"

void ResourceManager::init() {

    Texture texture = LoadTexture("texture_atlas.png");
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    WORLD_MATERIAL = LoadMaterialDefault();
    SetMaterialTexture(&WORLD_MATERIAL, MATERIAL_MAP_ALBEDO, texture);
}
