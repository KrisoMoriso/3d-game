#pragma once
#include "raylib.h"


class ResourceManager {
public:

    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

    static ResourceManager& Get() {
        static ResourceManager instance;
        return instance;
    }


    Material WORLD_MATERIAL;
    Texture2D CROSSHAIR_TEXTURE;
    void init();

private:
    ResourceManager() {};

};
